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



#ifndef hifi_RenderableTextEntityItem_h
#define hifi_RenderableTextEntityItem_h

#include "RenderableEntityItem.h"

class TextEntityItem;
class TextRenderer3D;

namespace render { namespace entities {

class TextPayload;

class TextEntityRenderer : public TypedEntityRenderer<TextEntityItem> {
    using Parent = TypedEntityRenderer<TextEntityItem>;
    using Pointer = std::shared_ptr<TextEntityRenderer>;
public:
    TextEntityRenderer(const EntityItemPointer& entity);
    ~TextEntityRenderer();

    QSizeF textSize(const QString& text) const;

protected:
    bool isTransparent() const override;
    bool isTextTransparent() const;
    Item::Bound getBound() override;
    ShapeKey getShapeKey() override;
    ItemKey getKey() override;
    virtual uint32_t metaFetchMetaSubItems(ItemIDs& subItems) const override;

    void onAddToSceneTyped(const TypedEntityPointer& entity) override;
    void onRemoveFromSceneTyped(const TypedEntityPointer& entity) override;

private:
    virtual bool needsRenderUpdateFromTypedEntity(const TypedEntityPointer& entity) const override;
    virtual void doRenderUpdateSynchronousTyped(const ScenePointer& scene, Transaction& transaction, const TypedEntityPointer& entity) override;
    virtual void doRenderUpdateAsynchronousTyped(const TypedEntityPointer& entity) override;
    virtual void doRender(RenderArgs* args) override;

    std::shared_ptr<TextRenderer3D> _textRenderer;

    PulsePropertyGroup _pulseProperties;

    QString _text;
    float _lineHeight;
    glm::vec3 _textColor;
    float _textAlpha;
    glm::vec3 _backgroundColor;
    float _backgroundAlpha;
    bool _unlit;

    float _leftMargin;
    float _rightMargin;
    float _topMargin;
    float _bottomMargin;

    BillboardMode _billboardMode;
    glm::vec3 _dimensions;

    QString _font { "" };
    TextEffect _effect { TextEffect::NO_EFFECT };
    glm::vec3 _effectColor { 0 };
    float _effectThickness { 0.0f };

    int _geometryID { 0 };

    std::shared_ptr<TextPayload> _textPayload;
    render::ItemID _textRenderID;
    void updateTextRenderItem() const;

    friend class render::entities::TextPayload;
};

class TextPayload {
public:
    TextPayload() = default;
    TextPayload(const QUuid& entityID, const std::weak_ptr<TextRenderer3D>& textRenderer);
    ~TextPayload();

    typedef render::Payload<TextPayload> Payload;
    typedef Payload::DataPointer Pointer;

    ItemKey getKey() const;
    Item::Bound getBound() const;
    ShapeKey getShapeKey() const;
    void render(RenderArgs* args);

protected:
    QUuid _entityID;
    std::weak_ptr<TextRenderer3D> _textRenderer;

    int _geometryID { 0 };
};

} }

namespace render {
    template <> const ItemKey payloadGetKey(const entities::TextPayload::Pointer& payload);
    template <> const Item::Bound payloadGetBound(const entities::TextPayload::Pointer& payload);
    template <> const ShapeKey shapeGetShapeKey(const entities::TextPayload::Pointer& payload);
    template <> void payloadRender(const entities::TextPayload::Pointer& payload, RenderArgs* args);
}

#endif // hifi_RenderableTextEntityItem_h
