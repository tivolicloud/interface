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
        xr::ViewLocateInfo vi{ xr::ViewConfigurationType::PrimaryStereo, 0, _space };
        auto eyeViewStates = _session.locateViews(vi, &(vs.operator XrViewState&()));
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
    if (!_session) {
        xr::GraphicsBindingOpenGLWin32KHR graphicsBinding{ wglGetCurrentDC(), wglGetCurrentContext() };
        xr::SessionCreateInfo sci{ {}, _xrManager.systemId };
        sci.next = &graphicsBinding;
        _session = _xrManager.instance.createSession(sci);
        // FIXME which reference space is best for Hifi?  Does it depend on state?
        auto referenceSpaces = _session.enumerateReferenceSpaces();
        _space = _session.createReferenceSpace(xr::ReferenceSpaceCreateInfo{ xr::ReferenceSpaceType::Local });
        auto swapchainFormats = _session.enumerateSwapchainFormats();
    }
    const auto& renderTargetSize = _xrManager.renderTargetSize;

    _swapchain = _session.createSwapchain(xr::SwapchainCreateInfo{
        {}, xr::SwapchainUsageFlagBits::TransferDst, GL_SRGB8_ALPHA8, 1, renderTargetSize.x, renderTargetSize.y, 1, 1, 1 });

    _swapchainImages = _swapchain.enumerateSwapchainImages<xr::SwapchainImageOpenGLKHR>();
}

void OpenXrDisplayPlugin::uncustomizeContext() {
    Parent::uncustomizeContext();
}

void OpenXrDisplayPlugin::resetSensors() {
    //glm::mat4 m;
    //withNonPresentThreadLock([&] { m = toGlm(_nextSimPoseData.vrPoses[0].mDeviceToAbsoluteTracking); });
    //_sensorResetMat = glm::inverse(cancelOutRollAndPitch(m));
}

//static bool isBadPose(vr::HmdMatrix34_t* mat) {
//    if (mat->m[1][3] < -0.2f) {
//        return true;
//    }
//    return false;
//}

