#include "VorbisCodecManager.h"

#include <QtCore/QCoreApplication>

#include <PerfStat.h>

const char* VorbisCodec::NAME { "vorbis" };

void VorbisCodec::init() {
}

void VorbisCodec::deinit() {
}

bool VorbisCodec::activate() {
    CodecPlugin::activate();
    return true;
}

void VorbisCodec::deactivate() {
    CodecPlugin::deactivate();
}

bool VorbisCodec::isSupported() const {
    return true;
}

Encoder* VorbisCodec::createEncoder(int sampleRate, int numChannels) {
    ret = vorbis_encode_init_vbr(&vi, numChannels, sampleRate, 0.4);
    
    vorbis_analysis_init(&vd, &vi);
    vorbis_block_init(&vd, &vb);

    return this;
}

Decoder* VorbisCodec::createDecoder(int sampleRate, int numChannels) {
    return this;
}

void VorbisCodec::releaseEncoder(Encoder* encoder) {
    vorbis_info_clear(&vi);
}

void VorbisCodec::releaseDecoder(Decoder* decoder) {
    // do nothing
}

void VorbisCodec::encode(const QByteArray& decodedBuffer, QByteArray& encodedBuffer) {


    // float **buffer=vorbis_analysis_buffer(&vd,READ);

    encodedBuffer = decodedBuffer;
}

void VorbisCodec::decode(const QByteArray& encodedBuffer, QByteArray& decodedBuffer) {
    decodedBuffer = encodedBuffer;
}

void VorbisCodec::lostFrame(QByteArray& decodedBuffer) {
    memset(decodedBuffer.data(), 0, decodedBuffer.size());
}
