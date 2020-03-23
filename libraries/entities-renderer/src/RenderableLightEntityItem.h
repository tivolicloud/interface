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


#ifndef hifi_RenderableLightEntityItem_h
#define hifi_RenderableLightEntityItem_h

#include "RenderableEntityItem.h"
#include <LightEntityItem.h>
#include <LightPayload.h>

namespace render { namespace entities { 

class LightEntityRenderer final : public TypedEntityRenderer<LightEntityItem> {
    using Parent = TypedEntityRenderer<LightEntityItem>;
    friend class EntityRenderer;

public:
    LightEntityRenderer(const EntityItemPointer& entity) : Parent(entity) { }

protected:
    virtual void doRenderUpdateAsynchronousTyped(const TypedEntityPointer& entity) override;

    virtual ItemKey getKey() override;
    virtual Item::Bound getBound() override;
    virtual void doRender(RenderArgs* args) override;

private:
    const LightPayload::Pointer _lightPayload{ std::make_shared<LightPayload>() };
};

} } // namespace 

#endif // hifi_RenderableLightEntityItem_h
