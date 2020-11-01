//
//  TivoliWebEngineProfile.cpp
//  interface/src/networking
//
//  Created by Stephen Birarda on 2016-10-17.
//  Copyright 2016 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#include "TivoliWebEngineProfile.h"

#include <set>
#include <mutex>
#include <QtQml/QQmlContext>

#include "RequestFilters.h"

#if !defined(Q_OS_ANDROID)

static const QString QML_WEB_ENGINE_STORAGE_NAME = "qmlWebEngine";

static std::set<TivoliWebEngineProfile*> TivoliWebEngineProfile_instances;
static std::mutex TivoliWebEngineProfile_mutex;

TivoliWebEngineProfile::TivoliWebEngineProfile(QQmlContext* parent) : Parent(parent)
{
    setStorageName(QML_WEB_ENGINE_STORAGE_NAME);
    setOffTheRecord(false);

    // we use the HFWebEngineRequestInterceptor to make sure that web requests are authenticated for the interface user
    setUrlRequestInterceptor(new RequestInterceptor(this));

    std::lock_guard<std::mutex> lock(TivoliWebEngineProfile_mutex);
    TivoliWebEngineProfile_instances.insert(this);
}

TivoliWebEngineProfile::~TivoliWebEngineProfile() {
    std::lock_guard<std::mutex> lock(TivoliWebEngineProfile_mutex);
    TivoliWebEngineProfile_instances.erase(this);
}

void TivoliWebEngineProfile::RequestInterceptor::interceptRequest(QWebEngineUrlRequestInfo& info) {
    RequestFilters::interceptTivoliWebEngineRequest(info, isRestricted());
}

void TivoliWebEngineProfile::registerWithContext(QQmlContext* context) {
    context->setContextProperty(QStringLiteral("TivoliWebEngineProfile"), new TivoliWebEngineProfile(context));
}

void TivoliWebEngineProfile::clearCache() {
    std::lock_guard<std::mutex> lock(TivoliWebEngineProfile_mutex);
    foreach (auto instance, TivoliWebEngineProfile_instances) {
        instance->clearHttpCache();
    }
}

#endif
