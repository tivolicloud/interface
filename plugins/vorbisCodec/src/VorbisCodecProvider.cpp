#include <mutex>

#include <QtCore/QObject>
#include <QtCore/QtPlugin>
#include <QtCore/QStringList>

#include <plugins/RuntimePlugin.h>
#include <plugins/CodecPlugin.h>

#include "VorbisCodecManager.h"

class VorbisCodecProvider : public QObject, public CodecProvider {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID CodecProvider_iid FILE "plugin.json")
    Q_INTERFACES(CodecProvider)

public:
    VorbisCodecProvider(QObject* parent = nullptr) : QObject(parent) {}
    virtual ~VorbisCodecProvider() {}

    virtual CodecPluginList getCodecPlugins() override {
        static std::once_flag once;
        std::call_once(once, [&] {

            CodecPluginPointer vorbisCodec(new VorbisCodec());
            if (vorbisCodec->isSupported()) {
                _codecPlugins.push_back(vorbisCodec);
            }

        });
        return _codecPlugins;
    }

private:
    CodecPluginList _codecPlugins;
};

#include "VorbisCodecProvider.moc"
