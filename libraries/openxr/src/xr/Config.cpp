//
//  Created by Bradley Austin Davis on 2021/10/23
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#include "Config.hpp"
#include "Manager.hpp"

Q_LOGGING_CATEGORY(xr_logging, "hifi.xr")

static const auto& manager = xrs::Manager::get();
static const auto& sessionState = manager.sessionState;
static const auto& instance = manager.instance;
static const auto& systemId = manager.systemId;
static const auto& dispatch = manager.dispatch;

using namespace xrs;


XrBool32 __stdcall xrsDebugCallback(XrDebugUtilsMessageSeverityFlagsEXT messageSeverity,
                                   XrDebugUtilsMessageTypeFlagsEXT messageTypes,
                                   const XrDebugUtilsMessengerCallbackDataEXT* callbackData,
                                   void* userData) {
    constexpr auto WARN_BITS = XR_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | XR_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

    if (0 != (WARN_BITS & messageSeverity)) {
        qWarning() << "QQQ " << callbackData->message;
    } else {
        qDebug() << "ZZZ " << callbackData->message;
    }
    return XR_TRUE;
}


DebugUtils::Messenger DebugUtils::createMessenger(const DebugUtils::MessageSeverityFlags& severityFlags,
                                      const DebugUtils::MessageTypeFlags& typeFlags) {
    return instance.createDebugUtilsMessengerEXT({ severityFlags, typeFlags, xrsDebugCallback, (void*)&manager },
                                                 xr::DispatchLoaderDynamic{ instance });
}
