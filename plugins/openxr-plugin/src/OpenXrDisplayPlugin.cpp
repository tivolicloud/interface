//
//  Created by Bradley Austin Davis on 2015/05/12
//  Copyright 2015 High Fidelity, Inc.
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

#include "OpenXrHelpers.h"

bool OpenXrDisplayPlugin::isSupported() const {
    return true;
}

glm::mat4 OpenXrDisplayPlugin::getEyeProjection(Eye eye, const glm::mat4& baseProjection) const {
    if (_session) {
        ViewFrustum baseFrustum;
        baseFrustum.setProjection(baseProjection);
        float baseNearClip = baseFrustum.getNearClip();
        float baseFarClip = baseFrustum.getFarClip();
        
        xr::ViewState vs;
        xr::ViewLocateInfo vi{ xr::ViewConfigurationType::PrimaryStereo, xr::Time{}, _space };
        std::vector<xr::View> eyeViewStates = _session.locateViewsToVector(vi, &(vs.operator XrViewState&()));
        return xrs::toGlm(eyeViewStates[eye].fov);
    } else {
        return baseProjection;
    }
}

glm::mat4 OpenXrDisplayPlugin::getCullingProjection(const glm::mat4& baseProjection) const {
    if (_session) {
        // FIXME Calculate the proper combined projection by using GetProjectionRaw values from both eyes
        return getEyeProjection(Eye::Left, baseProjection);
    } else {
        return baseProjection;
    }
}

float OpenXrDisplayPlugin::getTargetFrameRate() const {
    return 120;
}

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
    return Parent::internalActivate();
}

void OpenXrDisplayPlugin::internalDeactivate() {
    Parent::internalDeactivate();
}

void OpenXrDisplayPlugin::customizeContext() {
    // Display plugins in DLLs must initialize GL locally
    gl::initModuleGl();
    Parent::customizeContext();
    _xrManager.graphicsBinding = { wglGetCurrentDC(), wglGetCurrentContext() };
}

void OpenXrDisplayPlugin::beginSession() {
    if (!_session) {
        xr::GraphicsBindingOpenGLWin32KHR graphicsBinding{ wglGetCurrentDC(), wglGetCurrentContext() };
        xr::SessionCreateInfo sci{ {}, _xrManager.systemId };
        sci.next = &graphicsBinding;
        _session = _xrManager.instance.createSession(sci);
        // FIXME which reference space is best for Hifi?  Does it depend on state?
        auto referenceSpaces = _session.enumerateReferenceSpacesToVector();
        _space = _session.createReferenceSpace(xr::ReferenceSpaceCreateInfo{ xr::ReferenceSpaceType::Local, {} });
        auto swapchainFormats = _session.enumerateSwapchainFormatsToVector();
    }
    const auto& renderTargetSize = _xrManager.renderTargetSize;

    _swapchain = _session.createSwapchain(xr::SwapchainCreateInfo{
        {}, xr::SwapchainUsageFlagBits::TransferDst, GL_SRGB8_ALPHA8, 1, renderTargetSize.x, renderTargetSize.y, 1, 1, 1 });

    _swapchainImages = _swapchain.enumerateSwapchainImagesToVector<xr::SwapchainImageOpenGLKHR>();
}
 
void OpenXrDisplayPlugin::resetSensors() {
    //glm::mat4 m;
    //withNonPresentThreadLock([&] { m = toGlm(_nextSimPoseData.vrPoses[0].mDeviceToAbsoluteTracking); });
    //_sensorResetMat = glm::inverse(cancelOutRollAndPitch(m));
}

