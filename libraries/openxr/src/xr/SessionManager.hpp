//
//  Created by Bradley Austin Davis on 2021/10/23
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//
#pragma once
#include "Config.hpp"

#include "Manager.hpp"
#include "FrameData.hpp"

namespace xrs {

class SessionManager {
public:
    using Ptr = std::shared_ptr<SessionManager>;

    SessionManager(const GraphicsBinding& graphicsBinding);
    virtual ~SessionManager();
    bool shouldRender();
    void buildSwapchain();
    void emptyFrame();
    void endFrame(bool render = true);
    void beginFrame();

    uint32_t acquireSwapchainImage();
    void releaseSwapchainImage();

    const FrameData& getNextFrame() const { return _nextFrame;}

private:
    const xrs::Manager& manager{ Manager::get() };
    const xr::SessionState& sessionState{ manager.sessionState };
    const xr::Instance& instance{ manager.instance };
    const xr::SystemId& systemId{ manager.systemId };
    const xr::DispatchLoaderDynamic& dispatch{ manager.dispatch };


    bool _sessionStarted{ false };
    bool _sessionEnded{ false };
    bool _frameStarted{ false };
    xr::Session& _session{ xrs::Manager::get().session };
    xr::Swapchain _swapchain;
    std::vector<xr::SwapchainImageOpenGLKHR> _swapchainImages;

    std::array<xr::CompositionLayerProjectionView, 2> _projectionLayerViews;
    xr::CompositionLayerProjection _projectionLayer{ {}, {}, 2, _projectionLayerViews.data() };
    std::vector<xr::CompositionLayerBaseHeader*> _layersPointers{ &_projectionLayer };
    xr::Space& _space{ _projectionLayer.space };

    FrameData _nextFrame;
    const xr::FrameState& _frameState{ _nextFrame.frameState };
    const xr::ViewState& _viewState{ _nextFrame.viewState };

    xr::FrameEndInfo _frameEndInfo{ xr::Time{}, xr::EnvironmentBlendMode::Opaque, (uint32_t)_layersPointers.size(),
                                    _layersPointers.data() };

    QThread* _sessionThread{ nullptr };
};

} // namespace xrs