//
//  Created by Sam Gondelman on 5/16/19
//  Copyright 2013-2019 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//
#include "RenderScriptingInterface.h"

#include "LightingModel.h"
#include "AntialiasingEffect.h"

RenderScriptingInterface* RenderScriptingInterface::getInstance() {
    static RenderScriptingInterface sharedInstance;
    return &sharedInstance;
}

std::once_flag RenderScriptingInterface::registry_flag;

RenderScriptingInterface::RenderScriptingInterface() {
    std::call_once(registry_flag, [] {
        qmlRegisterType<RenderScriptingInterface>("RenderEnums", 1, 0, "RenderEnums");
    });
}

void RenderScriptingInterface::loadSettings() {
    _renderSettingLock.withReadLock([&] {
        _renderMethod = (_renderMethodSetting.get());
        _shadowsEnabled = (_shadowsEnabledSetting.get());
        _ambientOcclusionEnabled = (_ambientOcclusionEnabledSetting.get());
        _antialiasingMethod = (_antialiasingMethodSetting.get());
        _viewportResolutionScale = (_viewportResolutionScaleSetting.get());
        _nametagsEnabled = (_nametagsEnabledSetting.get());
    });
    forceRenderMethod((RenderMethod)_renderMethod);
    forceShadowsEnabled(_shadowsEnabled);
    forceAmbientOcclusionEnabled(_ambientOcclusionEnabled);
    forceAntialiasingMethod((AntialiasingMethod)_antialiasingMethod);
    forceViewportResolutionScale(_viewportResolutionScale);
    forceNametagsEnabled(_nametagsEnabled);
}

RenderScriptingInterface::RenderMethod RenderScriptingInterface::getRenderMethod() const {
    return (RenderMethod)_renderMethod;
}

void RenderScriptingInterface::setRenderMethod(RenderMethod renderMethod) {
    if (isValidRenderMethod(renderMethod) && (_renderMethod != (int)renderMethod)) {
        forceRenderMethod(renderMethod);
        emit settingsChanged();
    }
}
void RenderScriptingInterface::forceRenderMethod(RenderMethod renderMethod) {
    _renderSettingLock.withWriteLock([&] {
        _renderMethod = (int)renderMethod;
        _renderMethodSetting.set((int)renderMethod);

        auto config = dynamic_cast<render::SwitchConfig*>(
            qApp->getRenderEngine()->getConfiguration()->getConfig(
                "RenderMainView.DeferredForwardSwitch"
            )
        );
        if (config) config->setBranch((int)renderMethod);

        auto secondaryConfig = dynamic_cast<render::SwitchConfig*>(
            qApp->getRenderEngine()->getConfiguration()->getConfig(
                "RenderSecondView.DeferredForwardSwitch"
            )
        );
        if (secondaryConfig) secondaryConfig->setBranch((int)renderMethod);

    });
}

QStringList RenderScriptingInterface::getRenderMethodNames() const {
    static const QStringList renderMethodNames = { "DEFERRED", "FORWARD" };
    return renderMethodNames;
}

bool RenderScriptingInterface::getShadowsEnabled() const {
    return _shadowsEnabled;
}

void RenderScriptingInterface::setShadowsEnabled(bool enabled) {
    if (_shadowsEnabled != enabled) {
        forceShadowsEnabled(enabled);
        emit settingsChanged();
    }
}

void RenderScriptingInterface::forceShadowsEnabled(bool enabled) {
    _renderSettingLock.withWriteLock([&] {
        _shadowsEnabled = (enabled);
        _shadowsEnabledSetting.set(enabled);

        auto renderConfig = qApp->getRenderEngine()->getConfiguration();
        assert(renderConfig);
        auto lightingModelConfig = renderConfig->getConfig<MakeLightingModel>("RenderMainView.LightingModel");
        if (lightingModelConfig) {
            Menu::getInstance()->setIsOptionChecked(MenuOption::Shadows, enabled);
            lightingModelConfig->setShadow(enabled);
        }
        auto secondaryLightingModelConfig = renderConfig->getConfig<MakeLightingModel>("RenderSecondView.LightingModel");
        if (secondaryLightingModelConfig) {
            Menu::getInstance()->setIsOptionChecked(MenuOption::Shadows, enabled);
            secondaryLightingModelConfig->setShadow(enabled);
        }
    });
}

bool RenderScriptingInterface::getAmbientOcclusionEnabled() const {
    return _ambientOcclusionEnabled;
}

void RenderScriptingInterface::setAmbientOcclusionEnabled(bool enabled) {
    if (_ambientOcclusionEnabled != enabled) {
        forceAmbientOcclusionEnabled(enabled);
        emit settingsChanged();
    }
}

void RenderScriptingInterface::forceAmbientOcclusionEnabled(bool enabled) {
    _renderSettingLock.withWriteLock([&] {
        _ambientOcclusionEnabled = (enabled);
        _ambientOcclusionEnabledSetting.set(enabled);

        auto config = qApp->getRenderEngine()->getConfiguration();

        auto lightingModelConfig = config->getConfig<MakeLightingModel>(
            "RenderMainView.LightingModel"
        );
        if (lightingModelConfig) {
            Menu::getInstance()->setIsOptionChecked(MenuOption::AmbientOcclusion, enabled);
            lightingModelConfig->setAmbientOcclusion(enabled);
        }

        auto secondaryLightingModelConfig = config->getConfig<MakeLightingModel>(
            "RenderSecondView.LightingModel"
        );
        if (secondaryLightingModelConfig) {
            Menu::getInstance()->setIsOptionChecked(MenuOption::AmbientOcclusion, enabled);
            secondaryLightingModelConfig->setAmbientOcclusion(enabled);
        }
    });
}

