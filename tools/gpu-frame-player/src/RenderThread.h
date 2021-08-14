//
//  Created by Bradley Austin Davis on 2018/10/21
//  Copyright 2014 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#pragma once

#include <QtCore/QElapsedTimer>

#include <GenericThread.h>
#include <shared/RateCounter.h>
#include "OpenXrHelpers.hpp"

#include <gl/Config.h>
#include <gl/Context.h>
#include <gpu/gl/GLBackend.h>

class RenderThread : public GenericThread {
    using Parent = GenericThread;
public:
    QWindow* _window{ nullptr };

    gl::OffscreenContext _context;

    std::mutex _mutex;
    gpu::ContextPointer _gpuContext;  // initialized during window creation
    std::shared_ptr<gpu::Backend> _backend;
    std::atomic<size_t> _presentCount{ 0 };
    QElapsedTimer _elapsed;
    size_t _frameIndex{ 0 };
    std::mutex _frameLock;
    std::queue<gpu::FramePointer> _pendingFrames;
    std::queue<QSize> _pendingSize;
    gpu::FramePointer _activeFrame;
    uint32_t _externalTexture{ 0 };
    void move(const glm::vec3& v);
    glm::mat4 _correction;
    gpu::PipelinePointer _presentPipeline;

    // OpenXR
    xr::FrameState _frameState;
    xrs::InstanceManager _instanceManager;
    std::array<xr::CompositionLayerProjectionView, 2> projectionLayerViews;
    xr::CompositionLayerProjection projectionLayer{ {}, {}, 2, projectionLayerViews.data() };
    std::vector<xr::CompositionLayerBaseHeader*> layersPointers;

    struct GLFBO {
        GLuint id{ 0 };
        GLuint depthBuffer{ 0 };
    } fbo;

    void resize(const QSize& newSize);
    void setup() override;
    bool process() override;
    void shutdown() override;

    void submitFrame(const gpu::FramePointer& frame);
    void initialize(QWindow* window);
    void renderFrame();
};
