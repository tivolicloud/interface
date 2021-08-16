//
//  Created by Bradley Austin Davis on 2015/06/12
//  Copyright 2015 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//
#pragma once

#include <WinSock2.h>
#include <QtCore/QLoggingCategory>

#include <GLMHelpers.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <plugins/Forward.h>
#include <Windows.h>
#include <combaseapi.h>
#include <openxr/openxr_platform.h>
#include <openxr/openxr.hpp>

Q_DECLARE_LOGGING_CATEGORY(xr_logging)


namespace xrs {

namespace DebugUtilsEXT {

using MessageSeverityFlagBits = xr::DebugUtilsMessageSeverityFlagBitsEXT;
using MessageTypeFlagBits = xr::DebugUtilsMessageTypeFlagBitsEXT;
using MessageSeverityFlags = xr::DebugUtilsMessageSeverityFlagsEXT;
using MessageTypeFlags = xr::DebugUtilsMessageTypeFlagsEXT;
using CallbackData = xr::DebugUtilsMessengerCallbackDataEXT;
using Messenger = xr::DebugUtilsMessengerEXT;

}  // namespace DebugUtilsEXT

inline XrFovf toTanFovf(const XrFovf& fov) {
    return { tanf(fov.angleLeft), tanf(fov.angleRight), tanf(fov.angleUp), tanf(fov.angleDown) };
}

inline glm::mat4 toGlm(const XrFovf fov, const float nearZ = 0.01f, const float farZ = 10000.0f) {
    auto tanFov = toTanFovf(fov);
    const auto& tanAngleUp = tanFov.angleUp;
    const auto& tanAngleDown = tanFov.angleDown;
    const auto& tanAngleLeft = tanFov.angleLeft;
    const auto& tanAngleRight = tanFov.angleRight;

    const float tanAngleWidth = tanAngleRight - tanAngleLeft;
    // Set to tanAngleUp - tanAngleDown for a clip space with positive Y up (OpenGL / D3D / Metal).
    const float tanAngleHeight = (tanAngleUp - tanAngleDown);
    // Set to nearZ for a [-1,1] Z clip space (OpenGL / OpenGL ES).
    const float offsetZ = nearZ;

    glm::mat4 result;
    float* m = &(result[0][0]);

    // normal projection
    m[0] = 2.0f / tanAngleWidth;
    m[4] = 0.0f;
    m[8] = (tanAngleRight + tanAngleLeft) / tanAngleWidth;
    m[12] = 0.0f;
    m[1] = 0.0f;
    m[5] = 2.0f / tanAngleHeight;
    m[9] = (tanAngleUp + tanAngleDown) / tanAngleHeight;
    m[13] = 0.0f;
    m[2] = 0.0f;
    m[6] = 0.0f;
    m[10] = -(farZ + offsetZ) / (farZ - nearZ);
    m[14] = -(farZ * (nearZ + offsetZ)) / (farZ - nearZ);
    m[3] = 0.0f;
    m[7] = 0.0f;
    m[11] = -1.0f;
    m[15] = 0.0f;
    return result;
}


inline glm::quat toGlm(const XrQuaternionf& q) {
    return glm::make_quat(&q.x);
}

inline glm::vec3 toGlm(const XrVector3f& v) {
    return glm::make_vec3(&v.x);
}

inline glm::mat4 toGlm(const XrPosef& p) {
    glm::mat4 orientation = glm::mat4_cast(toGlm(p.orientation));
    glm::mat4 translation = glm::translate(glm::mat4{ 1 }, toGlm(p.position));
    return translation * orientation;
}

inline void for_each_side_index(const std::function < void(size_t)>& f) {
    f(0);
    f(1);
}


class Manager {

    using GraphicsRequirements = xr::GraphicsRequirementsOpenGLKHR;
    using DispatchLoaderDynamic = xr::DispatchLoaderDynamic;
    using Messenger = xrs::DebugUtilsEXT::Messenger;
    using MessageSeverity= xrs::DebugUtilsEXT::MessageSeverityFlags;
    using MessageType = xrs::DebugUtilsEXT::MessageTypeFlags;
    using CallbackData = xrs::DebugUtilsEXT::CallbackData;
    using Instance = xr::Instance;
    using SystemId = xr::SystemId;
public:
    DebugUtilsEXT::Messenger createMessenger(
        const DebugUtilsEXT::MessageSeverityFlags& severityFlags = DebugUtilsEXT::MessageSeverityFlagBits::AllBits,
        const DebugUtilsEXT::MessageTypeFlags& typeFlags = DebugUtilsEXT::MessageTypeFlagBits::AllBits);

