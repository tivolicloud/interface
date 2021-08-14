#pragma once

#include <GLMHelpers.h>
#include <glm/gtc/type_ptr.hpp>

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
using IUnknown = void;
#include <Windows.h>
#endif

#include <openxr/openxr.hpp>

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

class InstanceManager {
public:
    DebugUtilsEXT::Messenger createMessenger(
        const DebugUtilsEXT::MessageSeverityFlags& severityFlags = DebugUtilsEXT::MessageSeverityFlagBits::AllBits,
        const DebugUtilsEXT::MessageTypeFlags& typeFlags = DebugUtilsEXT::MessageTypeFlagBits::AllBits);

    void prepareInstance();
    void prepareSystem();
    void prepareSession(xr::GraphicsBindingOpenGLWin32KHR graphicsBinding);
    void prepareSwapchain();
    void prepareActions();
    void pollEvents();
    void pollActions();
    void beginSession();
    void endSession();
    bool beginFrame();
    bool isSessionActive() const { return sessionActive; }
    void endFrame(const xr::FrameEndInfo& end);
    void endFrame(xr::Time predictedDisplayTime,
                  const std::vector<xr::CompositionLayerBaseHeader*>& layers,
                  xr::EnvironmentBlendMode blendMode = xr::EnvironmentBlendMode::Opaque);
    xr::SessionState getSessionState() const { return sessionState; }
    const std::vector<xr::View>& getUpdatedViewStates(xr::Time displayTime);
    uint32_t getNextSwapchainImage();
    void commitSwapchainImage();

    const xr::Space& getSpace() const { return space; }
    const xr::Swapchain& getSwapchain() const { return swapchain; };


    xr::FrameState waitFrame() const {
        xr::FrameState frameState;
        auto result = session.waitFrame(xr::FrameWaitInfo{}, frameState);
        return frameState;
    }

    const glm::uvec2& getRenderTargetSize() const { return renderTargetSize; }

private:
    xr::ActionSet actionSet;
    struct ActionState {
        xr::Action grabAction{ XR_NULL_HANDLE };
        xr::Action poseAction{ XR_NULL_HANDLE };
        xr::Action vibrateAction{ XR_NULL_HANDLE };
        xr::Action quitAction{ XR_NULL_HANDLE };
        std::array<xr::Path, xr::SIDE_COUNT> handSubactionPath;
        std::array<xr::Space, xr::SIDE_COUNT> handSpace;
        std::array<float, xr::SIDE_COUNT> handScale = { { 1.0f, 1.0f } };
        std::array<xr::Bool32, xr::SIDE_COUNT> handActive;
    };
    ActionState actionState;

    std::vector<xr::View> eyeViewStates;
    bool shutdown{ false };
    xr::Instance instance;
    xr::SystemId systemId;
    xr::SessionState sessionState{ xr::SessionState::Unknown };
    bool enableDebug{ true };
    bool sessionActive{ false };
    xrs::DebugUtilsEXT::Messenger messenger;
    xr::DispatchLoaderDynamic dispatch;
    glm::uvec2 renderTargetSize;
    xr::GraphicsRequirementsOpenGLKHR graphicsRequirements;

    // Session
    xr::Session session;

    // Swapchain
    xr::SwapchainCreateInfo swapchainCreateInfo;
    xr::Swapchain swapchain;
    std::vector<xr::SwapchainImageOpenGLKHR> swapchainImages;
    xr::Space space;
};

}  // namespace xrs
