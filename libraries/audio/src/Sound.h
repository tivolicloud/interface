//
//  Sound.h
//  libraries/audio/src
//
//  Created by Stephen Birarda on 1/2/2014.
//  Copyright 2014 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#ifndef hifi_Sound_h
#define hifi_Sound_h

#include <QRunnable>
#include <QtCore/QObject>
#include <QtNetwork/QNetworkReply>
#include <QtScript/qscriptengine.h>

#include <ResourceCache.h>

#include "AudioConstants.h"

class AudioData;
using AudioDataPointer = std::shared_ptr<const AudioData>;

Q_DECLARE_METATYPE(AudioDataPointer);

// AudioData is designed to be immutable
// All of its members and methods are const
// This makes it perfectly safe to access from multiple threads at once
class AudioData {
public:
    using AudioSample = AudioConstants::AudioSample;

    // Allocates the buffer memory contiguous with the object
    static AudioDataPointer make(uint32_t numSamples, uint32_t numChannels,
                                 const AudioSample* samples);

    uint32_t getNumSamples() const { return _numSamples; }
    uint32_t getNumChannels() const { return _numChannels; }
    const AudioSample* data() const { return _data; }
    const char* rawData() const { return reinterpret_cast<const char*>(_data); }

    float isStereo() const { return _numChannels == 2; }
    float isAmbisonic() const { return _numChannels == 4; }
    float getDuration() const { return (float)_numSamples / (_numChannels * AudioConstants::SAMPLE_RATE); }
    uint32_t getNumFrames() const { return _numSamples / _numChannels; }
    uint32_t getNumBytes() const { return _numSamples * sizeof(AudioSample); }

private:
    AudioData(uint32_t numSamples, uint32_t numChannels, const AudioSample* samples);

    const uint32_t _numSamples { 0 };
    const uint32_t _numChannels { 0 };
    const AudioSample* const _data { nullptr };
};

class Sound : public Resource {
    Q_OBJECT

public:
    Sound(const QUrl& url, bool isStereo = false, bool isAmbisonic = false);
    Sound(const Sound& other) : Resource(other), _audioData(other._audioData), _numChannels(other._numChannels) {}

    bool isReady() const { return (bool)_audioData; }

    bool isStereo() const { return _audioData ? _audioData->isStereo() : false; }
    bool isAmbisonic() const { return _audioData ? _audioData->isAmbisonic() : false; }
    float getDuration() const { return _audioData ? _audioData->getDuration() : 0.0f; }

    AudioDataPointer getAudioData() const { return _audioData; }

    int getNumChannels() const { return _numChannels; }

signals:
    void ready();

protected slots:
    void soundProcessSuccess(AudioDataPointer audioData);
    void soundProcessError(int error, QString str);
    
private:
    virtual void downloadFinished(const QByteArray& data) override;

    AudioDataPointer _audioData;

     // Only used for caching until the download has finished
    int _numChannels { 0 };
};

class SoundProcessor : public QObject, public QRunnable {
    Q_OBJECT

public:
    struct AudioProperties {
        uint8_t numChannels { 0 };
        uint32_t sampleRate { 0 };
    };

    SoundProcessor(QWeakPointer<Resource> sound, QByteArray data);

    virtual void run() override;

    QByteArray downSample(const QByteArray& rawAudioByteArray,
                          AudioProperties properties);

signals:
    void onSuccess(AudioDataPointer audioData);
    void onError(int error, QString str);

private:
    const QWeakPointer<Resource> _sound;
    const QByteArray _data;
};

typedef QSharedPointer<Sound> SharedSoundPointer;

/**jsdoc
 * An audio resource, created by {@link SoundCache.getSound}, to be played back using {@link Audio.playSound}.
 * <p>Supported file formats:</p>
 * <ul>
 *   <li><b><code>*.wav</code></b>, <b><code>*.mp3</code></b>, <b><code>*.ogg</code></b>, <b><code>*.flac</code></b>, <b><code>*.aiff</code></b> and <a target="_blank" href="https://github.com/libsndfile/libsndfile/blob/v1.0.30/src/command.c#L122">any available from here</a></li>
 *   <li><b><code>*.raw</code></b> as 48 kHz 16-bit mono</li>
 *   <li><b><code>*.stereo.raw</code></b> as 48 kHz 16-bit stereo</li>
 * </ul>
 *
 * @class SoundObject
 * @hideconstructor
 * 
 * @hifi-interface
 * @hifi-client-entity
 * @hifi-avatar
 * @hifi-server-entity
 * @hifi-assignment-client
 *
 * @property {boolean} downloaded - <code>true</code> if the sound has been downloaded and is ready to be played, otherwise 
 *     <code>false</code>. <em>Read-only.</em>
 * @property {number} duration - The duration of the sound, in seconds. <em>Read-only.</em>
 */
class SoundScriptingInterface : public QObject {
    Q_OBJECT

    Q_PROPERTY(bool downloaded READ isReady)
    Q_PROPERTY(float duration READ getDuration)

public:
    SoundScriptingInterface(const SharedSoundPointer& sound);
    const SharedSoundPointer& getSound() { return _sound; }

    bool isReady() const { return _sound ? _sound->isReady() : false; }
    float getDuration() { return _sound ? _sound->getDuration() : 0.0f; }

/**jsdoc
 * Triggered when the sound has been downloaded and is ready to be played.
 * @function SoundObject.ready
 * @returns {Signal}
 */
signals:
    void ready();

private:
    SharedSoundPointer _sound;
};

Q_DECLARE_METATYPE(SharedSoundPointer)
QScriptValue soundSharedPointerToScriptValue(QScriptEngine* engine, const SharedSoundPointer& in);
void soundSharedPointerFromScriptValue(const QScriptValue& object, SharedSoundPointer& out);

#endif // hifi_Sound_h
