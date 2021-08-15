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

inline glm::mat4 toGlm(const XrFovf& fov, float nearZ = 0.01f, float farZ = 10000.0f) {
    auto tanFov = toTanFovf(fov);
    const auto& tanAngleRight = tanFov.angleRight;
    const auto& tanAngleLeft = tanFov.angleLeft;
    const auto& tanAngleUp = tanFov.angleUp;
    const auto& tanAngleDown = tanFov.angleDown;

    const float tanAngleWidth = tanAngleRight - tanAngleLeft;
    const float tanAngleHeight = (tanAngleDown - tanAngleUp);
    const float offsetZ = 0;

    glm::mat4 resultm{};
    float* result = &resultm[0][0];
    // normal projection
    result[0] = 2 / tanAngleWidth;
    result[4] = 0;
    result[8] = (tanAngleRight + tanAngleLeft) / tanAngleWidth;
    result[12] = 0;

    result[1] = 0;
    result[5] = 2 / tanAngleHeight;
    result[9] = (tanAngleUp + tanAngleDown) / tanAngleHeight;
    result[13] = 0;

    result[2] = 0;
    result[6] = 0;
    result[10] = -(farZ + offsetZ) / (farZ - nearZ);
    result[14] = -(farZ * (nearZ + offsetZ)) / (farZ - nearZ);

    result[3] = 0;
    result[7] = 0;
    result[11] = -1;
    result[15] = 0;

    return resultm;
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

class Manager {
#if defined(Q_OS_WIN)
    using GraphicsBinding = xr::GraphicsBindingOpenGLWin32KHR;
#else
#error "Unknown OS for OpenXR"
#endif

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
    XrBool32 debugCallback(const MessageSeverity& messageSeverity,
                             const MessageType& messageType,
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
    GraphicsBinding graphicsBinding;
    xr::SessionState sessionState{ xr::SessionState::Unknown };
    bool quitRequested{ false };
};


}  // namespace xrs



