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


#ifndef hifi_RenderableLineEntityItem_h
#define hifi_RenderableLineEntityItem_h

#include "RenderableEntityItem.h"
#include <LineEntityItem.h>
#include <GeometryCache.h>


namespace render { namespace entities { 

class LineEntityRenderer : public TypedEntityRenderer<LineEntityItem> {
    using Parent = TypedEntityRenderer<LineEntityItem>;
    friend class EntityRenderer;

public:
    LineEntityRenderer(const EntityItemPointer& entity) : Parent(entity) { }

protected:
    virtual void onRemoveFromSceneTyped(const TypedEntityPointer& entity) override;
    virtual void doRenderUpdateAsynchronousTyped(const TypedEntityPointer& entity) override;
    virtual void doRender(RenderArgs* args) override;

private:
    int _lineVerticesID { GeometryCache::UNKNOWN_ID };
    QVector<glm::vec3> _linePoints;
};

} } // namespace 

#endif // hifi_RenderableLineEntityItem_h
