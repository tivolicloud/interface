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


#include "RenderableLineEntityItem.h"

#include <gpu/Batch.h>
#include <PerfStat.h>

using namespace render;
using namespace render::entities;

void LineEntityRenderer::onRemoveFromSceneTyped(const TypedEntityPointer& entity) {
    if (_lineVerticesID != GeometryCache::UNKNOWN_ID) {
        auto geometryCache = DependencyManager::get<GeometryCache>();
        if (geometryCache) {
            geometryCache->releaseID(_lineVerticesID);
        }
    }
}

void LineEntityRenderer::doRenderUpdateAsynchronousTyped(const TypedEntityPointer& entity) {
    _linePoints = entity->getLinePoints();
    auto geometryCache = DependencyManager::get<GeometryCache>();
    if (_lineVerticesID == GeometryCache::UNKNOWN_ID) {
        _lineVerticesID = geometryCache->allocateID();
    }
    glm::vec4 lineColor(toGlm(entity->getColor()), 1.0f);
    geometryCache->updateVertices(_lineVerticesID, _linePoints, lineColor);
}

void LineEntityRenderer::doRender(RenderArgs* args) {

    const bool hasChanged = evaluateEntityZoneCullState(_entity);
    if (_lineVerticesID == GeometryCache::UNKNOWN_ID) {
        return;
    }

    PerformanceTimer perfTimer("RenderableLineEntityItem::render");
    Q_ASSERT(args->_batch);
    gpu::Batch& batch = *args->_batch;
    const auto& modelTransform = getModelTransform();
    Transform transform = Transform();
    transform.setTranslation(modelTransform.getTranslation());
    transform.setRotation(modelTransform.getRotation());
    batch.setModelTransform(transform);
    if (_linePoints.size() > 1) {
        DependencyManager::get<GeometryCache>()->bindSimpleProgram(batch, false, false, false, false, true,
            _renderLayer != RenderLayer::WORLD || args->_renderMethod == Args::RenderMethod::FORWARD);
        DependencyManager::get<GeometryCache>()->renderVertices(batch, gpu::LINE_STRIP, _lineVerticesID);
    }
}
