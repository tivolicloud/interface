//
//  Created by Bradley Austin Davis on 2021/09/26.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#include "OpenXrInputPlugin.h"

#include <QtCore/QLoggingCategory>

#include <ui-plugins/PluginContainer.h>
#include <controllers/UserInputMapper.h>
#include <controllers/StandardControls.h>

#include <AvatarConstants.h>
#include <PerfStat.h>
#include <PathUtils.h>
#include <NumericalConstants.h>
#include <StreamUtils.h>

#include "OpenXrHelpers.h"

const char* OpenXrInputPlugin::NAME = "OpenXrInput";

bool OpenXrInputPlugin::isSupported() const {
    return true;
}

bool OpenXrInputPlugin::activate() {
    InputPlugin::activate();
    return true;
}

void OpenXrInputPlugin::deactivate() {
    InputPlugin::deactivate();

    //// unregister with UserInputMapper
    //auto userInputMapper = DependencyManager::get<controller::UserInputMapper>();
    //if (_touch) {
    //    userInputMapper->removeDevice(_touch->getDeviceID());
    //}
    //if (_remote) {
    //    userInputMapper->removeDevice(_remote->getDeviceID());
    //}
}

void OpenXrInputPlugin::pluginUpdate(float deltaTime, const controller::InputCalibrationData& inputCalibrationData) {
    //PerformanceTimer perfTimer("OpenXrInputPlugin::TouchDevice::update");

    //checkForConnectedDevices();

    //bool updateRemote = false, updateTouch = false;

    //ovr::withSession([&](ovrSession session) {
    //    if (_remote) {
    //        updateRemote = OVR_SUCCESS(ovr_GetInputState(session, ovrControllerType_Remote, &_remoteInputState));
    //        if (!updateRemote) {
    //            qCWarning(oculusLog) << "Unable to read Oculus remote input state" << ovr::getError();
    //        }
    //    }
    //});


    //if (_remote && updateRemote) {
    //    _remote->update(deltaTime, inputCalibrationData);
    //}
}

//void OpenXrInputPlugin::pluginFocusOutEvent() {
    //if (_remote) {
    //    _remote->focusOutEvent();
    //}
//}




