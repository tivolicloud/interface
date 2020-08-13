//
//  AboutUtil.h
//  interface/src
//
//  Created by Vlad Stelmahovsky on 15/5/2018.
//  Copyright 2018 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//


#ifndef hifi_AboutUtil_h
#define hifi_AboutUtil_h

#include <QObject>

/**jsdoc
 * The <code>About</code> API provides information about the version of Interface that is currently running. It also
 * has the functionality to open a web page in an Interface browser window.
 *
 * @namespace About
 *
 * @hifi-interface
 * @hifi-client-entity
 * @hifi-avatar
 *
 * @property {string} buildDate - The build date of Tivoli Interface that is currently running. <em>Read-only.</em>
 * @property {string} buildVersion - The build version of Tivoli Interface that is currently running. <em>Read-only.</em>
 * @property {string} qtVersion - The Qt version used in Tivoli Interface that is currently running. <em>Read-only.</em>
 * @property {string} platform - The name of the platform that is currently running, usually "Tivoli Cloud VR". <em>Read-only.</em>
 *
 * @example <caption>Report build information for the version of Interface currently running.</caption>
 * print("Tivoli build date: " + About.buildDate);  // Returns the build date of the version of Interface currently running on your machine.
 * print("Tivoli version: " + About.buildVersion);  // Returns the build version of Interface currently running on your machine.
 * print("Qt version: " + About.qtVersion);  // Returns the Qt version details of the version of Interface currently running on your machine.
 * print("Platform: " + About.platform);  // Returns the platform that is currently running on your machine, usually "Tivoli Cloud VR".
 */

class AboutUtil : public QObject {
    Q_OBJECT

    Q_PROPERTY(QString buildDate READ getBuildDate CONSTANT)
    Q_PROPERTY(QString buildVersion READ getBuildVersion CONSTANT)
    Q_PROPERTY(QString qtVersion READ getQtVersion CONSTANT)
    Q_PROPERTY(QString platform READ getPlatform CONSTANT)
public:
    static AboutUtil* getInstance();
    ~AboutUtil() {}

    QString getBuildDate() const;
    QString getBuildVersion() const;
    QString getQtVersion() const;
    QString getPlatform() { return "Tivoli Cloud VR"; };

private:
    AboutUtil(QObject* parent = nullptr);
    QString _dateConverted;
};

#endif // hifi_AboutUtil_h
