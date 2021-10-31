#ifndef hifi_MetaverseScriptingInterface_h
#define hifi_MetaverseScriptingInterface_h

#include <QtCore/QObject>

#include <DependencyManager.h>

/**jsdoc
 * The <code>Metaverse</code> API lets you interact with the metaverse.</br>
 * This is a very specific API for things like the explore, avatar or settings app.</br>
 * It's rolling and anything may change in any update.</br>
 * 
 * @namespace Metaverse
 *
 * @hifi-interface
 * @hifi-client-entity
 * @hifi-avatar
 */
class MetaverseScriptingInterface : public QObject, public Dependency {
    Q_OBJECT

public:
    MetaverseScriptingInterface();

public slots:
    /**jsdoc
     * <p class="important">Only works over file:// or Tivoli things</p>
     * @function Metaverse.readyPlayerMe
     * @param {string} name
     * @param {string} avatarUrl
	 * @returns {Object}
     */
    QVariant readyPlayerMe(const QString& name, const QString& avatarUrl);
};

#endif // hifi_MetaverseScriptingInterface_h
