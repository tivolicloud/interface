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


#ifndef hifi_RenderableShapeEntityItem_h
#define hifi_RenderableShapeEntityItem_h

#include "RenderableEntityItem.h"

#include <procedural/Procedural.h>
#include <ShapeEntityItem.h>

namespace render { namespace entities { 

class ShapeEntityRenderer : public TypedEntityRenderer<ShapeEntityItem> {
    using Parent = TypedEntityRenderer<ShapeEntityItem>;
    using Pointer = std::shared_ptr<ShapeEntityRenderer>;
public:
    ShapeEntityRenderer(const EntityItemPointer& entity);

    virtual scriptable::ScriptableModelBase getScriptableModel() override;

protected:
    ShapeKey getShapeKey() override;
    Item::Bound getBound() override;
    virtual bool evaluateEntityZoneCullState(const EntityItemPointer& entity) override { return false; }

private:
    virtual bool needsRenderUpdate() const override;
    virtual bool needsRenderUpdateFromTypedEntity(const TypedEntityPointer& entity) const override;
    virtual void doRenderUpdateSynchronousTyped(const ScenePointer& scene, Transaction& transaction, const TypedEntityPointer& entity) override;
    virtual void doRenderUpdateAsynchronousTyped(const TypedEntityPointer& entity) override;
    virtual void doRender(RenderArgs* args) override;
    virtual bool isTransparent() const override;

    bool _doZoneCull{ false };  // TIVOLI Zone Culling

    enum Pipeline { SIMPLE, MATERIAL, PROCEDURAL };
    Pipeline getPipelineType(const graphics::MultiMaterial& materials) const;

    QString _proceduralData;
    entity::Shape _shape { entity::Sphere };

    PulsePropertyGroup _pulseProperties;
    std::shared_ptr<graphics::ProceduralMaterial> _material { std::make_shared<graphics::ProceduralMaterial>() };
    glm::vec3 _color { NAN };
    float _alpha;

    glm::vec3 _position;
    glm::vec3 _dimensions;
    glm::quat _orientation;
};

} } 
#endif // hifi_RenderableShapeEntityItem_h
