#include "SessionManager.hpp"

#include <AvatarConstants.h>
#include <gl/Config.h>
using namespace xrs;

void SessionManager::beginFrame() {
    Q_ASSERT(!_frameStarted);
    _session.beginFrame(xr::FrameBeginInfo{});
    _frameStarted = true;
}

void SessionManager::endFrame(bool render) {
    Q_ASSERT(_frameStarted);

    if (!render) {
        _session.endFrame({ _frameState.predictedDisplayTime, xr::EnvironmentBlendMode::Opaque, 0, nullptr });
    } else {
        _session.endFrame(_frameEndInfo);
    }
    _frameStarted = false;
}

SessionManager::SessionManager(const GraphicsBinding& graphicsBinding) {
    const auto& _instance = Manager::get();
    _session = instance.createSession({ {}, systemId, &graphicsBinding });
    // Use local reference space with the avatar height (or avatar center eye position) as the reference pose so that incoming poses
    // come out with a reasonable height.  Do not use an identity pose of the camera will be stuck in the user's feet and do not use
    // the current OpenXR head because if the user is sitting that will be too low for a standing avatar.
    const auto pose = xr::Posef{ xr::Quaternionf{}, xr::Vector3f{ 0, 0.0f - DEFAULT_AVATAR_EYE_HEIGHT, 0 } };
    _space = _session.createReferenceSpace(xr::ReferenceSpaceCreateInfo{ xr::ReferenceSpaceType::Local, pose });
    buildSwapchain();
    FrameData::SESSION = _session;
    FrameData::SPACE = _space;
}

SessionManager::~SessionManager() {
    FrameData::SESSION = xr::Session{};
    FrameData::SPACE = xr::Space{};
    _swapchainImages.clear();
    _swapchain.destroy();
    _session.destroy();
}

void SessionManager::buildSwapchain() {
    auto swapchainFormats = _session.enumerateSwapchainFormatsToVector();
    const auto& renderTargetSize = Manager::get().renderTargetSize;;
    _swapchain = _session.createSwapchain(xr::SwapchainCreateInfo{
        {}, xr::SwapchainUsageFlagBits::TransferDst, GL_SRGB8_ALPHA8, 1, renderTargetSize.x, renderTargetSize.y, 1, 1, 1 });
    _swapchainImages = _swapchain.enumerateSwapchainImagesToVector<xr::SwapchainImageOpenGLKHR>();
    // Finish setting up the layer submission
    xrs::for_each_side_index([&](uint32_t eyeIndex) {
        auto& layerView = _projectionLayerViews[eyeIndex];
        layerView.subImage.swapchain = _swapchain;
        layerView.subImage.imageRect.extent = xr::Extent2Di{ (int32_t)renderTargetSize.x / 2, (int32_t)renderTargetSize.y };
        if (eyeIndex == 1) {
            layerView.subImage.imageRect.offset.x = layerView.subImage.imageRect.extent.width;
        }
    });
}

void SessionManager::emptyFrame() {
    beginFrame();
    endFrame(false);
}


bool SessionManager::shouldRender() {
    if (_sessionEnded) {
        return false;
    }
    manager.pollEvents();
    if (sessionState == xr::SessionState::Ready && !_sessionStarted) {
        _session.beginSession(xr::SessionBeginInfo{ xr::ViewConfigurationType::PrimaryStereo });
        // Force through an initial empty frame, or Oculus won't change to the focused state
        _nextFrame = FrameData::wait();
        emptyFrame();
        _sessionStarted = true;
        return false;
    } else if (sessionState == xr::SessionState::Stopping && !_sessionEnded) {
        _session.endSession();
        _sessionStarted = false;
    }

    if (!_sessionStarted) {
        return false;
    }

    switch (sessionState) {
        case xr::SessionState::Focused:
        case xr::SessionState::Synchronized:
        case xr::SessionState::Visible:
            break;

        default:
            return false;
    }

    _nextFrame = FrameData::wait();
    _frameEndInfo.displayTime = _frameState.predictedDisplayTime;
    for_each_side_index([&](size_t eyeIndex) {
        const auto& frameView = _nextFrame.views[eyeIndex];
        auto& layerView = _projectionLayerViews[eyeIndex];
        layerView.fov = frameView.fov;
        layerView.pose = frameView.pose;
    });
    if (!_frameState.shouldRender) {
        emptyFrame();
        return false;
    }

    return true;
}

uint32_t SessionManager::acquireSwapchainImage() {
    uint32_t swapchainIndex;
    _swapchain.acquireSwapchainImage(xr::SwapchainImageAcquireInfo{}, &swapchainIndex);
    _swapchain.waitSwapchainImage(xr::SwapchainImageWaitInfo{ xr::Duration::infinite() });
    return _swapchainImages[swapchainIndex].image;
}

void SessionManager::releaseSwapchainImage() {
    _swapchain.releaseSwapchainImage(xr::SwapchainImageReleaseInfo{});
}
