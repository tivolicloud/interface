//
//  Created by Bradley Austin Davis on 2021/10/23
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//
#pragma once
#include "Config.hpp"

namespace xrs {

class FrameData {
public:
    static xr::Session SESSION;
    static xr::Space SPACE;
    static XrTime START_TIME;

    xr::FrameState frameState;
    xr::ViewState viewState;
    std::vector<xr::View> views;

    static FrameData wait();
    static double toSeconds(xr::Time xrtime);

    void updateViews();
    FrameData next() const;
};

}  // namespace xrs
