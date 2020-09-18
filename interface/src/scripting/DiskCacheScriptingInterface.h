#ifndef hifi_DiskCacheScriptingInterface_h
#define hifi_DiskCacheScriptingInterface_h

#include <QtCore/QObject>
#include <QNetworkDiskCache>

#include <DependencyManager.h>
#include <PathUtils.h>
#include <SettingHandle.h>

/**jsdoc
 * The <code>DiskCache</code> API lets you manage your cache.
 *
 * @namespace DiskCache
 *
 * @hifi-interface
 * @hifi-client-entity
 * @hifi-avatar
 * 
 * @property {number} maximumCacheSize - The maximum cache size in gigabytes.
 * @property {number} usedCacheSize - The used cache size in gigabytes.
 * @property {string} cacheDirectory - The path to the cache directory.
 */
class DiskCacheScriptingInterface : public QObject, public Dependency {
    Q_OBJECT
    Q_PROPERTY(float maximumCacheSize WRITE setMaximumCacheSize READ getMaximumCacheSize)
    Q_PROPERTY(float usedCacheSize READ getUsedCacheSize)
    Q_PROPERTY(QString cacheDirectory WRITE setCacheDirectory READ getCacheDirectory)

public:
    DiskCacheScriptingInterface();
    ~DiskCacheScriptingInterface() {}

    /**jsdoc
     * Sets the maximum cache size in gigabytes.
     * @function DiskCache.setMaximumCacheSize
     * @param {number} maximumCacheSize - The maximum cache size in gigabytes.
     */
    Q_INVOKABLE void setMaximumCacheSize(float maximumCacheSize);

    /**jsdoc
     * Gets the maximum cache size in gigabytes.
     * @function DiskCache.getMaximumCacheSize
     * @returns {number} maximumCacheSize - The maximum cache size in gigabytes.
     */
    Q_INVOKABLE float getMaximumCacheSize();

    /**jsdoc
     * Gets the used cache size in gigabytes.
     * @function DiskCache.getUsedCacheSize
     * @returns {number} usedCacheSize - The used cache size in gigabytes.
     */
    Q_INVOKABLE float getUsedCacheSize();

    /**jsdoc
     * Sets the path to the cache directory.
     * @function DiskCache.setCacheDirectory
     * @param {string} cacheDirectory - The path to the cache directory.
     */
    Q_INVOKABLE void setCacheDirectory(QString cacheDirectory);

    /**jsdoc
     * Gets the path to the cache directory.
     * @function DiskCache.getCacheDirectory
     * @returns {string} cacheDirectory - The path to the cache directory.
     */
    Q_INVOKABLE QString getCacheDirectory();

    /**jsdoc
     * Sets the path to the cache directory back to the default.
     * @function DiskCache.restoreDefaultCacheDirectory
     */
    Q_INVOKABLE void restoreDefaultCacheDirectory();

private:
    QNetworkDiskCache* networkDiskCache;

    Setting::Handle<float> maximumCacheSizeSetting{ "maximumCacheSize", 2.0f };
    Setting::Handle<QString> cacheDirectorySetting{
        "cacheDirectory", PathUtils::getAppLocalDataFilePath("cache")
    };

};

#endif // hifi_DiskCacheScriptingInterface_h