bool OpenXrDisplayPlugin::beginFrameRender(uint32_t frameIndex) {
    PROFILE_RANGE_EX(render, __FUNCTION__, 0xff7fff00, frameIndex)
    handleOpenVrEvents();
    if (openVrQuitRequested()) {
        QMetaObject::invokeMethod(qApp, "quit");
        return false;
    }
    _currentRenderFrameInfo = FrameInfo();

    PoseData nextSimPoseData;
    withNonPresentThreadLock([&] { nextSimPoseData = _nextSimPoseData; });

    // HACK: when interface is launched and steam vr is NOT running, openvr will return bad HMD poses for a few frames
    // To workaround this, filter out any hmd poses that are obviously bad, i.e. beneath the floor.
    if (isBadPose(&nextSimPoseData.vrPoses[vr::k_unTrackedDeviceIndex_Hmd].mDeviceToAbsoluteTracking)) {
        // qDebug() << "WARNING: ignoring bad hmd pose from openvr";

        // use the last known good HMD pose
        nextSimPoseData.vrPoses[vr::k_unTrackedDeviceIndex_Hmd].mDeviceToAbsoluteTracking = _lastGoodHMDPose;
    } else {
        _lastGoodHMDPose = nextSimPoseData.vrPoses[vr::k_unTrackedDeviceIndex_Hmd].mDeviceToAbsoluteTracking;
    }

    vr::TrackedDeviceIndex_t handIndices[2]{ vr::k_unTrackedDeviceIndexInvalid, vr::k_unTrackedDeviceIndexInvalid };
    {
        vr::TrackedDeviceIndex_t controllerIndices[2];
        auto trackedCount =
            _system->GetSortedTrackedDeviceIndicesOfClass(vr::TrackedDeviceClass_Controller, controllerIndices, 2);
        // Find the left and right hand controllers, if they exist
        for (uint32_t i = 0; i < std::min<uint32_t>(trackedCount, 2); ++i) {
            if (nextSimPoseData.vrPoses[i].bPoseIsValid) {
                auto role = _system->GetControllerRoleForTrackedDeviceIndex(controllerIndices[i]);
                if (vr::TrackedControllerRole_LeftHand == role) {
                    handIndices[0] = controllerIndices[i];
                } else if (vr::TrackedControllerRole_RightHand == role) {
                    handIndices[1] = controllerIndices[i];
                }
            }
        }
    }

    _currentRenderFrameInfo.renderPose = nextSimPoseData.poses[vr::k_unTrackedDeviceIndex_Hmd];
    bool keyboardVisible = isOpenVrKeyboardShown();

    std::array<mat4, 2> handPoses;
    if (!keyboardVisible) {
        for (int i = 0; i < 2; ++i) {
            if (handIndices[i] == vr::k_unTrackedDeviceIndexInvalid) {
                continue;
            }
            auto deviceIndex = handIndices[i];
            const mat4& mat = nextSimPoseData.poses[deviceIndex];
            const vec3& linearVelocity = nextSimPoseData.linearVelocities[deviceIndex];
            const vec3& angularVelocity = nextSimPoseData.angularVelocities[deviceIndex];
            auto correctedPose = openVrControllerPoseToHandPose(i == 0, mat, linearVelocity, angularVelocity);
            static const glm::quat HAND_TO_LASER_ROTATION = glm::rotation(Vectors::UNIT_Z, Vectors::UNIT_NEG_Y);
            handPoses[i] = glm::translate(glm::mat4(), correctedPose.translation) *
                           glm::mat4_cast(correctedPose.rotation * HAND_TO_LASER_ROTATION);
        }
    }

    withNonPresentThreadLock([&] { _frameInfos[frameIndex] = _currentRenderFrameInfo; });
    return Parent::beginFrameRender(frameIndex);
}

void OpenXrDisplayPlugin::compositeLayers() {
    Parent::compositeLayers();
}

void OpenXrDisplayPlugin::hmdPresent() {
    PROFILE_RANGE_EX(render, __FUNCTION__, 0xff00ff00, (uint64_t)_currentFrame->frameIndex)


    _visionSqueezeParametersBuffer.edit<VisionSqueezeParameters>()._leftProjection = _eyeProjections[0];
    _visionSqueezeParametersBuffer.edit<VisionSqueezeParameters>()._rightProjection = _eyeProjections[1];
    _visionSqueezeParametersBuffer.edit<VisionSqueezeParameters>()._hmdSensorMatrix = _currentPresentFrameInfo.presentPose;

    GLuint glTexId = getGLBackend()->getTextureID(_compositeFramebuffer->getRenderBuffer(0));
    vr::Texture_t vrTexture{ (void*)(uintptr_t)glTexId, vr::TextureType_OpenGL, vr::ColorSpace_Auto };
    vr::VRCompositor()->Submit(vr::Eye_Left, &vrTexture, &OPENVR_TEXTURE_BOUNDS_LEFT);
    vr::VRCompositor()->Submit(vr::Eye_Right, &vrTexture, &OPENVR_TEXTURE_BOUNDS_RIGHT);
    vr::VRCompositor()->PostPresentHandoff();
    _presentRate.increment();

    vr::Compositor_FrameTiming frameTiming;
    memset(&frameTiming, 0, sizeof(vr::Compositor_FrameTiming));
    frameTiming.m_nSize = sizeof(vr::Compositor_FrameTiming);
    vr::VRCompositor()->GetFrameTiming(&frameTiming);
    _stutterRate.increment(frameTiming.m_nNumDroppedFrames);
}

