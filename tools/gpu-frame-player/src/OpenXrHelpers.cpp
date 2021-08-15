#include "OpenXrHelpers.hpp"

#include <unordered_map>

#include <QtCore/QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(xr_logging)
Q_LOGGING_CATEGORY(xr_logging, "hifi.xr")

#if !defined(GL_SRGB8_ALPHA8)
#define GL_SRGB8_ALPHA8 0x8C43
#endif

using namespace xrs;
using namespace xrs::DebugUtilsEXT;

static XrBool32 debugCallback(XrDebugUtilsMessageSeverityFlagsEXT sev_,
                              XrDebugUtilsMessageTypeFlagsEXT type_,
                              const XrDebugUtilsMessengerCallbackDataEXT* data_,
                              void* userData) {
    auto instance = reinterpret_cast<InstanceManager*>(userData);

    qDebug(xr_logging, "%s: %s", data_->functionName, data_->message);
    return XR_TRUE;
}

Messenger InstanceManager::createMessenger(const MessageSeverityFlags& severityFlags, const MessageTypeFlags& typeFlags) {
    return instance.createDebugUtilsMessengerEXT({ severityFlags, typeFlags, debugCallback, this },
                                                 xr::DispatchLoaderDynamic{ instance });
}

void InstanceManager::prepareInstance() {

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
        if (0 != discoveredLayers.count(XR_EXTX_OVERLAY_EXTENSION_NAME)) {

        }
        requestedExtensions.push_back(XR_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    xr::InstanceCreateInfo ici{ {},
                                { "gpu_frame_player", 0, "cakewalk", 0, xr::Version::current() },
                                0,
                                nullptr,
                                (uint32_t)requestedExtensions.size(),
                                requestedExtensions.data() };

    xr::DebugUtilsMessengerCreateInfoEXT dumci;
    if (enableDebug) {
        dumci.messageSeverities = xr::DebugUtilsMessageSeverityFlagBitsEXT::AllBits;
        dumci.messageTypes = xr::DebugUtilsMessageTypeFlagBitsEXT::AllBits;
        dumci.userData = this;
        dumci.userCallback = &debugCallback;
        ici.next = &dumci;
    }

    // Create the actual instance
    instance = xr::createInstance(ici);

    // Turn on debug logging
    if (enableDebug) {
        messenger = createMessenger();
    }

    // Having created the isntance, the very first thing to do is populate the dynamic dispatch, loading
    // all the available functions from the runtime
    dispatch = xr::DispatchLoaderDynamic::createFullyPopulated(instance, &xrGetInstanceProcAddr);

    // Log the instance properties
    xr::InstanceProperties instanceProperties = instance.getInstanceProperties();
    qDebug(xr_logging, "OpenXR Runtime %s version %d.%d.%d", (const char*)instanceProperties.runtimeName,
           instanceProperties.runtimeVersion.major(), instanceProperties.runtimeVersion.minor(),
           instanceProperties.runtimeVersion.patch());
}

void InstanceManager::prepareSystem() {
    // We want to create an HMD example, so we ask for a runtime that supposts that form factor
    // and get a response in the form of a systemId
    systemId = instance.getSystem(xr::SystemGetInfo{ xr::FormFactor::HeadMountedDisplay });

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
            throw std::runtime_error("Example only supports stereo-based HMD rendering");
        }
        //xr::ViewConfigurationProperties viewConfigProperties =
        //    instance.getViewConfigurationProperties(systemId, viewConfigType);
        //logging::log(logging::Level::Info, fmt::format(""));
    }

    std::vector<xr::ViewConfigurationView> viewConfigViews =
        instance.enumerateViewConfigurationViewsToVector(systemId, xr::ViewConfigurationType::PrimaryStereo);

    // Instead of createing a swapchain per-eye, we create a single swapchain of double width.
    // Even preferable would be to create a swapchain texture array with one layer per eye, so that we could use the
    // VK_KHR_multiview to render both eyes with a single set of draws, but sadly the Oculus runtime doesn't currently
    // support texture array swapchains
    if (viewConfigViews.size() != 2) {
        throw std::runtime_error("Unexpected number of view configurations");
    }

    if (viewConfigViews[0].recommendedImageRectHeight != viewConfigViews[1].recommendedImageRectHeight) {
        throw std::runtime_error("Per-eye images have different recommended heights");
    }

    renderTargetSize = { viewConfigViews[0].recommendedImageRectWidth * 2, viewConfigViews[0].recommendedImageRectHeight };

    graphicsRequirements = instance.getOpenGLGraphicsRequirementsKHR(systemId, dispatch);
}

