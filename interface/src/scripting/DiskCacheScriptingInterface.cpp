#include "DiskCacheScriptingInterface.h"

#include <ResourceManager.h>

float oneDecimalPoint(float number) {
    return static_cast<float>(static_cast<int>(number * 10.0f)) / 10.0f;
}

// https://stackoverflow.com/a/47854799
qint64 dirSize(QString dirPath) {
    qint64 size = 0;

    QDir dir(dirPath);
    // calculate total size of current directories' files
    QDir::Filters fileFilters = QDir::Files|QDir::System|QDir::Hidden;
    for (QString filePath : dir.entryList(fileFilters)) {
        QFileInfo fi(dir, filePath);
        size += fi.size();
    }
    // add size of child directories recursively
    QDir::Filters dirFilters = QDir::Dirs|QDir::NoDotAndDotDot|QDir::System|QDir::Hidden;
    for (QString childDirPath : dir.entryList(dirFilters)) {
        size += dirSize(dirPath + QDir::separator() + childDirPath);
    }

    return size;
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
    // return networkDiskCache->cacheSize() / 1000000000.0f;  
    return dirSize(networkDiskCache->cacheDirectory()) / 1000000000.0f;
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

void DiskCacheScriptingInterface::restoreDefaultMaximumCacheSize() {
    setMaximumCacheSize(maximumCacheSizeSetting.getDefault());
}

void DiskCacheScriptingInterface::restoreDefaultCacheDirectory() {
    setCacheDirectory(cacheDirectorySetting.getDefault());
}

void DiskCacheScriptingInterface::clearCache() {
    networkDiskCache->clear();
}