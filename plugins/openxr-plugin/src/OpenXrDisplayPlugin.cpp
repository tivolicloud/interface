//
//  Created by Bradley Austin Davis on 2021/08/14
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//
#include "OpenXrDisplayPlugin.h"

// Odd ordering of header is required to avoid 'macro redinition warnings'
#include <AudioClient.h>

#include <QtCore/QThread>
#include <QtCore/QLoggingCategory>
#include <QtCore/QFileInfo>
#include <QtCore/QDateTime>

#include <GLMHelpers.h>

#include <gl/Context.h>
#include <gl/GLShaders.h>

#include <gpu/Frame.h>
#include <gpu/gl/GLBackend.h>

#include <ViewFrustum.h>
#include <PathUtils.h>
#include <shared/NsightHelpers.h>
#include <controllers/Pose.h>
#include <display-plugins/CompositorHelper.h>
#include <ui-plugins/PluginContainer.h>
#include <gl/OffscreenGLCanvas.h>

bool OpenXrDisplayPlugin::isSupported() const {
    return true;
}

glm::mat4 OpenXrDisplayPlugin::getEyeProjection(Eye eye, const glm::mat4& baseProjection) const {
    xrs::FrameData frameData;
    withNonPresentThreadLock([&] { 
        frameData = _nextFrame;
    });
    if (frameData.viewState.viewStateFlags & xr::ViewStateFlagBits::PositionValid) {
        ViewFrustum baseFrustum;
        baseFrustum.setProjection(baseProjection);
        float baseNearClip = baseFrustum.getNearClip();
        float baseFarClip = baseFrustum.getFarClip();
        return xrs::toGlm(frameData.views[eye].fov, baseNearClip, baseFarClip);
    } 
    return baseProjection;
}

glm::mat4 OpenXrDisplayPlugin::getCullingProjection(const glm::mat4& baseProjection) const {
    xrs::FrameData frameData;
    withNonPresentThreadLock([&] { frameData = _nextFrame; });
    if (frameData.viewState.viewStateFlags & xr::ViewStateFlagBits::PositionValid) {
        auto fov = frameData.views[0].fov;
        fov.angleRight = frameData.views[1].fov.angleRight;
        ViewFrustum baseFrustum;
        baseFrustum.setProjection(baseProjection);
        float baseNearClip = baseFrustum.getNearClip();
        float baseFarClip = baseFrustum.getFarClip();
        return xrs::toGlm(fov, baseNearClip, baseFarClip);
    }
    return baseProjection;
}

// Main Thread

void OpenXrDisplayPlugin::init() {
    Plugin::init();
    _xrManager.init();
    emit deviceConnected(getName());
}

const QString OpenXrDisplayPlugin::getName() const {
    // FIXME fetch the headset name from the OpenXR API somehow
    return QString("OpenXR Headset");
}

bool OpenXrDisplayPlugin::internalActivate() {
    _renderTargetSize = _xrManager.renderTargetSize;
    return Parent::internalActivate();
}

void OpenXrDisplayPlugin::internalDeactivate() {
    Parent::internalDeactivate();
}

bool OpenXrDisplayPlugin::isHmdMounted() const {
    return true;
}

float OpenXrDisplayPlugin::getTargetFrameRate() const {
    return 120;
}

//
// Render thread functions
//
bool OpenXrDisplayPlugin::beginFrameRender(uint32_t frameIndex) {
    PROFILE_RANGE_EX(render, __FUNCTION__, 0xff7fff00, frameIndex)

    static const auto requiredFlags = xr::ViewStateFlagBits::PositionTracked | xr::ViewStateFlagBits::OrientationTracked;

    withNonPresentThreadLock([&] {
        xrs::FrameData frameData = _nextFrame;
        if (frameData.viewState.viewStateFlags & requiredFlags) {
            const auto leftPose = xrs::toGlmPose(frameData.views[0].pose);
            const auto rightPose = xrs::toGlmPose(frameData.views[1].pose);
            const auto center = (leftPose.position + rightPose.position) / 2.0f;
            const auto centerPose = GlmPose{ leftPose.orientation, center };
            _ipd = glm::length(leftPose.position - rightPose.position);
            _currentRenderFrameInfo.renderPose = centerPose.getMatrix(); 
            _currentRenderFrameInfo.presentPose = _currentRenderFrameInfo.renderPose;
            _currentRenderFrameInfo.sensorSampleTime = xrs::FrameData::toSeconds(frameData.frameState.predictedDisplayTime);
            _currentRenderFrameInfo.predictedDisplayTime = xrs::FrameData::toSeconds(frameData.frameState.predictedDisplayTime);
            xrs::for_each_side_index([&](size_t eye) {
                _eyeProjections[eye] = xrs::toGlm(frameData.views[eye].fov);
                _eyeOffsets[eye] = glm::translate(mat4(), vec3{ _ipd * (eye == 0 ? -0.5f : 0.5f), 0.0f, 0.0f });
            });
        }
        _frameInfos[frameIndex] = _currentRenderFrameInfo;
    });

    return Parent::beginFrameRender(frameIndex);
}