    static Manager& get();
    void init();

public:
    XrBool32 debugCallback(const XrDebugUtilsMessageSeverityFlagsEXT& messageSeverity,
                           const XrDebugUtilsMessageTypeFlagsEXT& messageType,
                             const CallbackData& callbackData);

    void pollEvents();

public:
    bool enableDebug{ true };
    Instance instance;
    SystemId systemId;
    Messenger messenger;
    DispatchLoaderDynamic dispatch;
    glm::uvec2 renderTargetSize;
    GraphicsRequirements graphicsRequirements;
    bool quitRequested{ false };
};


struct FrameData {
    static xr::Session SESSION;
    static xr::Space SPACE;
    static XrTime START_TIME;

    xr::FrameState frameState;
    xr::ViewState viewState;
    std::vector<xr::View> views;

    void updateViews() {
        XrViewState* viewStatePtr = &(viewState.operator XrViewState&());
        views =
            SESSION.locateViewsToVector({ xr::ViewConfigurationType::PrimaryStereo, frameState.predictedDisplayTime, SPACE },
                                        viewStatePtr);
    }

    static FrameData wait() {
        FrameData result;
        result.frameState = SESSION.waitFrame({});
        if (0 == START_TIME) {
            START_TIME = result.frameState.predictedDisplayTime.get();
        }
        result.updateViews();
        return result;
    }

    static double toSeconds(xr::Time xrtime) { 
        auto time = xrtime.get() - START_TIME;
        time /= 1000;
        double result = time;
        result /= 1000.0;
        return result;
    }

    FrameData next() const {
        FrameData result;
        result.frameState = frameState;
        result.frameState.predictedDisplayTime += frameState.predictedDisplayPeriod;
        result.updateViews();
        return result;
    }
};

class SessionManager {
public:
    static xr::SessionState STATE;
    using Ptr = std::shared_ptr<SessionManager>;
#if defined(Q_OS_WIN)
    using GraphicsBinding = xr::GraphicsBindingOpenGLWin32KHR;
#else
#error "Unknown OS for OpenXR"
#endif

    SessionManager(const GraphicsBinding& graphicsBinding);
    virtual ~SessionManager();
    bool shouldRender();
    void buildSwapchain();
    void emptyFrame();

    void endFrame(bool render = true) {
        Q_ASSERT(_frameStarted);

        if (!render) {
            _session.endFrame({ _frameState.predictedDisplayTime, xr::EnvironmentBlendMode::Opaque, 0, nullptr });
        } else {
            _session.endFrame(_frameEndInfo);
        }
        _frameStarted = false;
    }

    void beginFrame() {
        Q_ASSERT(!_frameStarted);
        _session.beginFrame(xr::FrameBeginInfo{});
        _frameStarted = true;
    }

    uint32_t acquireSwapchainImage();
    void releaseSwapchainImage();

    const FrameData& getNextFrame() const { return _nextFrame;}

private:
    bool _sessionStarted{ false };
    bool _sessionEnded{ false };
    bool _frameStarted{ false };
    xrs::Manager& _instance{ xrs::Manager::get() };
    xr::Session _session;
    xr::Swapchain _swapchain;
    std::vector<xr::SwapchainImageOpenGLKHR> _swapchainImages;

    std::array<xr::CompositionLayerProjectionView, 2> _projectionLayerViews;
    xr::CompositionLayerProjection _projectionLayer{ {}, {}, 2, _projectionLayerViews.data() };
    std::vector<xr::CompositionLayerBaseHeader*> _layersPointers{ &_projectionLayer };
    xr::Space& _space{ _projectionLayer.space };

    FrameData _nextFrame;
    const xr::FrameState& _frameState{ _nextFrame.frameState };
    const xr::ViewState& _viewState{ _nextFrame.viewState };

    xr::FrameEndInfo _frameEndInfo{ xr::Time{}, xr::EnvironmentBlendMode::Opaque, (uint32_t)_layersPointers.size(),
                                    _layersPointers.data() };

    QThread* _sessionThread{ nullptr };
};

}  // namespace xrs