bool OpenXrDisplayPlugin::beginFrameRender(uint32_t frameIndex) {
    PROFILE_RANGE_EX(render, __FUNCTION__, 0xff7fff00, frameIndex)

    _xrManager.pollEvents();

    if (_xrManager.quitRequested) {
        QMetaObject::invokeMethod(qApp, "quit");
        return false;
    }
    _currentRenderFrameInfo = {};
    switch (_xrManager.sessionState) {
        case xr::SessionState::Ready:
            beginSession();
            // fallthrough
        case xr::SessionState::Focused:
        case xr::SessionState::Synchronized:
        case xr::SessionState::Visible:
            break;
        default:
            return false;
    }

    if (!_session) {
        return false;
    }

    auto frameState = _session.waitFrame({});
    if (_startTime == xr::Time{ 0 }) {
        _startTime = frameState.predictedDisplayTime;
    }
    if (frameState.shouldRender == XR_FALSE) {
        // ???
    }
    

    xr::ViewState vs;
    xr::ViewLocateInfo vi{ xr::ViewConfigurationType::PrimaryStereo, frameState.predictedDisplayTime, _space };
    auto eyeViewStates = _session.locateViewsToVector(vi, &(vs.operator XrViewState&()));

    //PoseData nextSimPoseData;
    //withNonPresentThreadLock([&] { nextSimPoseData = _nextSimPoseData; });

    //// HACK: when interface is launched and steam vr is NOT running, openvr will return bad HMD poses for a few frames
    //// To workaround this, filter out any hmd poses that are obviously bad, i.e. beneath the floor.
    //if (isBadPose(&nextSimPoseData.vrPoses[vr::k_unTrackedDeviceIndex_Hmd].mDeviceToAbsoluteTracking)) {
    //    // qDebug() << "WARNING: ignoring bad hmd pose from openvr";

    //    // use the last known good HMD pose
    //    nextSimPoseData.vrPoses[vr::k_unTrackedDeviceIndex_Hmd].mDeviceToAbsoluteTracking = _lastGoodHMDPose;
    //} else {
    //    _lastGoodHMDPose = nextSimPoseData.vrPoses[vr::k_unTrackedDeviceIndex_Hmd].mDeviceToAbsoluteTracking;
    //}

    //vr::TrackedDeviceIndex_t handIndices[2]{ vr::k_unTrackedDeviceIndexInvalid, vr::k_unTrackedDeviceIndexInvalid };
    //{
    //    vr::TrackedDeviceIndex_t controllerIndices[2];
    //    auto trackedCount =
    //        _system->GetSortedTrackedDeviceIndicesOfClass(vr::TrackedDeviceClass_Controller, controllerIndices, 2);
    //    // Find the left and right hand controllers, if they exist
    //    for (uint32_t i = 0; i < std::min<uint32_t>(trackedCount, 2); ++i) {
    //        if (nextSimPoseData.vrPoses[i].bPoseIsValid) {
    //            auto role = _system->GetControllerRoleForTrackedDeviceIndex(controllerIndices[i]);
    //            if (vr::TrackedControllerRole_LeftHand == role) {
    //                handIndices[0] = controllerIndices[i];
    //            } else if (vr::TrackedControllerRole_RightHand == role) {
    //                handIndices[1] = controllerIndices[i];
    //            }
    //        }
    //    }
    //}

    //_currentRenderFrameInfo.renderPose = nextSimPoseData.poses[vr::k_unTrackedDeviceIndex_Hmd];
    //bool keyboardVisible = isOpenVrKeyboardShown();

    //std::array<mat4, 2> handPoses;
    //if (!keyboardVisible) {
    //    for (int i = 0; i < 2; ++i) {
    //        if (handIndices[i] == vr::k_unTrackedDeviceIndexInvalid) {
    //            continue;
    //        }
    //        auto deviceIndex = handIndices[i];
    //        const mat4& mat = nextSimPoseData.poses[deviceIndex];
    //        const vec3& linearVelocity = nextSimPoseData.linearVelocities[deviceIndex];
    //        const vec3& angularVelocity = nextSimPoseData.angularVelocities[deviceIndex];
    //        auto correctedPose = openVrControllerPoseToHandPose(i == 0, mat, linearVelocity, angularVelocity);
    //        static const glm::quat HAND_TO_LASER_ROTATION = glm::rotation(Vectors::UNIT_Z, Vectors::UNIT_NEG_Y);
    //        handPoses[i] = glm::translate(glm::mat4(), correctedPose.translation) *
    //                       glm::mat4_cast(correctedPose.rotation * HAND_TO_LASER_ROTATION);
    //    }
    //}

    //withNonPresentThreadLock([&] { _frameInfos[frameIndex] = _currentRenderFrameInfo; });
    return Parent::beginFrameRender(frameIndex);
}

void OpenXrDisplayPlugin::hmdPresent() {
    //PROFILE_RANGE_EX(render, __FUNCTION__, 0xff00ff00, (uint64_t)_currentFrame->frameIndex)

    //_visionSqueezeParametersBuffer.edit<VisionSqueezeParameters>()._leftProjection = _eyeProjections[0];
    //_visionSqueezeParametersBuffer.edit<VisionSqueezeParameters>()._rightProjection = _eyeProjections[1];
    //_visionSqueezeParametersBuffer.edit<VisionSqueezeParameters>()._hmdSensorMatrix = _currentPresentFrameInfo.presentPose;
    //GLuint glTexId = getGLBackend()->getTextureID(_compositeFramebuffer->getRenderBuffer(0));
    //vr::Texture_t vrTexture{ (void*)(uintptr_t)glTexId, vr::TextureType_OpenGL, vr::ColorSpace_Auto };
    //vr::VRCompositor()->Submit(vr::Eye_Left, &vrTexture, &OPENVR_TEXTURE_BOUNDS_LEFT);
    //vr::VRCompositor()->Submit(vr::Eye_Right, &vrTexture, &OPENVR_TEXTURE_BOUNDS_RIGHT);
    //vr::VRCompositor()->PostPresentHandoff();
    //_presentRate.increment();

    //vr::Compositor_FrameTiming frameTiming;
    //memset(&frameTiming, 0, sizeof(vr::Compositor_FrameTiming));
    //frameTiming.m_nSize = sizeof(vr::Compositor_FrameTiming);
    //vr::VRCompositor()->GetFrameTiming(&frameTiming);
    //_stutterRate.increment(frameTiming.m_nNumDroppedFrames);
}

void OpenXrDisplayPlugin::postPreview() {
    PROFILE_RANGE_EX(render, __FUNCTION__, 0xff00ff00, (uint64_t)_currentFrame->frameIndex)

    const XrFrameData nextRender = XrFrameData::nextRender(_session, _space, presentCount());

    if (_startTime == xr::Time{ 0 }) {
        _startTime = nextRender.frameState.predictedDisplayTime;
    }
    const XrFrameData nextSim = nextRender.next(_session, _space);

    {
        glm::mat4 resetMat;
        withPresentThreadLock([&] { resetMat = _sensorResetMat; });
        //nextRender.update(resetMat);
        //nextSim.update(resetMat);
        withPresentThreadLock([&] { _nextSimPoseData = nextSim; });
        _nextRenderPoseData = nextRender;
    }
}

bool OpenXrDisplayPlugin::isHmdMounted() const {
    return true;
}

void OpenXrDisplayPlugin::updatePresentPose() {
    //_currentPresentFrameInfo.presentPose = _nextRenderPoseData.poses[vr::k_unTrackedDeviceIndex_Hmd];
}



