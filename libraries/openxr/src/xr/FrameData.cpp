//
//  Created by Bradley Austin Davis on 2021/10/23
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//
#include "FrameData.hpp"

using namespace xrs;

xr::Session FrameData::SESSION;
xr::Space FrameData::SPACE;
XrTime FrameData::START_TIME{ 0 };

void FrameData::updateViews() {
    XrViewState* viewStatePtr = &(viewState.operator XrViewState&());
    views =
        SESSION.locateViewsToVector({ xr::ViewConfigurationType::PrimaryStereo, frameState.predictedDisplayTime, SPACE },
                                    viewStatePtr);
}

FrameData FrameData::wait() {
    FrameData result;
    result.frameState = SESSION.waitFrame({});
    if (0 == START_TIME) {
        START_TIME = result.frameState.predictedDisplayTime.get();
    }
    result.updateViews();
    return result;
}

double FrameData::toSeconds(xr::Time xrtime) {
    auto time = xrtime.get() - START_TIME;
    time /= 1000;
    double result = time;
    result /= 1000.0;
    return result;
}

FrameData FrameData::next() const {
    FrameData result;
    result.frameState = frameState;
    result.frameState.predictedDisplayTime += frameState.predictedDisplayPeriod;
    result.updateViews();
    return result;
}
