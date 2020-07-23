#ifndef hifi__VorbisCodecManager_h
#define hifi__VorbisCodecManager_h

#include <plugins/CodecPlugin.h>

#include <vorbis/vorbisenc.h>

class VorbisCodec : public CodecPlugin, public Encoder, public Decoder {
    Q_OBJECT

public:
    // Plugin functions
    bool isSupported() const override;
    const QString getName() const override { return NAME; }

    void init() override;
    void deinit() override;

    /// Called when a plugin is being activated for use.  May be called multiple times.
    bool activate() override;
    /// Called when a plugin is no longer being used.  May be called multiple times.
    void deactivate() override;

    virtual Encoder* createEncoder(int sampleRate, int numChannels) override;
    virtual Decoder* createDecoder(int sampleRate, int numChannels) override;
    virtual void releaseEncoder(Encoder* encoder) override;
    virtual void releaseDecoder(Decoder* decoder) override;

    virtual void encode(const QByteArray& decodedBuffer, QByteArray& encodedBuffer) override;

    virtual void decode(const QByteArray& encodedBuffer, QByteArray& decodedBuffer) override;

    virtual void lostFrame(QByteArray& decodedBuffer) override;

private:
    static const char* NAME;

    vorbis_info vi;
    vorbis_dsp_state vd;
    vorbis_block vb;

    int ret = -1;
};

#endif // hifi__VorbisCodecManager_h
