//
//  Created by Bradley Austin Davis on 2015/11/01
//  Copyright 2015 High Fidelity, Inc.
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

#include <unordered_map>


Q_LOGGING_CATEGORY(xr_logging, "hifi.xr")

using namespace xrs;
using namespace xrs::DebugUtilsEXT;

Manager& Manager::get() {
    static Manager instance;
    return instance;
}

XrBool32 __stdcall xrDebugCallback(XrDebugUtilsMessageSeverityFlagsEXT messageSeverity,
                                 XrDebugUtilsMessageTypeFlagsEXT messageTypes,
                                 const XrDebugUtilsMessengerCallbackDataEXT* callbackData,
                                 void* userData) {
    auto manager = reinterpret_cast<Manager*>(userData);
    const auto sev = xrs::DebugUtilsEXT::MessageSeverityFlags{ messageSeverity };
    const auto type = xrs::DebugUtilsEXT::MessageTypeFlags{ messageSeverity };
    return manager->debugCallback(sev, type, callbackData);
}

XrBool32 Manager::debugCallback(const MessageSeverity& messageSeverity,
                              const MessageType& messageType,
                              const CallbackData& callbackData) {
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
    }

    xr::InstanceCreateInfo ici{ {},
                                { "interface", 0, "cakewalk", 0, xr::Version::current() },
                                0,
                                nullptr,
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
                sessionState = e.state;
            } break;

            default:
                break;
        }
    }
}
