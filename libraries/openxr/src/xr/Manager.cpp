//
//  Created by Bradley Austin Davis on 2021/08/14
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#include "Manager.hpp"

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

using namespace xrs;
using namespace xrs::DebugUtils;

static std::vector<const char*> REQUIRED_EXTENSIONS{ {
    XR_KHR_OPENGL_ENABLE_EXTENSION_NAME,
} };

static std::vector<const char*> DESIRED_EXTENSIONS{ {
    "XR_EXT_hand_joints_motion_range",
    "XR_EXT_hand_tracking",
    "XR_HTC_vive_cosmos_controller_interaction",
    "XR_HTCX_vive_tracker_interaction",
    "XR_KHR_visibility_mask",
    "XR_OCULUS_audio_device_guid",
    "XR_VALVE_analog_threshold",
} };

Manager& Manager::get() {
    static Manager manager;
    return manager;
}

void Manager::pollEvents() {
    static Manager& manager = Manager::get();
    static xr::EventDataBuffer eventBuffer;
    while (true) {
        auto pollResult = manager.instance.pollEvent(eventBuffer);
        if (pollResult == xr::Result::EventUnavailable) {
            break;
        }
        manager.onEvent(eventBuffer);
    }
}

void Manager::init() {
    std::unordered_map<std::string, xr::ApiLayerProperties> discoveredLayers;
    for (const auto& layerProperties : xr::enumerateApiLayerPropertiesToVector()) {
        qDebug(xr_logging, "Layer found: %s", layerProperties.layerName);
        discoveredLayers.insert({ layerProperties.layerName, layerProperties });
    }

    std::unordered_map<std::string, xr::ExtensionProperties> discoveredExtensions;
    for (const auto& extensionProperties : xr::enumerateInstanceExtensionPropertiesToVector(nullptr)) {
        //qDebug(xr_logging, "Extension found: %s", extensionProperties.extensionName);
        discoveredExtensions.insert({ extensionProperties.extensionName, extensionProperties });
    }

    if (0 == discoveredExtensions.count(XR_EXT_DEBUG_UTILS_EXTENSION_NAME)) {
        enableDebug = false;
    }
    enableDebug = false;

    std::vector<const char*> requestedExtensions;
    std::vector<const char*> requestedLayers;
    for (const auto& requiredExtension : REQUIRED_EXTENSIONS) {
        if (0 == discoveredExtensions.count(requiredExtension)) {
            qFatal("Required extension not available: {}", requiredExtension);
        }
        requestedExtensions.push_back(requiredExtension);
    }

    //for (const auto& desiredExtension : DESIRED_EXTENSIONS) {
    //    if (0 == discoveredExtensions.count(desiredExtension)) {
    //        qInfo("Desired extension not available: {}", desiredExtension);
    //        continue;
    //    }
    //    requestedExtensions.push_back(desiredExtension);
    //}

    if (enableDebug) {
        // Try to add validation layer if found
        requestedExtensions.push_back(XR_EXT_DEBUG_UTILS_EXTENSION_NAME);
        if (discoveredLayers.count("XR_APILAYER_LUNARG_core_validation") != 0) {
            requestedLayers.push_back("XR_APILAYER_LUNARG_core_validation");
        }
    }

    xr::InstanceCreateInfo ici{ {},
                                { "tivoli", 0, "samcake", 0, xr::Version::current() },
                                (uint32_t)requestedLayers.size(),
                                requestedLayers.data(),
                                (uint32_t)requestedExtensions.size(),
                                requestedExtensions.data() };

    xr::DebugUtilsMessengerCreateInfoEXT dumci;
    if (enableDebug) {
        dumci.messageSeverities = xr::DebugUtilsMessageSeverityFlagBitsEXT::AllBits;
        dumci.messageTypes = xr::DebugUtilsMessageTypeFlagBitsEXT::AllBits;
        dumci.userData = this;
        dumci.userCallback = &xrsDebugCallback;
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

void Manager::onEvent(const xr::EventDataBuffer& eventBuffer) {
    switch (eventBuffer.type) {
        case xr::StructureType::EventDataSessionStateChanged: {
            const auto& e = reinterpret_cast<const xr::EventDataSessionStateChanged&>(eventBuffer);
            sessionState = e.state;
        } break;

        default:
            break;
    }
}

