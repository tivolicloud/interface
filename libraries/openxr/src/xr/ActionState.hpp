//
//  Created by Bradley Austin Davis on 2021/10/23
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//
#pragma once

#include "Config.hpp"
#include <unordered_map>

namespace xrs {

struct ActionState {
    xr::ActionSet actionSet;
    xr::Action grabAction{ XR_NULL_HANDLE };
    xr::Action poseAction{ XR_NULL_HANDLE };
    xr::Action footPoseAction{ XR_NULL_HANDLE };
    xr::Action vibrateAction{ XR_NULL_HANDLE };
    xr::Action quitAction{ XR_NULL_HANDLE };
    std::array<xr::Path, SIDE_COUNT> handSubactionPath;
    std::array<xr::Space, SIDE_COUNT> handSpace;
    std::array<float, SIDE_COUNT> handScale = { { 1.0f, 1.0f } };
    std::array<xr::Bool32, SIDE_COUNT> handActive;
    std::unordered_map<std::string, xr::Path> trackerPaths;
    void init();
    void sync();
};

}  // namespace xrs
