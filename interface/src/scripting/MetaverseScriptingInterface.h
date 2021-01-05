#ifndef hifi_MetaverseScriptingInterface_h
#define hifi_MetaverseScriptingInterface_h

#include <QtCore/QObject>

#include <DependencyManager.h>

/**jsdoc
 * The <code>Metaverse</code> API lets you interact with the metaverse.</br>
 * This is a very specific API for things like the explore, avatar or settings app.</br>
 * It's rolling and anything may change in any update.</br>
 * </br>
 * <b>Note: You can't use this API on scripts over http, tea, etc.</b>
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
     * @function Metaverse.getPopularWorlds
     * @param {Object} details
	 * @returns {Object[]}
     */
    QVariant getPopularWorlds(QVariantMap details);

    /**jsdoc
     * @function Metaverse.getLikedWorlds
     * @param {Object} details
	 * @returns {Object[]}
     */
    QVariant getLikedWorlds(QVariantMap details);

    /**jsdoc
     * @function Metaverse.getPrivateWorlds
     * @param {Object} details
	 * @returns {Object[]}
     */
    QVariant getPrivateWorlds(QVariantMap details);

    /**jsdoc
     * @function Metaverse.likeWorld
     * @param {string} id
     * @param {boolean} [like=true]
	 * @returns {Object}
     */
    QVariant likeWorld(const QString& id, const bool like = true);

    /**jsdoc
     * @function Metaverse.getFriends
	 * @returns {Object[]}
     */
    QVariant getFriends();

    /**jsdoc
     * @function Metaverse.getAvatarsFromFiles
	 * @returns {Object[]}
     */
    QVariant getAvatarsFromFiles();

    /**jsdoc
     * @function Metaverse.getNametagDetails
	 * @returns {Object}
     */
    QVariant getNametagDetails();

    /**jsdoc
     * @function Metaverse.setNametagDetails
     * @param {Object} details
	 * @returns {Object}
     */
    QVariant setNametagDetails(QVariantMap details);
};

#endif // hifi_MetaverseScriptingInterface_h
