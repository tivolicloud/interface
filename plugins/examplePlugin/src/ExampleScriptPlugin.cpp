//
//  ExampleScriptPlugin.cpp
//  plugins/examplePlugin/src
//
//  Example of prototyping new JS APIs by leveraging the existing plugin system

#include "ExampleScriptPlugin.h"

#include <QCoreApplication>
#include <QtCore/QJsonObject>
#include <QtCore/QLoggingCategory>
#include <QtCore/QThread>
#include <QtCore/QTimer>

#include <SharedUtil.h>
#include <AvatarHashMap.h>

namespace custom_api_example {

QLoggingCategory logger{ "custom_api_example" };

class ExamplePlugin : public example::ScriptPlugin {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "ExamplePlugin" FILE "plugin.json")
public:
    ExamplePlugin() : example::ScriptPlugin("ExamplePlugin", "1.0.0") {
        qCInfo(logger) << "plugin loaded" << qApp << toString() << QThread::currentThread(); 
    }

public slots:
    /**jsdoc
     * Returns current microseconds (usecs) since Epoch. note: 1000usecs == 1ms
     * @example <caption>Measure current setTimeout accuracy.</caption>
     * var expected = 1000;
     * var start = ExamplePlugin.now();
     * Script.setTimeout(function () {
     *     var elapsed = (ExamplePlugin.now() - start)/1000;
     *     print("expected (ms):", expected, "actual (ms):", elapsed);
     * }, expected);
     */
    QVariant now() const {
        return usecTimestampNow();
    }

    /**jsdoc
     * Returns the available blendshape names for an avatar.
     * @example <caption>Get blendshape names</caption>
     * print(JSON.stringify(ExamplePlugin.getBlendshapeNames(MyAvatar.sessionUUID)));
     */
    QStringList getBlendshapeNames(const QUuid& avatarID) const {
        QVector<QString> out;
        if (auto head = getAvatarHead(avatarID)) {
            for (const auto& kv : head->getBlendshapeMap().toStdMap()) {
                if (kv.second >= out.size()) out.resize(kv.second+1);
                out[kv.second] = kv.first;
            }
        }
        return out.toList();
    }

    /**jsdoc
     * Returns a key-value object with active (non-zero) blendshapes.
     * eg: { JawOpen: 1.0, ... }
     * @example <caption>Get active blendshape map</caption>
     * print(JSON.stringify(ExamplePlugin.getActiveBlendshapes(MyAvatar.sessionUUID)));
     */
    QVariant getActiveBlendshapes(const QUuid& avatarID) const {
        if (auto head = getAvatarHead(avatarID)) {
            return head->toJson()["blendShapes"].toVariant();
        }
        return {};
    }

    QVariant getBlendshapeMapping(const QUuid& avatarID) const {
        QVariantMap out;
        if (auto head = getAvatarHead(avatarID)) {
            for (const auto& kv : head->getBlendshapeMap().toStdMap()) {
                out[kv.first] = kv.second;
            }
        }
        return out;
    }

    QVariant getBlendshapes(const QUuid& avatarID) const {
        QVariantMap result;
        if (auto head = getAvatarHead(avatarID)) {
            QStringList names = getBlendshapeNames(avatarID);
            auto states = head->getBlendshapeStates();
            result = {
                { "base", zipNonZeroValues(names, states.base) },
                { "summed", zipNonZeroValues(names, states.summed) },
                { "transient", zipNonZeroValues(names, states.transient) },
            };
        }
        return result;
    }

private:
    static QVariantMap zipNonZeroValues(const QStringList& keys, const QVector<float>& values) {
        QVariantMap out;
        for (int i=1; i < values.size(); i++) {
            if (fabs(values[i]) > 1.0e-6f) {
                out[keys.value(i)] = values[i];
            }
        }
        return out;
    }
    struct _HeadHelper : public HeadData {
        QMap<QString,int> getBlendshapeMap() const {
            return BLENDSHAPE_LOOKUP_MAP;
        }
        struct States { QVector<float> base, summed, transient; };
        States getBlendshapeStates() const {
            return {
                _blendshapeCoefficients,
                _summedBlendshapeCoefficients,
                _transientBlendshapeCoefficients
            };
        }
    };
    static const _HeadHelper* getAvatarHead(const QUuid& avatarID) {
        auto avatars = DependencyManager::get<AvatarHashMap>();
        auto avatar = avatars ? avatars->getAvatarBySessionID(avatarID) : nullptr;
        auto head = avatar ? avatar->getHeadData() : nullptr;
        return reinterpret_cast<const _HeadHelper*>(head);
    }
};

}

const QLoggingCategory& example::logger{ custom_api_example::logger };

#include "ExampleScriptPlugin.moc"