//
// Present thread functions
//
void OpenXrDisplayPlugin::customizeContext() {
    // Display plugins in DLLs must initialize GL locally
    gl::initModuleGl();
    Parent::customizeContext();
    _xrSessionManager = std::make_shared<xrs::SessionManager>(xrs::GraphicsBinding{ wglGetCurrentDC(), wglGetCurrentContext() });
    // We should really try to start using the extensions that let us do stereo rendering using multiple layers of a texture array.
    _outputFramebuffer.reset(gpu::Framebuffer::create("OpenXR_Output", gpu::Element::COLOR_SRGBA_32, _renderTargetSize.x, _renderTargetSize.y));
}

void OpenXrDisplayPlugin::uncustomizeContext() {
    _outputFramebuffer.reset();
    _xrSessionManager.reset();
    Parent::uncustomizeContext();
}
 
void OpenXrDisplayPlugin::resetSensors() {
    //glm::mat4 m;
    //withNonPresentThreadLock([&] { m = toGlm(_nextSimPoseData.vrPoses[0].mDeviceToAbsoluteTracking); });
    //_sensorResetMat = glm::inverse(cancelOutRollAndPitch(m));
}

bool OpenXrDisplayPlugin::shouldRender() {
    if (!_xrSessionManager->shouldRender()) {
        return false;
    }

    withPresentThreadLock([&] { 
        _nextFrame = _xrSessionManager->getNextFrame();
    });

    return true;
}

void OpenXrDisplayPlugin::updateFrameData() {
    Parent::updateFrameData();
    _xrSessionManager->beginFrame();
}

void OpenXrDisplayPlugin::hmdPresent() {
    PROFILE_RANGE_EX(render, __FUNCTION__, 0xff00ff00, (uint64_t)_currentFrame->frameIndex)
    {
        PROFILE_RANGE_EX(render, "OpenXR Blit", 0xff00ff00, (uint64_t)_currentFrame->frameIndex);

        _visionSqueezeParametersBuffer.edit<VisionSqueezeParameters>()._leftProjection = _eyeProjections[0];
        _visionSqueezeParametersBuffer.edit<VisionSqueezeParameters>()._rightProjection = _eyeProjections[1];

        // Manually bind the texture to the FBO
        // FIXME we should have a way of wrapping raw GL ids in GPU objects without
        // taking ownership of the object
        // FIXME we actually have that, it's called an ExternalTexture, but we're not using it here.
        auto fbo = getGLBackend()->getFramebufferID(_outputFramebuffer);
        uint32_t curTexId = _xrSessionManager->acquireSwapchainImage();
        glNamedFramebufferTexture(fbo, GL_COLOR_ATTACHMENT0, curTexId, 0);
        render([&](gpu::Batch& batch) {
            batch.enableStereo(false);
            batch.setFramebuffer(_outputFramebuffer);
            batch.setViewportTransform(ivec4(uvec2(), _outputFramebuffer->getSize()));
            batch.setStateScissorRect(ivec4(uvec2(), _outputFramebuffer->getSize()));
            batch.resetViewTransform();
            batch.setProjectionTransform(mat4());
            batch.setPipeline(_drawTexturePipeline);
            batch.setResourceTexture(0, _compositeFramebuffer->getRenderBuffer(0));
            batch.draw(gpu::TRIANGLE_STRIP, 4);
        });
        glNamedFramebufferTexture(fbo, GL_COLOR_ATTACHMENT0, 0, 0);
    }
    _xrSessionManager->releaseSwapchainImage();
    _xrSessionManager->endFrame();
    _presentRate.increment();
}

void OpenXrDisplayPlugin::postPreview() {
    PROFILE_RANGE_EX(render, __FUNCTION__, 0xff00ff00, (uint64_t)_currentFrame->frameIndex)
}

void OpenXrDisplayPlugin::updatePresentPose() {
    if (_nextFrame.viewState.viewStateFlags & xr::ViewStateFlagBits::OrientationValid) {
        _currentPresentFrameInfo.presentPose = xrs::toGlm(_nextFrame.views[0].pose);
    }
}



