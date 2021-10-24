//
//  Created by Bradley Austin Davis on 2021/10/23
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//
#pragma once

#include <QtCore/QLoggingCategory>

#ifdef Q_OS_WIN
#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#include <Windows.h>
#include <unknwn.h>
#endif

#include <openxr/openxr_platform.h>
#include <openxr/openxr.hpp>

#include <GLMHelpers.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <functional>

Q_DECLARE_LOGGING_CATEGORY(xr_logging)

class GlmPose {
public:
    GlmPose() = default;
    GlmPose(GlmPose&& o) = default;
    GlmPose(const GlmPose& o) = default;
    GlmPose(const glm::quat orientation_, const glm::vec3& position_) : orientation(orientation_), position(position_) {}

    glm::quat orientation;
    glm::vec3 position;

    glm::mat4 getMatrix() const { return glm::translate(glm::mat4{ 1 }, position) * glm::mat4_cast(orientation); }
};

extern "C" {
XrBool32 __stdcall xrsDebugCallback(XrDebugUtilsMessageSeverityFlagsEXT messageSeverity,
                                    XrDebugUtilsMessageTypeFlagsEXT messageTypes,
                                    const XrDebugUtilsMessengerCallbackDataEXT* callbackData,
                                    void* userData);
}

namespace xrs {

#if defined(Q_OS_WIN)
using GraphicsBinding = xr::GraphicsBindingOpenGLWin32KHR;
#else
#error "Unknown OS for OpenXR"
#endif

constexpr uint32_t SIDE_COUNT = 2;

enum Side
{
    LEFT = 0,
    RIGHT = 1,
    COUNT = 2
};

namespace DebugUtils {

using MessageSeverityFlagBits = xr::DebugUtilsMessageSeverityFlagBitsEXT;
using MessageTypeFlagBits = xr::DebugUtilsMessageTypeFlagBitsEXT;
using MessageSeverityFlags = xr::DebugUtilsMessageSeverityFlagsEXT;
using MessageTypeFlags = xr::DebugUtilsMessageTypeFlagsEXT;
using CallbackData = xr::DebugUtilsMessengerCallbackDataEXT;
using Messenger = xr::DebugUtilsMessengerEXT;

Messenger createMessenger(const MessageSeverityFlags& severityFlags = MessageSeverityFlagBits::AllBits,
                          const MessageTypeFlags& typeFlags = MessageTypeFlagBits::AllBits);
}  // namespace DebugUtils

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

inline GlmPose toGlmPose(const XrPosef& p) {
    return GlmPose{ toGlm(p.orientation), toGlm(p.position) };
}

inline void for_each_side_index(const std::function<void(size_t)>& f) {
    f(0);
    f(1);
}

class Manager;
class FrameData;
class SessionManager;


}  // namespace xrs
