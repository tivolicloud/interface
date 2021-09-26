//
//  Created by Bradley Austin Davis on 2021/08/14
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//
#include "OpenXrHelpers.h"

#include <atomic>
#include <mutex>

#include <QtCore/QDebug>
#include <QtCore/QTimer>
#include <QtCore/QThread>
#include <QtCore/QLoggingCategory>
#include <QtCore/QProcessEnvironment>
#include <QtGui/QInputMethodEvent>
#include <QtQuick/QQuickWindow>
#include <AvatarConstants.h>

#include <unordered_map>


Q_LOGGING_CATEGORY(xr_logging, "hifi.xr")

using namespace xrs;
using namespace xrs::DebugUtilsEXT;

XrTime FrameData::START_TIME{ 0 };
xr::Session FrameData::SESSION{};
xr::Space FrameData::SPACE{};
xr::SessionState SessionManager::STATE{ xr::SessionState::Unknown };

Manager& Manager::get() {
    static Manager instance;
    return instance;
}

XrBool32 __stdcall xrDebugCallback(XrDebugUtilsMessageSeverityFlagsEXT messageSeverity,
                                   XrDebugUtilsMessageTypeFlagsEXT messageTypes,
                                   const XrDebugUtilsMessengerCallbackDataEXT* callbackData,
                                   void* userData) {
    auto manager = reinterpret_cast<Manager*>(userData);
    return manager->debugCallback(messageSeverity, messageTypes, *callbackData);
}

XrBool32 Manager::debugCallback(const XrDebugUtilsMessageSeverityFlagsEXT& messageSeverity,
                                const XrDebugUtilsMessageTypeFlagsEXT& messageType,
                              const CallbackData& callbackData) {
    constexpr auto WARN_BITS = XR_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | XR_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

    if (0 != (WARN_BITS & messageSeverity)) {
        qWarning() << "QQQ " << callbackData.message;
    } else {
        qDebug() << "ZZZ " << callbackData.message;
    }
    return XR_TRUE;
}

Messenger Manager::createMessenger(const MessageSeverityFlags& severityFlags, const MessageTypeFlags& typeFlags) {
    return instance.createDebugUtilsMessengerEXT({ severityFlags, typeFlags, xrDebugCallback, this },
                                                 xr::DispatchLoaderDynamic{ instance });
}

