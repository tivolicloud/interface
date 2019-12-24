//
//  FileUtils.h
//  interface/src
//
//  Created by Stojce Slavkovski on 12/23/13.
//  Copyright 2013 High Fidelity, Inc.
//
//  Modified by Caitlyn Meeks on 12/24/19
//  Copyright 2019 Tivoli Cloud VR

#ifndef hifi_FileUtils_h
#define hifi_FileUtils_h

#include <QtCore/QString>

class FileUtils {

public:
    static const QStringList& getFileSelectors();
    static QString selectFile(const QString& fileName);
    static void locateFile(const QString& fileName);
    static bool exists(const QString& fileName);
    static bool isRelative(const QString& fileName);
    static QString standardPath(QString subfolder);
    static QString readFile(const QString& filename);
    static QStringList readLines(const QString& filename, QString::SplitBehavior splitBehavior = QString::KeepEmptyParts);
    static QString replaceDateTimeTokens(const QString& path);
    static QString computeDocumentPath(const QString& path);
    static bool canCreateFile(const QString& fullPath);
    static QString getParentPath(const QString& fullPath);
};

#endif // hifi_FileUtils_h