RenderScriptingInterface::AntialiasingMethod RenderScriptingInterface::getAntialiasingMethod() const {
    return (AntialiasingMethod)_antialiasingMethod;
}

void RenderScriptingInterface::setAntialiasingMethod(AntialiasingMethod antialiasingMethod) {
    if (isValidAntialiasingMethod(antialiasingMethod) && (_antialiasingMethod != (int)antialiasingMethod)) {
        forceAntialiasingMethod(antialiasingMethod);
        emit settingsChanged();
    }
}

void RenderScriptingInterface::forceAntialiasingMethod(AntialiasingMethod antialiasingMethod) {
    _renderSettingLock.withWriteLock([&] {
        _antialiasingMethod = (int)antialiasingMethod;
        _antialiasingMethodSetting.set((int)antialiasingMethod);

        bool useTAA = antialiasingMethod == AntialiasingMethod::TAA;
        bool useFXAA = antialiasingMethod == AntialiasingMethod::FXAA;

        auto instance = qApp->getRenderEngine()->getConfiguration();
        
        auto mainAa = instance->getConfig<Antialiasing>("RenderMainView.Antialiasing");
        auto mainQAa = instance->getConfig("RenderMainView.Antialiasing");
        auto mainJitter = instance->getConfig<JitterSample>("RenderMainView.JitterCam");

        auto secondAa = instance->getConfig<Antialiasing>("RenderSecondView.Antialiasing");
        auto secondQAa = instance->getConfig("RenderSecondView.Antialiasing");
        auto secondJitter = instance->getConfig<JitterSample>("RenderSecondView.JitterCam");

        if (
            mainAa && mainQAa && mainJitter &&
            secondAa && secondQAa && secondJitter
        ) {
            mainAa->setDebugFXAA(useFXAA);
            mainQAa->setProperty("constrainColor", useTAA);
            mainQAa->setProperty("feedbackColor", useTAA);
            mainQAa->setProperty("blend", useTAA ? 0.25f : 1);
            mainQAa->setProperty("sharpen", useTAA ? 0.05f : 0);

            secondAa->setDebugFXAA(useFXAA);
            secondQAa->setProperty("constrainColor", useTAA);
            secondQAa->setProperty("feedbackColor", useTAA);
            secondQAa->setProperty("blend", useTAA ? 0.25f : 1);
            secondQAa->setProperty("sharpen", useTAA ? 0.05f : 0);

            if (useTAA) {
                mainJitter->play();
                secondJitter->play();
            } else {
                mainJitter->none();
                secondJitter->none();
            }
        }
    });
}

QStringList RenderScriptingInterface::getAntialiasingMethodNames() const {
    static const QStringList antialiasingMethodNames = { "NONE", "TAA", "FXAA" };
    return antialiasingMethodNames;
}

float RenderScriptingInterface::getViewportResolutionScale() const {
    return _viewportResolutionScale;
}

void RenderScriptingInterface::setViewportResolutionScale(float scale) {
    if (_viewportResolutionScale != scale) {
        forceViewportResolutionScale(scale);
        emit settingsChanged();
    }
}

void RenderScriptingInterface::forceViewportResolutionScale(float scale) {
    // just not negative values or zero
    if (scale <= 0.f) {
        return;
    }
    _renderSettingLock.withWriteLock([&] {
        _viewportResolutionScale = (scale);
        _viewportResolutionScaleSetting.set(scale);

        auto renderConfig = qApp->getRenderEngine()->getConfiguration();
        assert(renderConfig);
        auto deferredView = renderConfig->getConfig("RenderMainView.RenderDeferredTask");
        // mainView can be null if we're rendering in forward mode
        if (deferredView) {
            deferredView->setProperty("resolutionScale", _viewportResolutionScale);
        }
        auto forwardView = renderConfig->getConfig("RenderMainView.RenderForwardTask");
        // mainView can be null if we're rendering in forward mode
        if (forwardView) {
            forwardView->setProperty("resolutionScale", _viewportResolutionScale);
        }
    });
}

bool RenderScriptingInterface::getNametagsEnabled() const {
    return _nametagsEnabled;
}

void RenderScriptingInterface::setNametagsEnabled(bool enabled) {
    if (_nametagsEnabled != enabled) {
        forceNametagsEnabled(enabled);
        emit settingsChanged();
    }
}

// TODO: Hook this up to a ToneMappingStage
// void RenderScriptingInterface::setToneMappingMode(int curve, float exposure) {
//    // tonemapping curve {0:RGB, 1:SRGB, 2:Reinhard, 3:Filmic} 
//     _renderSettingLock.withWriteLock([&] {
//         auto renderConfig = qApp->getRenderEngine()->getConfiguration();
//         assert(renderConfig);
//         auto deferredView = renderConfig->getConfig("RenderMainView.ToneMapping");  
//           if (deferredView) {
//               deferredView->setProperty("toneMappingMode", curve);
//               deferredView->setProperty("Tonemapping.exposure", exposure);
//           }
//          auto forwardView = renderConfig->getConfig("RenderMainView.ToneMapping");
//           if (forwardView) {
//               forwardView->setProperty("toneMappingMode", curve);
//               forwardView->setProperty("Tonemapping.exposure", exposure);
//           }
//     });
// }

void RenderScriptingInterface::forceNametagsEnabled(bool enabled) {
    _nametagsEnabled = (enabled);
    _nametagsEnabledSetting.set(enabled);

    // put nametags logic here when implementing in cpp
    // code is in javascript right now and uses these functions
}
