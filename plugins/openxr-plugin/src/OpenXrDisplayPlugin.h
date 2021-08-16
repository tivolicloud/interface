//
//  Created by Bradley Austin Davis on 2015/06/12
//  Copyright 2015 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//
#pragma once

#include <QtGlobal>

#include <display-plugins/hmd/HmdDisplayPlugin.h>
#include "OpenXrHelpers.h"

#include <graphics/Geometry.h>

class OpenXrDisplayPlugin : public HmdDisplayPlugin {
    using Parent = HmdDisplayPlugin;
public:
    bool isSupported() const override;
    const QString getName() const override;
    bool getSupportsAutoSwitch() override final { return true; }

    glm::mat4 getEyeProjection(Eye eye, const glm::mat4& baseProjection) const override;
    glm::mat4 getCullingProjection(const glm::mat4& baseProjection) const override;

    void init() override;

    float getTargetFrameRate() const override;
    bool hasAsyncReprojection() const override { return true; }

    void customizeContext() override;
    void uncustomizeContext() override;

    // Stereo specific methods
    void resetSensors() override;
    bool beginFrameRender(uint32_t frameIndex) override;
    void cycleDebugOutput() override { _lockCurrentTexture = !_lockCurrentTexture; }
    glm::mat4 getSensorResetMatrix() const { return _sensorResetMat; }

protected:
    bool internalActivate() override;
    void internalDeactivate() override;
    void updatePresentPose() override;

    void hmdPresent() override;
    bool isHmdMounted() const override;
    void postPreview() override;
    void updateFrameData() override;
    bool shouldRender() override;


private:
    xrs::Manager& _xrManager{ xrs::Manager::get() };
    std::shared_ptr<xrs::SessionManager> _xrSessionManager;
    gpu::FramebufferPointer _outputFramebuffer;
    mat4 _sensorResetMat;
    xrs::FrameData _nextFrame;
};
