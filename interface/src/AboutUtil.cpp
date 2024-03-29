//
//  AboutUtil.cpp
//  interface/src
//
//  Created by Vlad Stelmahovsky on 15/5/2018.
//  Copyright 2018 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#include "AboutUtil.h"
#include <QDate>
#include <QLocale>

#include <ui/TabletScriptingInterface.h>
#include <OffscreenQmlDialog.h>

#include "BuildInfo.h"
#include "DependencyManager.h"
#include "scripting/HMDScriptingInterface.h"
#include "Application.h"

AboutUtil::AboutUtil(QObject *parent) : QObject(parent) {
    QLocale locale;
    _dateConverted = QDate::fromString(BuildInfo::BUILD_TIME, "dd/MM/yyyy").
            toString(locale.dateFormat(QLocale::ShortFormat));
}

AboutUtil *AboutUtil::getInstance() {
    static AboutUtil instance;
    return &instance;
}

QString AboutUtil::getBuildDate() const {
    return _dateConverted;
}

QString AboutUtil::getBuildVersion() const {
    return BuildInfo::VERSION;
}

QString AboutUtil::getQtVersion() const {
    return qVersion();
}


