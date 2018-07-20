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
#include "NetworkingConstants.h"

#include <QtCore/QDebug>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>

#include <SettingHandle.h>

#include "AccountManager.h"

#if !defined(Q_OS_ANDROID)

namespace {
    bool isAuthableHighFidelityURL(const QUrl& url) {
        auto metaverseServerURL = NetworkingConstants::METAVERSE_SERVER_URL();
        static const QStringList HF_HOSTS = {
            "highfidelity.com", "highfidelity.io",
            metaverseServerURL.toString(), "metaverse.highfidelity.io"
        };
        const auto& scheme = url.scheme();
        const auto& host = url.host();

        return (scheme == "https" && HF_HOSTS.contains(host)) ||
            ((scheme == metaverseServerURL.scheme()) && (host == metaverseServerURL.host()));
    }

     bool isScript(const QString filename) {
         return filename.endsWith(".js", Qt::CaseInsensitive);
     }

     bool isJSON(const QString filename) {
        return filename.endsWith(".json", Qt::CaseInsensitive);
     }


     bool isApplicationFilePath(const QString& path) {
         static const QString applicationRootDir;
         static std::once_flag once;
         std::call_once(once, [&] {
             auto dir = QFileInfo(QCoreApplication::applicationFilePath()).absoluteDir();
#if defined(Q_OS_OSX)
             dir.cdUp();
#endif
             const_cast<QString&>(applicationRootDir) = dir.canonicalPath();
         });
         return path.startsWith(applicationRootDir);
     }

    static bool ALLOW_WEB_LOCAL_FILE_ACCESS() {
        static bool result = false;
        static std::once_flag once;
        std::call_once(once, [&] {
            const QString ALLOW_WEB_LOCAL_FILE_ACCESS_FLAG("HIFI_ALLOW_WEB_LOCAL_FILE_ACCESS");
            result = QProcessEnvironment::systemEnvironment().contains(ALLOW_WEB_LOCAL_FILE_ACCESS_FLAG);
        });
        return result;
    }

     bool blockLocalFiles(QWebEngineUrlRequestInfo& info) {
         if (ALLOW_WEB_LOCAL_FILE_ACCESS()) {
             return false;
         }

         auto requestUrl = info.requestUrl();
         if (requestUrl.isLocalFile()) {
             QString targetFilePath = QFileInfo(requestUrl.toLocalFile()).canonicalFilePath();
             // Files installed by the application are allowed to be rendered in web views
             if (isApplicationFilePath(targetFilePath)) {
                 return false;
             }
             qWarning() << "Blocking web access to local file path" << targetFilePath;
             info.block(true);
             return true;
         }
         return false;
     }
} // anonymous namespace

void RequestFilters::interceptHFWebEngineRequest(QWebEngineUrlRequestInfo& info) {
    if (blockLocalFiles(info)) {
        return;
    }

    // check if this is a request to a highfidelity URL
    bool isAuthable = isAuthableHighFidelityURL(info.requestUrl());
    if (isAuthable) {
        // if we have an access token, add it to the right HTTP header for authorization
        auto accountManager = DependencyManager::get<AccountManager>();

        if (accountManager->hasValidAccessToken()) {
            static const QString OAUTH_AUTHORIZATION_HEADER = "Authorization";

            QString bearerTokenString = "Bearer " + accountManager->getAccountInfo().getAccessToken().token;
            info.setHttpHeader(OAUTH_AUTHORIZATION_HEADER.toLocal8Bit(), bearerTokenString.toLocal8Bit());
        }
    }
    static const QString USER_AGENT = "User-Agent";
    const QString tokenStringMobile{ "Mozilla/5.0 (Linux; Android 6.0; Nexus 5 Build/MRA58N) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/56.0.2924.87 Mobile Safari/537.36" };
    const QString tokenStringMetaverse{ "Chrome/48.0 (HighFidelityInterface)" };

    // During the period in which we have HFC commerce in the system, but not applied everywhere:
    const QString tokenStringCommerce{ "Chrome/48.0 (HighFidelityInterface WithHFC)" };
    Setting::Handle<bool> _settingSwitch{ "commerce", true };
    bool isMoney = _settingSwitch.get();

    const QString tokenString = !isAuthable ? tokenStringMobile : (isMoney ? tokenStringCommerce : tokenStringMetaverse);
    info.setHttpHeader(USER_AGENT.toLocal8Bit(), tokenString.toLocal8Bit());
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