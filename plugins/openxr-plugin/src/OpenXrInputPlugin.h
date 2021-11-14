//
//  Created by Bradley Austin Davis on 2021/09/26.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#pragma once

#include <QObject>
#include <unordered_set>
#include <vector>
#include <map>
#include <utility>

#include <GLMHelpers.h>
#include <controllers/InputDevice.h>
#include <plugins/InputPlugin.h>
#include <xr/xr.hpp>

class OpenXrInputPlugin : public InputPlugin {
    using Parent = InputPlugin ;
    Q_OBJECT
public:
    bool isSupported() const override;
    const QString getName() const override { return NAME; }

    bool isHandController() const override { return true; }
    bool configurable() override { return true; }

    bool isHeadController() const override { return true; }

    bool activate() override;
    void deactivate() override;

    void pluginUpdate(float deltaTime, const controller::InputCalibrationData& inputCalibrationData) override;
private:
    static const char* NAME;
};

