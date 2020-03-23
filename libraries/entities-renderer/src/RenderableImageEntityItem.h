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


#ifndef hifi_RenderableImageEntityItem_h
#define hifi_RenderableImageEntityItem_h

#include "RenderableEntityItem.h"

#include <ImageEntityItem.h>

namespace render { namespace entities {

class ImageEntityRenderer : public TypedEntityRenderer<ImageEntityItem> {
    using Parent = TypedEntityRenderer<ImageEntityItem>;
    using Pointer = std::shared_ptr<ImageEntityRenderer>;
public:
    ImageEntityRenderer(const EntityItemPointer& entity);
    ~ImageEntityRenderer();

protected:
    Item::Bound getBound() override;
    ShapeKey getShapeKey() override;

    bool isTransparent() const override;

private:
    virtual bool needsRenderUpdate() const override;
    virtual void doRenderUpdateSynchronousTyped(const ScenePointer& scene, Transaction& transaction, const TypedEntityPointer& entity) override;
    virtual void doRender(RenderArgs* args) override;

    QString _imageURL;
    NetworkTexturePointer _texture;
    bool _textureIsLoaded { false };

    bool _emissive;
    bool _keepAspectRatio;
    QRect _subImage;

    glm::u8vec3 _color;
    float _alpha;
    PulsePropertyGroup _pulseProperties;
    BillboardMode _billboardMode;

    int _geometryId { 0 };
};

} }
#endif // hifi_RenderableImageEntityItem_h
