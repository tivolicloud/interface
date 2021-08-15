//
//  Created by Bradley Austin Davis on 2018/10/21
//  Copyright 2014 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#include "RenderThread.h"
#include <QtGui/QWindow>
#include <gl/QOpenGLContextWrapper.h>
#include <QtPlatformHeaders/QWGLNativeContext>

void RenderThread::submitFrame(const gpu::FramePointer& frame) {
    std::unique_lock<std::mutex> lock(_frameLock);
    _pendingFrames.push(frame);
}

void RenderThread::resize(const QSize& newSize) {
    std::unique_lock<std::mutex> lock(_frameLock);
    _pendingSize.push(newSize);
}

void RenderThread::move(const glm::vec3& v) {
    std::unique_lock<std::mutex> lock(_frameLock);
    _correction = glm::inverse(glm::translate(mat4(), v)) * _correction;
}

void RenderThread::initialize(QWindow* window) {
    std::unique_lock<std::mutex> lock(_frameLock);
    setObjectName("RenderThread");
    Parent::initialize();

    _instanceManager.prepareInstance();
    _instanceManager.prepareSystem();

    _window = window;
    _window->setFormat(getDefaultOpenGLSurfaceFormat());
    _context.setWindow(window);
    _context.create();
    if (!_context.makeCurrent()) {
        qFatal("Unable to make context current");
    }

    const auto& renderTargetSize = _instanceManager.getRenderTargetSize();

    glGenTextures(1, &_externalTexture);
    glBindTexture(GL_TEXTURE_2D, _externalTexture);
    static const glm::u8vec4 color{ 0 };
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &color);

    // Create a depth renderbuffer compatible with the Swapchain sample count and size
    glGenRenderbuffers(1, &fbo.depthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, fbo.depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, renderTargetSize.x, renderTargetSize.y);

    // Create a framebuffer and attach the depth buffer to it
    glCreateFramebuffers(1, &fbo.id);
    glNamedFramebufferRenderbuffer(fbo.id, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, fbo.depthBuffer);


    gl::setSwapInterval(0);
    // GPU library init
    gpu::Context::init<gpu::gl::GLBackend>();
    _context.makeCurrent();
    _gpuContext = std::make_shared<gpu::Context>();
    _backend = _gpuContext->getBackend();
    _context.doneCurrent();
    _context.moveToThread(_thread);
    
    if (!_presentPipeline) {
        gpu::ShaderPointer program = gpu::Shader::createProgram(shader::gpu::program::DrawTexture);
        gpu::StatePointer state = gpu::StatePointer(new gpu::State());
        _presentPipeline = gpu::Pipeline::create(program, state);
    }
}

void RenderThread::setup() {
    // Wait until the context has been moved to this thread
    { std::unique_lock<std::mutex> lock(_frameLock); }
    _gpuContext->beginFrame();
    _gpuContext->endFrame();
    _context.makeCurrent();

    _instanceManager.prepareSession({ wglGetCurrentDC(), wglGetCurrentContext() });
    _instanceManager.prepareSwapchain();

    layersPointers.push_back(&projectionLayer);
    projectionLayer.viewCount = 2;
    projectionLayer.views = projectionLayerViews.data();
    projectionLayer.space = _instanceManager.getSpace();
    const auto& renderTargetSize = _instanceManager.getRenderTargetSize();
    // Finish setting up the layer submission
    xrs::for_each_side_index([&](uint32_t eyeIndex) {
        auto& layerView = projectionLayerViews[eyeIndex];
        layerView.subImage.swapchain = _instanceManager.getSwapchain();
        layerView.subImage.imageRect.extent = xr::Extent2Di{ (int32_t)renderTargetSize.x / 2, (int32_t)renderTargetSize.y };
        if (eyeIndex == 1) {
            layerView.subImage.imageRect.offset.x = layerView.subImage.imageRect.extent.width;
        }
    });

    _instanceManager.prepareActions();
    _instanceManager.pollEvents();
    _elapsed.start();
}

void RenderThread::shutdown() {
    _activeFrame.reset();
    while (!_pendingFrames.empty()) {
        _gpuContext->consumeFrameUpdates(_pendingFrames.front());
        _pendingFrames.pop();
    }
    _gpuContext->shutdown();
    _gpuContext.reset();
}


