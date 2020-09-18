#include "DiskCacheScriptingInterface.h"

#include <ResourceManager.h>

float oneDecimalPoint(float number) {
    return static_cast<float>(static_cast<int>(number * 10.0f)) / 10.0f;
}

DiskCacheScriptingInterface::DiskCacheScriptingInterface() {
    networkDiskCache = DependencyManager::get<ResourceManager>()->networkDiskCache;
    setMaximumCacheSize(maximumCacheSizeSetting.get());
    setCacheDirectory(cacheDirectorySetting.get());
}

void DiskCacheScriptingInterface::setMaximumCacheSize(float anyMaximumCacheSize) {
    float maximumCacheSize = oneDecimalPoint(anyMaximumCacheSize);
    networkDiskCache->setMaximumCacheSize(maximumCacheSize * 1000000000.0f);
    
    if (maximumCacheSize == maximumCacheSizeSetting.getDefault()) {
        maximumCacheSizeSetting.remove();
    } else {
        maximumCacheSizeSetting.set(maximumCacheSize);
    }
}

float DiskCacheScriptingInterface::getMaximumCacheSize() {
    return oneDecimalPoint(networkDiskCache->maximumCacheSize() / 1000000000.0f);
}

float DiskCacheScriptingInterface::getUsedCacheSize() {
    return networkDiskCache->cacheSize() / 1000000000.0f;
}

void DiskCacheScriptingInterface::setCacheDirectory(QString cacheDirectory) {
    networkDiskCache->setCacheDirectory(cacheDirectory);

    if (cacheDirectory == cacheDirectorySetting.getDefault()) {
        cacheDirectorySetting.remove();
    } else {
        cacheDirectorySetting.set(cacheDirectory);
    }
}

QString DiskCacheScriptingInterface::getCacheDirectory() {
    return networkDiskCache->cacheDirectory();
}

void DiskCacheScriptingInterface::restoreDefaultCacheDirectory() {
    setCacheDirectory(PathUtils::getAppLocalDataFilePath("cache"));
}