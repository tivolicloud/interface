//  Tivoli Cloud VR
//  Copyright (C) 2020, Tivoli Cloud VR, Inc
//  
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Affero General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Affero General Public License for more details.
//  
//  You should have received a copy of the GNU Affero General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//  


#include "RenderableLightEntityItem.h"

#include <GLMHelpers.h>
#include <PerfStat.h>

using namespace render;
using namespace render::entities;

void LightEntityRenderer::doRenderUpdateAsynchronousTyped(const TypedEntityPointer& entity) {

    auto& lightPayload = *_lightPayload;
   
    lightPayload.setVisible(_visible);

    auto light = lightPayload.editLight();
    light->setPosition(entity->getWorldPosition());
    light->setOrientation(entity->getWorldOrientation());

    bool success;
    lightPayload.editBound() = entity->getAABox(success);
    if (!success) {
        lightPayload.editBound() = render::Item::Bound();
    }

    glm::vec3 dimensions = entity->getScaledDimensions();
    float largestDiameter = glm::compMax(dimensions);
    light->setMaximumRadius(largestDiameter / 2.0f);

    light->setColor(toGlm(entity->getColor()));

    float intensity = entity->getIntensity();//* entity->getFadingRatio();
    light->setIntensity(intensity);

    light->setFalloffRadius(entity->getFalloffRadius());


    float exponent = entity->getExponent();
    float cutoff = glm::radians(entity->getCutoff());
    if (!entity->getIsSpotlight()) {
        light->setType(graphics::Light::POINT);
    } else {
        light->setType(graphics::Light::SPOT);

        light->setSpotAngle(cutoff);
        light->setSpotExponent(exponent);
    }
}

ItemKey LightEntityRenderer::getKey() {
    return payloadGetKey(_lightPayload);
}

Item::Bound LightEntityRenderer::getBound() {
    return payloadGetBound(_lightPayload);
}

void LightEntityRenderer::doRender(RenderArgs* args) {
    const bool hasChanged = evaluateEntityZoneCullState(_entity);
    _lightPayload->render(args);
}