void RenderThread::renderFrame() {
    //_context.makeCurrent();
    _frameState = _instanceManager.waitFrame();
    if (!_instanceManager.beginFrame() || !_frameState.shouldRender || !_activeFrame) {
        _instanceManager.endFrame({ _frameState.predictedDisplayTime, xr::EnvironmentBlendMode::Opaque, 0, nullptr });
        return;
    }

    ++_presentCount;

    const auto& eyeViewStates = _instanceManager.getUpdatedViewStates(_frameState.predictedDisplayTime);
    auto& stereoState = _activeFrame->stereoState;
    stereoState._enable = true;
    stereoState._skybox = true;

    xrs::for_each_side_index([&](size_t eyeIndex) {
        const auto& viewState = eyeViewStates[eyeIndex];
        auto& layerView = projectionLayerViews[eyeIndex];
        stereoState._eyeProjections[eyeIndex] = xrs::toGlm(viewState.fov);
        //stereoState._eyeViews[eyeIndex] = glm::inverse(xrs::toGlm(viewState.pose));
        layerView.fov = viewState.fov;
        layerView.pose = viewState.pose;
    });

    auto hmdCorrection = _correction * xrs::toGlm(eyeViewStates[0].pose);
    _backend->setStereoState(stereoState);
    _backend->setCameraCorrection(glm::inverse(hmdCorrection), _activeFrame->view);
    //_prevRenderView = _correction * _activeFrame->view;
    _backend->recycle();
    _backend->syncCache();

    auto& glbackend = (gpu::gl::GLBackend&)(*_backend);
    const auto& framebuffer = _activeFrame->framebuffer;
    glm::uvec2 fboSize{ framebuffer->getWidth(), framebuffer->getHeight() };
    auto gpu_fbo = glbackend.getFramebufferID(framebuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, gpu_fbo);
    glClearColor(0, 0, 0, 1);
    glClearDepth(0);
    glClear(GL_DEPTH_BUFFER_BIT);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    if (!_activeFrame->batches.empty()) {
        _gpuContext->executeFrame(_activeFrame);
    }

    auto renderSize = _instanceManager.getRenderTargetSize();
    uint32_t destImage = _instanceManager.getNextSwapchainImage();

    glNamedFramebufferTexture(fbo.id, GL_COLOR_ATTACHMENT0, destImage, 0);
    (void)CHECK_GL_ERROR();
    glBlitNamedFramebuffer(
        gpu_fbo, fbo.id, 
        0, 0,  fboSize.x, fboSize.y, 
        0, renderSize.y, renderSize.x,  0,
        GL_COLOR_BUFFER_BIT, GL_NEAREST);
    glNamedFramebufferTexture(fbo.id, GL_COLOR_ATTACHMENT0, 0, 0);



    _instanceManager.commitSwapchainImage();
    _instanceManager.endFrame(_frameState.predictedDisplayTime, layersPointers);

    //static gpu::BatchPointer batch = nullptr;
    //if (!batch) {
    //    batch = std::make_shared<gpu::Batch>();
    //    batch->setPipeline(_presentPipeline);
    //    batch->setFramebuffer(nullptr);
    //    batch->setResourceTexture(0, framebuffer->getRenderBuffer(0));
    //    batch->setViewportTransform(ivec4(uvec2(0), ivec2(windowSize.width(), windowSize.height())));
    //    batch->draw(gpu::TRIANGLE_STRIP, 4);
    //}
    //glDisable(GL_FRAMEBUFFER_SRGB);
    //_gpuContext->executeBatch(*batch);
    //_context.swapBuffers();
    //_context.doneCurrent();
}

bool RenderThread::process() {
    _instanceManager.pollEvents();

    auto sessionState = _instanceManager.getSessionState();
    _frameState = xr::FrameState{};
    switch (sessionState) {
        case xr::SessionState::Stopping:
            //_instanceManager.endSession();
            break;

        case xr::SessionState::Ready:
            _instanceManager.beginSession();
            // fallthrough

        case xr::SessionState::Focused:
        case xr::SessionState::Synchronized:
        case xr::SessionState::Visible:
            _instanceManager.pollActions();
            break;

        default:
            break;
    }

    std::queue<gpu::FramePointer> pendingFrames;
    std::queue<QSize> pendingSize;

    {
        std::unique_lock<std::mutex> lock(_frameLock);
        pendingFrames.swap(_pendingFrames);
        pendingSize.swap(_pendingSize);
    }

    while (!pendingFrames.empty()) {
        _activeFrame = pendingFrames.front();
        pendingFrames.pop();
        _gpuContext->consumeFrameUpdates(_activeFrame);
    }
    while (!pendingSize.empty()) {
        pendingSize.pop();
    }

    if (_instanceManager.isSessionActive()) {
        renderFrame();
    }

    return true;
}

