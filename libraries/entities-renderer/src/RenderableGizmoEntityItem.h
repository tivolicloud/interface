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


#ifndef hifi_RenderableGizmoEntityItem_h
#define hifi_RenderableGizmoEntityItem_h

#include "RenderableEntityItem.h"

#include <GizmoEntityItem.h>

namespace render { namespace entities {

class GizmoEntityRenderer : public TypedEntityRenderer<GizmoEntityItem> {
    using Parent = TypedEntityRenderer<GizmoEntityItem>;
    using Pointer = std::shared_ptr<GizmoEntityRenderer>;
public:
    GizmoEntityRenderer(const EntityItemPointer& entity);
    ~GizmoEntityRenderer();

protected:
    Item::Bound getBound() override;
    ShapeKey getShapeKey() override;

    bool isTransparent() const override;

private:
    virtual void doRenderUpdateAsynchronousTyped(const TypedEntityPointer& entity) override;
    virtual void doRender(RenderArgs* args) override;

    GizmoType _gizmoType { UNSET_GIZMO_TYPE };
    RingGizmoPropertyGroup _ringProperties;
    PrimitiveMode _prevPrimitiveMode;

    int _ringGeometryID { 0 };
    int _majorTicksGeometryID { 0 };
    int _minorTicksGeometryID { 0 };
    gpu::Primitive _solidPrimitive { gpu::TRIANGLE_FAN };

};

} }
#endif // hifi_RenderableGizmoEntityItem_h
