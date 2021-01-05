//
//  RequestFilters.cpp
//  interface/src/networking
//
//  Created by Kunal Gosar on 2017-03-10.
//  Copyright 2017 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#include "RequestFilters.h"

#include <QtCore/QDebug>
#include <QtCore/QFileInfo>
// #include <QWebEngineProfile>

#include <SettingHandle.h>
#include <NetworkingConstants.h>
#include <AccountManager.h>

#include "ContextAwareProfile.h"
#include <BuildInfo.h>

#if !defined(Q_OS_ANDROID)

namespace {

    bool isScript(const QString filename) {
        return filename.endsWith(".js", Qt::CaseInsensitive);
    }

    bool isJSON(const QString filename) {
        return filename.endsWith(".json", Qt::CaseInsensitive);
    }

    bool blockLocalFiles(QWebEngineUrlRequestInfo& info) {
        auto requestUrl = info.requestUrl();
        if (!requestUrl.isLocalFile()) {
            // Not a local file, do not block
            return false;
        }

        // We can potentially add whitelisting logic or development environment variables that
        // will allow people to override this setting on a per-client basis here.
        QString targetFilePath = QFileInfo(requestUrl.toLocalFile()).canonicalFilePath();

        // If we get here, we've determined it's a local file and we have no reason not to block it
        qWarning() << "Blocking web access to local file path" << targetFilePath;
        info.block(true);
        return true;
    }
}

void RequestFilters::interceptTivoliWebEngineRequest(QWebEngineUrlRequestInfo& info, bool restricted) {
    if (restricted && blockLocalFiles(info)) {
        return;
    }

    // line below causes crashes so i set it to what windows 10 would use
    // const QString defaultUserAgent = QWebEngineProfile::defaultProfile()->httpUserAgent();
    const QString defaultUserAgent = "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) QtWebEngine/5.15.0 Chrome/80.0.3987.163 Safari/537.36";

    info.setHttpHeader(
        "User-Agent",
        (
            defaultUserAgent + " TivoliCloudVR/" +
            (BuildInfo::BUILD_TYPE == BuildInfo::BuildType::Stable ? BuildInfo::VERSION : "dev")
        ).toLocal8Bit()
    );
}

void RequestFilters::interceptFileType(QWebEngineUrlRequestInfo& info) {
    QString filename = info.requestUrl().fileName();
    if (isScript(filename) || isJSON(filename)) {
        static const QString CONTENT_HEADER = "Accept";
        static const QString TYPE_VALUE = "text/plain,text/html";
        info.setHttpHeader(CONTENT_HEADER.toLocal8Bit(), TYPE_VALUE.toLocal8Bit());
    }
}
#endif