void InstanceManager::prepareSession(xr::GraphicsBindingOpenGLWin32KHR graphicsBinding) {
    //xr::GraphicsBindingOpenGLWin32KHR graphicsBinding{ wglGetCurrentDC(), wglGetCurrentContext() };
    xr::SessionCreateInfo sci{ {}, systemId };
    sci.next = &graphicsBinding;
    session = instance.createSession(sci);

    auto referenceSpaces = session.enumerateReferenceSpacesToVector();
    space = session.createReferenceSpace({ xr::ReferenceSpaceType::Local, {} });
    auto swapchainFormats = session.enumerateSwapchainFormatsToVector();
}

void InstanceManager::prepareSwapchain() {
    swapchainCreateInfo.usageFlags = xr::SwapchainUsageFlagBits::TransferDst;
    swapchainCreateInfo.format = (int64_t)GL_SRGB8_ALPHA8;
    swapchainCreateInfo.sampleCount = 1;
    swapchainCreateInfo.arraySize = 1;
    swapchainCreateInfo.faceCount = 1;
    swapchainCreateInfo.mipCount = 1;
    swapchainCreateInfo.width = renderTargetSize.x;
    swapchainCreateInfo.height = renderTargetSize.y;
    swapchain = session.createSwapchain(swapchainCreateInfo);
    swapchainImages = swapchain.enumerateSwapchainImagesToVector<xr::SwapchainImageOpenGLKHR>();
}