void Manager::init() {
    std::unordered_map<std::string, xr::ApiLayerProperties> discoveredLayers;
    for (const auto& layerProperties : xr::enumerateApiLayerPropertiesToVector()) {
        qDebug(xr_logging, "Layer found: %s", layerProperties.layerName);
        discoveredLayers.insert({ layerProperties.layerName, layerProperties });
    }

    std::unordered_map<std::string, xr::ExtensionProperties> discoveredExtensions;
    for (const auto& extensionProperties : xr::enumerateInstanceExtensionPropertiesToVector(nullptr)) {
        qDebug(xr_logging, "Extension found: %s", extensionProperties.extensionName);
        discoveredExtensions.insert({ extensionProperties.extensionName, extensionProperties });
    }

    if (0 == discoveredExtensions.count(XR_EXT_DEBUG_UTILS_EXTENSION_NAME)) {
        enableDebug = false;
    }

    std::vector<const char*> requestedExtensions;
    std::vector<const char*> requestedLayers;
    if (0 == discoveredExtensions.count(XR_KHR_OPENGL_ENABLE_EXTENSION_NAME)) {
        qFatal("Required Graphics API extension not available: {}", XR_KHR_OPENGL_ENABLE_EXTENSION_NAME);
    }
    requestedExtensions.push_back(XR_KHR_OPENGL_ENABLE_EXTENSION_NAME);
    if (enableDebug) {
        // Try to add validation layer if found
        requestedExtensions.push_back(XR_EXT_DEBUG_UTILS_EXTENSION_NAME);
        if (discoveredLayers.count("XR_APILAYER_LUNARG_core_validation") != 0) {
            requestedLayers.push_back("XR_APILAYER_LUNARG_core_validation");
        }
    }

    xr::InstanceCreateInfo ici{ {},
                                { "interface", 0, "cakewalk", 0, xr::Version::current() },
                                (uint32_t)requestedLayers.size(),
                                requestedLayers.data(),
                                (uint32_t)requestedExtensions.size(),
                                requestedExtensions.data() };

    xr::DebugUtilsMessengerCreateInfoEXT dumci;
    if (enableDebug) {
        dumci.messageSeverities = xr::DebugUtilsMessageSeverityFlagBitsEXT::AllBits;
        dumci.messageTypes = xr::DebugUtilsMessageTypeFlagBitsEXT::AllBits;
        dumci.userData = this;
        dumci.userCallback = &xrDebugCallback;
        ici.next = &dumci;
    }

    // Create the actual instance
    instance = xr::createInstance(ici);

    // Having created the isntance, the very first thing to do is populate the dynamic dispatch, loading
    // all the available functions from the runtime
    dispatch = xr::DispatchLoaderDynamic::createFullyPopulated(instance, &xrGetInstanceProcAddr);

    // Turn on debug logging
    if (enableDebug) {
        messenger = createMessenger();
    }


    // Log the instance properties
    xr::InstanceProperties instanceProperties = instance.getInstanceProperties();
    qDebug(xr_logging, "OpenXR Runtime %s version %d.%d.%d", (const char*)instanceProperties.runtimeName,
           instanceProperties.runtimeVersion.major(), instanceProperties.runtimeVersion.minor(),
           instanceProperties.runtimeVersion.patch());

    // We want to create an HMD example, so we ask for a runtime that supposts that form factor
    // and get a response in the form of a systemId
    try {
        systemId = instance.getSystem({ xr::FormFactor::HeadMountedDisplay });
    } catch (const xr::exceptions::SystemError&) {
        return;
    }

    // Log the system properties
    {
        xr::SystemProperties systemProperties = instance.getSystemProperties(systemId);
        qCDebug(xr_logging, "OpenXR System %s max layers %d max swapchain image size %dx%d",  //
                (const char*)systemProperties.systemName, (uint32_t)systemProperties.graphicsProperties.maxLayerCount,
                (uint32_t)systemProperties.graphicsProperties.maxSwapchainImageWidth,
                (uint32_t)systemProperties.graphicsProperties.maxSwapchainImageHeight);
    }

    // Find out what view configurations we have available
    {
        auto viewConfigTypes = instance.enumerateViewConfigurationsToVector(systemId);
        auto viewConfigType = viewConfigTypes[0];
        if (viewConfigType != xr::ViewConfigurationType::PrimaryStereo) {
            qDebug(xr_logging, "OpenXR runtime doens't support primary stereo rendering");
            systemId = {};
            return;
        }
    }

    std::vector<xr::ViewConfigurationView> viewConfigViews =
        instance.enumerateViewConfigurationViewsToVector(systemId, xr::ViewConfigurationType::PrimaryStereo);

    // Instead of createing a swapchain per-eye, we create a single swapchain of double width.
    // Even preferable would be to create a swapchain texture array with one layer per eye, so that we could use the
    // VK_KHR_multiview to render both eyes with a single set of draws, but sadly the Oculus runtime doesn't currently
    // support texture array swapchains
    if (viewConfigViews.size() != 2 ||
        viewConfigViews[0].recommendedImageRectHeight != viewConfigViews[1].recommendedImageRectHeight) {
        qDebug(xr_logging, "Unable to parse OpenXR view configurations");
        systemId = {};
        return;
    }

    renderTargetSize = { viewConfigViews[0].recommendedImageRectWidth * 2, viewConfigViews[0].recommendedImageRectHeight };
    graphicsRequirements = instance.getOpenGLGraphicsRequirementsKHR(systemId, dispatch);
}


void Manager::pollEvents() {
    while (true) {
        xr::EventDataBuffer eventBuffer;
        auto pollResult = instance.pollEvent(eventBuffer);
        if (pollResult == xr::Result::EventUnavailable) {
            break;
        }

        switch (eventBuffer.type) {
            case xr::StructureType::EventDataSessionStateChanged: {
                const auto& e = reinterpret_cast<xr::EventDataSessionStateChanged&>(eventBuffer);
                SessionManager::STATE = e.state;
            } break;

            default:
                break;
        }
    }
}


SessionManager::SessionManager(const GraphicsBinding& graphicsBinding) {
    _session = _instance.instance.createSession({ {}, _instance.systemId, &graphicsBinding });
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
    const auto& renderTargetSize = _instance.renderTargetSize;
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
    _instance.pollEvents();

    if (STATE == xr::SessionState::Ready && !_sessionStarted) {
        _session.beginSession(xr::SessionBeginInfo{ xr::ViewConfigurationType::PrimaryStereo });
        // Force through an initial empty frame, or Oculus won't change to the focused state
        _nextFrame = FrameData::wait();
        emptyFrame();
        _instance.pollEvents();
        _sessionStarted = true;
    } else if (STATE == xr::SessionState::Stopping && !_sessionEnded) {
        _session.endSession();
        _sessionStarted = false;
    }

    if (!_sessionStarted) {
        return false;
    }

    switch (STATE) {
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

