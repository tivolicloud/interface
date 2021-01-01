//
//  Sound.cpp
//  libraries/audio/src
//
//  Created by Stephen Birarda on 1/2/2014.
//  Copyright 2014 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#include "Sound.h"

#include <stdint.h>

#include <glm/glm.hpp>

#include <QRunnable>
#include <QThreadPool>
#include <QDataStream>
#include <QtCore/QDebug>
#include <QtCore/QFileInfo>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <qendian.h>

#include <LimitedNodeList.h>
#include <NetworkAccessManager.h>
#include <SharedUtil.h>

#include "AudioRingBuffer.h"
#include "AudioLogging.h"
#include "AudioSRC.h"
#include "SndfileConverter.h"

int audioDataPointerMetaTypeID = qRegisterMetaType<AudioDataPointer>("AudioDataPointer");

using AudioConstants::AudioSample;

AudioDataPointer AudioData::make(uint32_t numSamples, uint32_t numChannels,
                                 const AudioSample* samples) {
    // Compute the amount of memory required for the audio data object
    const size_t bufferSize = numSamples * sizeof(AudioSample);
    const size_t memorySize = sizeof(AudioData) + bufferSize;

    // Allocate the memory for the audio data object and the buffer
    void* memory = ::malloc(memorySize);
    auto audioData = reinterpret_cast<AudioData*>(memory);
    auto buffer = reinterpret_cast<AudioSample*>(audioData + 1);
    assert(((char*)buffer - (char*)audioData) == sizeof(AudioData));

    // Use placement new to construct the audio data object at the memory allocated
    ::new(audioData) AudioData(numSamples, numChannels, buffer);

    // Copy the samples to the buffer
    memcpy(buffer, samples, bufferSize);

    // Return shared_ptr that properly destruct the object and release the memory
    return AudioDataPointer(audioData, [](AudioData* ptr) {
        ptr->~AudioData();
        ::free(ptr);
    });
}


AudioData::AudioData(uint32_t numSamples, uint32_t numChannels, const AudioSample* samples)
    : _numSamples(numSamples),
      _numChannels(numChannels),
      _data(samples)
{}

void Sound::downloadFinished(const QByteArray& data) {
    if (!_self) {
        soundProcessError(301, "Sound object has gone out of scope");
        return;
    }

    // this is a QRunnable, will delete itself after it has finished running
    auto soundProcessor = new SoundProcessor(_self, data);
    connect(soundProcessor, &SoundProcessor::onSuccess, this, &Sound::soundProcessSuccess);
    connect(soundProcessor, &SoundProcessor::onError, this, &Sound::soundProcessError);
    QThreadPool::globalInstance()->start(soundProcessor);
}

void Sound::soundProcessSuccess(AudioDataPointer audioData) {
    qCDebug(audio) << "Setting ready state for sound file" << _url.fileName();

    _audioData = std::move(audioData);
    finishedLoading(true);

    emit ready();
}

void Sound::soundProcessError(int error, QString str) {
    qCCritical(audio) << "Failed to process sound file: code =" << error << str;
    emit failed(QNetworkReply::UnknownContentError);
    finishedLoading(false);
}


SoundProcessor::SoundProcessor(QWeakPointer<Resource> sound, QByteArray data) :
    _sound(sound),
    _data(data)
{
}

void SoundProcessor::run() {
    auto sound = qSharedPointerCast<Sound>(_sound.lock());
    if (!sound) {
        emit onError(301, "Sound object has gone out of scope");
        return;
    }

    auto url = sound->getURL();
    QString fileName = url.fileName().toLower();
    qCDebug(audio) << "Processing sound file" << fileName;

    QByteArray outputAudioByteArray;
    AudioProperties properties;

    QString ext = QFileInfo(fileName).suffix();
    if (fileName.endsWith(".stereo.raw")) {
        ext = "stereo.raw";
    }

    SndfileConverter converter;
    auto converterExts = converter.getAvailableExtensions();

    if (ext == "stereo.raw") {
        qCDebug(audio) << "Processing sound of" << _data.size() << "bytes from" << fileName << "as stereo audio file.";
        properties.numChannels = 2;
        properties.sampleRate = 48000;
        outputAudioByteArray = _data;
    } else if (ext == "raw") {
        properties.numChannels = 1;
        properties.sampleRate = 48000;
        outputAudioByteArray = _data;
    } else if (converterExts.contains(ext)) {
        auto output = converter.convertToPcm(_data);
        if (!output.first.isEmpty()) {
            outputAudioByteArray = output.first;
            properties = output.second;
        }
    } else {
        qCWarning(audio) << "Unknown sound file type";
        emit onError(300, "Failed to load sound file, reason: unknown sound file type");
        return;
    }

    if (properties.sampleRate == 0) {
        qCWarning(audio) << "Unsupported" << ext.toUpper() << "file type";
        emit onError(300, "Failed to load sound file, reason: unsupported " + ext.toUpper() + " file type");
        return;
    }

    auto data = downSample(outputAudioByteArray, properties);

    int numSamples = data.size() / AudioConstants::SAMPLE_SIZE;
    auto audioData = AudioData::make(numSamples, properties.numChannels,
                                     (const AudioSample*)data.constData());
    emit onSuccess(audioData);
}

QByteArray SoundProcessor::downSample(const QByteArray& rawAudioByteArray,
                                      AudioProperties properties) {

    // we want to convert it to the format that the audio-mixer wants
    // which is signed, 16-bit, 24Khz

    if (properties.sampleRate == AudioConstants::SAMPLE_RATE) {
        // no resampling needed
        return rawAudioByteArray;
    }

    AudioSRC resampler(properties.sampleRate, AudioConstants::SAMPLE_RATE,
                       properties.numChannels);

    // resize to max possible output
    int numSourceFrames = rawAudioByteArray.size() / (properties.numChannels * AudioConstants::SAMPLE_SIZE);
    int maxDestinationFrames = resampler.getMaxOutput(numSourceFrames);
    int maxDestinationBytes = maxDestinationFrames * properties.numChannels * AudioConstants::SAMPLE_SIZE;
    QByteArray data(maxDestinationBytes, Qt::Uninitialized);

    int numDestinationFrames = resampler.render((int16_t*)rawAudioByteArray.data(),
                                                (int16_t*)data.data(),
                                                numSourceFrames);

    // truncate to actual output
    int numDestinationBytes = numDestinationFrames * properties.numChannels * sizeof(AudioSample);
    data.resize(numDestinationBytes);

    return data;
}

QScriptValue soundSharedPointerToScriptValue(QScriptEngine* engine, const SharedSoundPointer& in) {
    return engine->newQObject(new SoundScriptingInterface(in), QScriptEngine::ScriptOwnership);
}

void soundSharedPointerFromScriptValue(const QScriptValue& object, SharedSoundPointer& out) {
    if (auto soundInterface = qobject_cast<SoundScriptingInterface*>(object.toQObject())) {
        out = soundInterface->getSound();
    }
}

SoundScriptingInterface::SoundScriptingInterface(const SharedSoundPointer& sound) : _sound(sound) {
    // During shutdown we can sometimes get an empty sound pointer back
    if (_sound) {
        QObject::connect(_sound.data(), &Sound::ready, this, &SoundScriptingInterface::ready);
    }
}

Sound::Sound(const QUrl& url, bool isStereo, bool isAmbisonic) : Resource(url) {
    _numChannels = isAmbisonic ? 4 : (isStereo ? 2 : 1);
}