void InstanceManager::prepareActions() {
    //// Create an action set.
    //actionSet = instance.createActionSet({ "gameplay", "gameplay", 0 });
    //actionState.handSubactionPath[xr::Side::Left] = instance.stringToPath("/user/hand/left");
    //actionState.handSubactionPath[xr::Side::Right] = instance.stringToPath("/user/hand/right");

    //// Create actions.
    //{
    //    XrActionCreateInfo actionInfo{ XR_TYPE_ACTION_CREATE_INFO };
    //    // Create an input action for grabbing objects with the left and right hands.
    //    //actionInfo.actionType = XR_ACTION_TYPE_FLOAT_INPUT;
    //    //strcpy_s(actionInfo.actionName, "grab_object");
    //    //strcpy_s(actionInfo.localizedActionName, "Grab Object");
    //    //actionInfo.countSubactionPaths = uint32_t(m_input.handSubactionPath.size());
    //    //actionInfo.subactionPaths = m_input.handSubactionPath.data();
    //    //CHECK_XRCMD(xrCreateAction(m_input.actionSet, &actionInfo, &m_input.grabAction));

    //    // Create an input action getting the left and right hand poses.
    //    actionState.poseAction = actionSet.createAction(
    //        { "hand_pose", xr::ActionType::PoseInput, 2, actionState.handSubactionPath.data(), "Hand Pose" });

    //    xr::for_each_side_index([&](uint32_t eyeIndex) {
    //        actionState.handSpace[eyeIndex] =
    //            session.createActionSpace({ actionState.poseAction, actionState.handSubactionPath[eyeIndex] });
    //    });

    //    // Create output actions for vibrating the left and right controller.
    //    actionState.vibrateAction = actionSet.createAction(
    //        { "vibrate_hand", xr::ActionType::VibrationOutput, 2, actionState.handSubactionPath.data(), "Vibrate Hand" });

    //    // Create input actions for quitting the session using the left and right controller.
    //    // Since it doesn't matter which hand did this, we do not specify subaction paths for it.
    //    // We will just suggest bindings for both hands, where possible.
    //    actionState.quitAction = actionSet.createAction(
    //        { "quit_session", xr::ActionType::BooleanInput, 0, nullptr, "Quit Session" });
    //}

    //std::array<XrPath, Side::COUNT> selectPath;
    //std::array<XrPath, Side::COUNT> squeezeValuePath;
    //std::array<XrPath, Side::COUNT> squeezeForcePath;
    //std::array<XrPath, Side::COUNT> squeezeClickPath;
    //std::array<XrPath, Side::COUNT> posePath;
    //std::array<XrPath, Side::COUNT> hapticPath;
    //std::array<XrPath, Side::COUNT> menuClickPath;
    //std::array<XrPath, Side::COUNT> bClickPath;
    //std::array<XrPath, Side::COUNT> triggerValuePath;
    //CHECK_XRCMD(xrStringToPath(m_instance, "/user/hand/left/input/select/click", &selectPath[Side::LEFT]));
    //CHECK_XRCMD(xrStringToPath(m_instance, "/user/hand/right/input/select/click", &selectPath[Side::RIGHT]));
    //CHECK_XRCMD(xrStringToPath(m_instance, "/user/hand/left/input/squeeze/value", &squeezeValuePath[Side::LEFT]));
    //CHECK_XRCMD(xrStringToPath(m_instance, "/user/hand/right/input/squeeze/value", &squeezeValuePath[Side::RIGHT]));
    //CHECK_XRCMD(xrStringToPath(m_instance, "/user/hand/left/input/squeeze/force", &squeezeForcePath[Side::LEFT]));
    //CHECK_XRCMD(xrStringToPath(m_instance, "/user/hand/right/input/squeeze/force", &squeezeForcePath[Side::RIGHT]));
    //CHECK_XRCMD(xrStringToPath(m_instance, "/user/hand/left/input/squeeze/click", &squeezeClickPath[Side::LEFT]));
    //CHECK_XRCMD(xrStringToPath(m_instance, "/user/hand/right/input/squeeze/click", &squeezeClickPath[Side::RIGHT]));
    //CHECK_XRCMD(xrStringToPath(m_instance, "/user/hand/left/input/grip/pose", &posePath[Side::LEFT]));
    //CHECK_XRCMD(xrStringToPath(m_instance, "/user/hand/right/input/grip/pose", &posePath[Side::RIGHT]));
    //CHECK_XRCMD(xrStringToPath(m_instance, "/user/hand/left/output/haptic", &hapticPath[Side::LEFT]));
    //CHECK_XRCMD(xrStringToPath(m_instance, "/user/hand/right/output/haptic", &hapticPath[Side::RIGHT]));
    //CHECK_XRCMD(xrStringToPath(m_instance, "/user/hand/left/input/menu/click", &menuClickPath[Side::LEFT]));
    //CHECK_XRCMD(xrStringToPath(m_instance, "/user/hand/right/input/menu/click", &menuClickPath[Side::RIGHT]));
    //CHECK_XRCMD(xrStringToPath(m_instance, "/user/hand/left/input/b/click", &bClickPath[Side::LEFT]));
    //CHECK_XRCMD(xrStringToPath(m_instance, "/user/hand/right/input/b/click", &bClickPath[Side::RIGHT]));
    //CHECK_XRCMD(xrStringToPath(m_instance, "/user/hand/left/input/trigger/value", &triggerValuePath[Side::LEFT]));
    //CHECK_XRCMD(xrStringToPath(m_instance, "/user/hand/right/input/trigger/value", &triggerValuePath[Side::RIGHT]));
    //// Suggest bindings for KHR Simple.
    //{
    //    XrPath khrSimpleInteractionProfilePath;
    //    CHECK_XRCMD(
    //        xrStringToPath(m_instance, "/interaction_profiles/khr/simple_controller", &khrSimpleInteractionProfilePath));
    //    std::vector<XrActionSuggestedBinding> bindings{ { // Fall back to a click input for the grab action.
    //                                                      { m_input.grabAction, selectPath[Side::LEFT] },
    //                                                      { m_input.grabAction, selectPath[Side::RIGHT] },
    //                                                      { m_input.poseAction, posePath[Side::LEFT] },
    //                                                      { m_input.poseAction, posePath[Side::RIGHT] },
    //                                                      { m_input.quitAction, menuClickPath[Side::LEFT] },
    //                                                      { m_input.quitAction, menuClickPath[Side::RIGHT] },
    //                                                      { m_input.vibrateAction, hapticPath[Side::LEFT] },
    //                                                      { m_input.vibrateAction, hapticPath[Side::RIGHT] } } };
    //    XrInteractionProfileSuggestedBinding suggestedBindings{ XR_TYPE_INTERACTION_PROFILE_SUGGESTED_BINDING };
    //    suggestedBindings.interactionProfile = khrSimpleInteractionProfilePath;
    //    suggestedBindings.suggestedBindings = bindings.data();
    //    suggestedBindings.countSuggestedBindings = (uint32_t)bindings.size();
    //    CHECK_XRCMD(xrSuggestInteractionProfileBindings(m_instance, &suggestedBindings));
    //}
    //// Suggest bindings for the Oculus Touch.
    //{
    //    XrPath oculusTouchInteractionProfilePath;
    //    CHECK_XRCMD(
    //        xrStringToPath(m_instance, "/interaction_profiles/oculus/touch_controller", &oculusTouchInteractionProfilePath));
    //    std::vector<XrActionSuggestedBinding> bindings{ { { m_input.grabAction, squeezeValuePath[Side::LEFT] },
    //                                                      { m_input.grabAction, squeezeValuePath[Side::RIGHT] },
    //                                                      { m_input.poseAction, posePath[Side::LEFT] },
    //                                                      { m_input.poseAction, posePath[Side::RIGHT] },
    //                                                      { m_input.quitAction, menuClickPath[Side::LEFT] },
    //                                                      { m_input.vibrateAction, hapticPath[Side::LEFT] },
    //                                                      { m_input.vibrateAction, hapticPath[Side::RIGHT] } } };
    //    XrInteractionProfileSuggestedBinding suggestedBindings{ XR_TYPE_INTERACTION_PROFILE_SUGGESTED_BINDING };
    //    suggestedBindings.interactionProfile = oculusTouchInteractionProfilePath;
    //    suggestedBindings.suggestedBindings = bindings.data();
    //    suggestedBindings.countSuggestedBindings = (uint32_t)bindings.size();
    //    CHECK_XRCMD(xrSuggestInteractionProfileBindings(m_instance, &suggestedBindings));
    //}
    //// Suggest bindings for the Vive Controller.
    //{
    //    XrPath viveControllerInteractionProfilePath;
    //    CHECK_XRCMD(
    //        xrStringToPath(m_instance, "/interaction_profiles/htc/vive_controller", &viveControllerInteractionProfilePath));
    //    std::vector<XrActionSuggestedBinding> bindings{ { { m_input.grabAction, triggerValuePath[Side::LEFT] },
    //                                                      { m_input.grabAction, triggerValuePath[Side::RIGHT] },
    //                                                      { m_input.poseAction, posePath[Side::LEFT] },
    //                                                      { m_input.poseAction, posePath[Side::RIGHT] },
    //                                                      { m_input.quitAction, menuClickPath[Side::LEFT] },
    //                                                      { m_input.quitAction, menuClickPath[Side::RIGHT] },
    //                                                      { m_input.vibrateAction, hapticPath[Side::LEFT] },
    //                                                      { m_input.vibrateAction, hapticPath[Side::RIGHT] } } };
    //    XrInteractionProfileSuggestedBinding suggestedBindings{ XR_TYPE_INTERACTION_PROFILE_SUGGESTED_BINDING };
    //    suggestedBindings.interactionProfile = viveControllerInteractionProfilePath;
    //    suggestedBindings.suggestedBindings = bindings.data();
    //    suggestedBindings.countSuggestedBindings = (uint32_t)bindings.size();
    //    CHECK_XRCMD(xrSuggestInteractionProfileBindings(m_instance, &suggestedBindings));
    //}

    //// Suggest bindings for the Valve Index Controller.
    //{
    //    XrPath indexControllerInteractionProfilePath;
    //    CHECK_XRCMD(
    //        xrStringToPath(m_instance, "/interaction_profiles/valve/index_controller", &indexControllerInteractionProfilePath));
    //    std::vector<XrActionSuggestedBinding> bindings{ { { m_input.grabAction, squeezeForcePath[Side::LEFT] },
    //                                                      { m_input.grabAction, squeezeForcePath[Side::RIGHT] },
    //                                                      { m_input.poseAction, posePath[Side::LEFT] },
    //                                                      { m_input.poseAction, posePath[Side::RIGHT] },
    //                                                      { m_input.quitAction, bClickPath[Side::LEFT] },
    //                                                      { m_input.quitAction, bClickPath[Side::RIGHT] },
    //                                                      { m_input.vibrateAction, hapticPath[Side::LEFT] },
    //                                                      { m_input.vibrateAction, hapticPath[Side::RIGHT] } } };
    //    XrInteractionProfileSuggestedBinding suggestedBindings{ XR_TYPE_INTERACTION_PROFILE_SUGGESTED_BINDING };
    //    suggestedBindings.interactionProfile = indexControllerInteractionProfilePath;
    //    suggestedBindings.suggestedBindings = bindings.data();
    //    suggestedBindings.countSuggestedBindings = (uint32_t)bindings.size();
    //    CHECK_XRCMD(xrSuggestInteractionProfileBindings(m_instance, &suggestedBindings));
    //}

    //// Suggest bindings for the Microsoft Mixed Reality Motion Controller.
    //{
    //    XrPath microsoftMixedRealityInteractionProfilePath;
    //    CHECK_XRCMD(xrStringToPath(m_instance, "/interaction_profiles/microsoft/motion_controller",
    //                               &microsoftMixedRealityInteractionProfilePath));
    //    std::vector<XrActionSuggestedBinding> bindings{ { { m_input.grabAction, squeezeClickPath[Side::LEFT] },
    //                                                      { m_input.grabAction, squeezeClickPath[Side::RIGHT] },
    //                                                      { m_input.poseAction, posePath[Side::LEFT] },
    //                                                      { m_input.poseAction, posePath[Side::RIGHT] },
    //                                                      { m_input.quitAction, menuClickPath[Side::LEFT] },
    //                                                      { m_input.quitAction, menuClickPath[Side::RIGHT] },
    //                                                      { m_input.vibrateAction, hapticPath[Side::LEFT] },
    //                                                      { m_input.vibrateAction, hapticPath[Side::RIGHT] } } };
    //    XrInteractionProfileSuggestedBinding suggestedBindings{ XR_TYPE_INTERACTION_PROFILE_SUGGESTED_BINDING };
    //    suggestedBindings.interactionProfile = microsoftMixedRealityInteractionProfilePath;
    //    suggestedBindings.suggestedBindings = bindings.data();
    //    suggestedBindings.countSuggestedBindings = (uint32_t)bindings.size();
    //    CHECK_XRCMD(xrSuggestInteractionProfileBindings(m_instance, &suggestedBindings));
    //}
    //session.attachSessionActionSets({ 1, &actionSet });
}

