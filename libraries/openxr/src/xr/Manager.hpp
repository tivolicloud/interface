//
//  Created by Bradley Austin Davis on 2021/10/23
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//
#pragma once
#include "Config.hpp"

namespace xrs {

class Manager {
    using GraphicsRequirements = xr::GraphicsRequirementsOpenGLKHR;
    using Messenger = xrs::DebugUtils::Messenger;
    using MessageSeverityFlags= xrs::DebugUtils::MessageSeverityFlags;
    using MessageTypeFlags = xrs::DebugUtils::MessageTypeFlags;
    using CallbackData = xrs::DebugUtils::CallbackData;
public:
    static Manager& get();
    static void pollEvents();
    void init();
public:
    void onEvent(const xr::EventDataBuffer& eventBuffer);

public:
    xr::SessionState sessionState;
    xr::Instance instance;
    xr::Session session;
    xr::SystemId systemId;
    xr::DispatchLoaderDynamic dispatch;

    bool enableDebug{ true };
    Messenger messenger;
    glm::uvec2 renderTargetSize;
    GraphicsRequirements graphicsRequirements;
    bool quitRequested{ false };
};

}

