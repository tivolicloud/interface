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


#ifndef hifi_RenderableGridEntityItem_h
#define hifi_RenderableGridEntityItem_h

#include "RenderableEntityItem.h"

#include <GridEntityItem.h>

namespace render { namespace entities { 

class GridEntityRenderer : public TypedEntityRenderer<GridEntityItem> {
    using Parent = TypedEntityRenderer<GridEntityItem>;
    using Pointer = std::shared_ptr<GridEntityRenderer>;
public:
    GridEntityRenderer(const EntityItemPointer& entity);
    ~GridEntityRenderer();

protected:
    Item::Bound getBound() override;
    ShapeKey getShapeKey() override;

    bool isTransparent() const override;

private:
    virtual void doRenderUpdateSynchronousTyped(const ScenePointer& scene, Transaction& transaction, const TypedEntityPointer& entity) override;
    virtual void doRender(RenderArgs* args) override;

    glm::u8vec3 _color;
    float _alpha;
    PulsePropertyGroup _pulseProperties;

    bool _followCamera;
    uint32_t _majorGridEvery;
    float _minorGridEvery;

    glm::vec3 _dimensions;

    int _geometryId { 0 };

};

} } 
#endif // hifi_RenderableGridEntityItem_h