void InstanceManager::pollActions() {
    //// Sync actions
    //const xr::ActiveActionSet activeActionSet{ actionSet };
    //auto result = session.syncActions({ 1, &activeActionSet });
}

void InstanceManager::beginSession() {
    if (!sessionActive) {
        sessionActive = true;
        session.beginSession(xr::SessionBeginInfo{ xr::ViewConfigurationType::PrimaryStereo });
        pollEvents();
    }
}

void InstanceManager::endSession() {
    if (sessionActive) {
        session.endSession();
        sessionActive = false;
    }
}

bool InstanceManager::beginFrame() {
    return xr::Result::Success == session.beginFrame(xr::FrameBeginInfo{});
}

void InstanceManager::endFrame(const xr::FrameEndInfo& frameEndInfo) {
    session.endFrame(frameEndInfo);
}

void InstanceManager::endFrame(xr::Time predictedDisplayTime,
                               const std::vector<xr::CompositionLayerBaseHeader*>& layers,
                               xr::EnvironmentBlendMode blendMode) {
    session.endFrame({ predictedDisplayTime, blendMode, (uint32_t)layers.size(), layers.data() });
}

const std::vector<xr::View>& InstanceManager::getUpdatedViewStates(xr::Time displayTime) {
    xr::ViewState vs;
    xr::ViewLocateInfo vi{ xr::ViewConfigurationType::PrimaryStereo, displayTime, space };
    eyeViewStates = session.locateViewsToVector(vi, &(vs.operator XrViewState&()));
    return eyeViewStates;
}

uint32_t InstanceManager::getNextSwapchainImage() {
    uint32_t swapchainIndex;
    swapchain.acquireSwapchainImage(xr::SwapchainImageAcquireInfo{}, &swapchainIndex);
    swapchain.waitSwapchainImage(xr::SwapchainImageWaitInfo{ xr::Duration::infinite() });
    return swapchainImages[swapchainIndex].image;
}

void InstanceManager::commitSwapchainImage() {
    swapchain.releaseSwapchainImage({});
}

void InstanceManager::pollEvents() {
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
                //sessionStateChanged(e.state, e.time);
            } break;

            default:
                break;
        }
    }
}