void OpenXrDisplayPlugin::postPreview() {
    PROFILE_RANGE_EX(render, __FUNCTION__, 0xff00ff00, (uint64_t)_currentFrame->frameIndex)
    PoseData nextRender, nextSim;
    nextRender.frameIndex = presentCount();

    if (!_threadedSubmit) {
        vr::VRCompositor()->WaitGetPoses(nextRender.vrPoses, vr::k_unMaxTrackedDeviceCount, nextSim.vrPoses,
                                         vr::k_unMaxTrackedDeviceCount);

        glm::mat4 resetMat;
        withPresentThreadLock([&] { resetMat = _sensorResetMat; });
        nextRender.update(resetMat);
        nextSim.update(resetMat);
        withPresentThreadLock([&] { _nextSimPoseData = nextSim; });
        _nextRenderPoseData = nextRender;
    }

    if (isHmdMounted() != _hmdMounted) {
        _hmdMounted = !_hmdMounted;
        emit hmdMountedChanged();
    }
}

bool OpenXrDisplayPlugin::isHmdMounted() const {
    return isHeadInHeadset();
}

void OpenXrDisplayPlugin::updatePresentPose() {
    _currentPresentFrameInfo.presentPose = _nextRenderPoseData.poses[vr::k_unTrackedDeviceIndex_Hmd];
}

bool OpenXrDisplayPlugin::suppressKeyboard() {
    if (isOpenVrKeyboardShown()) {
        return false;
    }
    if (!_keyboardSupressionCount.fetch_add(1)) {
        disableOpenVrKeyboard();
    }
    return true;
}

void OpenXrDisplayPlugin::unsuppressKeyboard() {
    if (_keyboardSupressionCount == 0) {
        qWarning() << "Attempted to unsuppress a keyboard that was not suppressed";
        return;
    }
    if (1 == _keyboardSupressionCount.fetch_sub(1)) {
        enableOpenVrKeyboard(_container);
    }
}

bool OpenXrDisplayPlugin::isKeyboardVisible() {
    return isOpenVrKeyboardShown();
}

int OpenXrDisplayPlugin::getRequiredThreadCount() const {
    return Parent::getRequiredThreadCount() + (_threadedSubmit ? 1 : 0);
}

QString OpenXrDisplayPlugin::getPreferredAudioInDevice() const {
    QString device = getVrSettingString(vr::k_pch_audio_Section, vr::k_pch_audio_RecordingDeviceOverride_String);
    // FIXME: Address Linux.
#ifdef Q_OS_WIN
    if (!device.isEmpty()) {
        static const WCHAR INIT = 0;
        size_t size = device.size() + 1;
        std::vector<WCHAR> deviceW;
        deviceW.assign(size, INIT);
        device.toWCharArray(deviceW.data());
        // FIXME: This may not be necessary if vr::k_pch_audio_RecordingDeviceOverride_StringName is used above.
        device = AudioClient::getWinDeviceName(deviceW.data());
    }
#endif
    return device;
}

QString OpenXrDisplayPlugin::getPreferredAudioOutDevice() const {
    QString device = getVrSettingString(vr::k_pch_audio_Section, vr::k_pch_audio_PlaybackDeviceOverride_String);
    // FIXME: Address Linux.
#ifdef Q_OS_WIN
    if (!device.isEmpty()) {
        static const WCHAR INIT = 0;
        size_t size = device.size() + 1;
        std::vector<WCHAR> deviceW;
        deviceW.assign(size, INIT);
        device.toWCharArray(deviceW.data());
        // FIXME: This may not be necessary if vr::k_pch_audio_PlaybackDeviceOverride_StringName is used above.
        device = AudioClient::getWinDeviceName(deviceW.data());
    }
#endif
    return device;
}

