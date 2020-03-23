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


#ifndef hifi_RenderablePolyLineEntityItem_h
#define hifi_RenderablePolyLineEntityItem_h

#include "RenderableEntityItem.h"
#include <PolyLineEntityItem.h>
#include <TextureCache.h>

namespace render { namespace entities {

class PolyLineEntityRenderer : public TypedEntityRenderer<PolyLineEntityItem> {
    using Parent = TypedEntityRenderer<PolyLineEntityItem>;
    friend class EntityRenderer;

public:
    PolyLineEntityRenderer(const EntityItemPointer& entity);

    void updateModelTransformAndBound() override;

    virtual bool isTransparent() const override;

protected:
    virtual bool needsRenderUpdate() const override;
    virtual bool needsRenderUpdateFromTypedEntity(const TypedEntityPointer& entity) const override;
    virtual void doRenderUpdateSynchronousTyped(const ScenePointer& scene, Transaction& transaction, const TypedEntityPointer& entity) override;

    virtual ItemKey getKey() override;
    virtual ShapeKey getShapeKey() override;
    virtual void doRender(RenderArgs* args) override;

    static void buildPipelines();
    void updateGeometry();
    void updateData();

    QVector<glm::vec3> _points;
    QVector<glm::vec3> _normals;
    QVector<glm::vec3> _colors;
    glm::vec3 _color;
    QVector<float> _widths;

    NetworkTexturePointer _texture;
    float _textureAspectRatio { 1.0f };
    bool _textureLoaded { false };

    bool _isUVModeStretch { false };
    bool _faceCamera { false };
    bool _glow { false };

    size_t _numVertices { 0 };
    gpu::BufferPointer _polylineDataBuffer;
    gpu::BufferPointer _polylineGeometryBuffer;
    static std::map<std::pair<render::Args::RenderMethod, bool>, gpu::PipelinePointer> _pipelines;
};

} } // namespace 

#endif // hifi_RenderablePolyLineEntityItem_h