QRectF OpenXrDisplayPlugin::getPlayAreaRect() {
    auto chaperone = vr::VRChaperone();
    if (!chaperone) {
        qWarning() << "No chaperone";
        return QRectF();
    }

    if (chaperone->GetCalibrationState() >= vr::ChaperoneCalibrationState_Error) {
        qWarning() << "Chaperone status =" << chaperone->GetCalibrationState();
        return QRectF();
    }

    vr::HmdQuad_t rect;
    if (!chaperone->GetPlayAreaRect(&rect)) {
        qWarning() << "Chaperone rect not obtained";
        return QRectF();
    }

    auto minXZ = transformPoint(_sensorResetMat, toGlm(rect.vCorners[0]));
    auto maxXZ = minXZ;
    for (int i = 1; i < 4; i++) {
        auto point = transformPoint(_sensorResetMat, toGlm(rect.vCorners[i]));
        minXZ.x = std::min(minXZ.x, point.x);
        minXZ.z = std::min(minXZ.z, point.z);
        maxXZ.x = std::max(maxXZ.x, point.x);
        maxXZ.z = std::max(maxXZ.z, point.z);
    }

    glm::vec2 center = glm::vec2((minXZ.x + maxXZ.x) / 2, (minXZ.z + maxXZ.z) / 2);
    glm::vec2 dimensions = glm::vec2(maxXZ.x - minXZ.x, maxXZ.z - minXZ.z);

    return QRectF(center.x, center.y, dimensions.x, dimensions.y);
}

DisplayPlugin::StencilMaskMeshOperator OpenXrDisplayPlugin::getStencilMaskMeshOperator() {
    if (_system) {
        if (!_stencilMeshesInitialized) {
            _stencilMeshesInitialized = true;
            for (auto eye : VR_EYES) {
                vr::HiddenAreaMesh_t stencilMesh = _system->GetHiddenAreaMesh(eye);
                if (stencilMesh.pVertexData && stencilMesh.unTriangleCount > 0) {
                    std::vector<glm::vec3> vertices;
                    std::vector<uint32_t> indices;

                    const int NUM_INDICES_PER_TRIANGLE = 3;
                    int numIndices = stencilMesh.unTriangleCount * NUM_INDICES_PER_TRIANGLE;
                    vertices.reserve(numIndices);
                    indices.reserve(numIndices);
                    for (int i = 0; i < numIndices; i++) {
                        vr::HmdVector2_t vertex2D = stencilMesh.pVertexData[i];
                        // We need the vertices in clip space
                        vertices.emplace_back(vertex2D.v[0] - (1.0f - (float)eye), 2.0f * vertex2D.v[1] - 1.0f, 0.0f);
                        indices.push_back(i);
                    }

                    _stencilMeshes[eye] = graphics::Mesh::createIndexedTriangles_P3F((uint32_t)vertices.size(), (uint32_t)indices.size(), vertices.data(), indices.data());
                } else {
                    _stencilMeshesInitialized = false;
                }
            }
        }

        if (_stencilMeshesInitialized) {
            return [&](gpu::Batch& batch) {
                for (auto& mesh : _stencilMeshes) {
                    batch.setIndexBuffer(mesh->getIndexBuffer());
                    batch.setInputFormat((mesh->getVertexFormat()));
                    batch.setInputStream(0, mesh->getVertexStream());

                    // Draw
                    auto part = mesh->getPartBuffer().get<graphics::Mesh::Part>(0);
                    batch.drawIndexed(gpu::TRIANGLES, part._numIndices, part._startIndex);
                }
            };
        }
    }
    return nullptr;
}

void OpenXrDisplayPlugin::updateParameters(float visionSqueezeX, float visionSqueezeY, float visionSqueezeTransition,
                                           int visionSqueezePerEye, float visionSqueezeGroundPlaneY,
                                           float visionSqueezeSpotlightSize) {
    _visionSqueezeX = visionSqueezeX;
    _visionSqueezeY = visionSqueezeY;
    _visionSqueezeTransition = visionSqueezeTransition;
    _visionSqueezePerEye = visionSqueezePerEye;
    _visionSqueezeGroundPlaneY = visionSqueezeGroundPlaneY;
    _visionSqueezeSpotlightSize = visionSqueezeSpotlightSize;
}
