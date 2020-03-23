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


#ifndef hifi_TextEntityItem_h
#define hifi_TextEntityItem_h

#include "EntityItem.h"

#include "PulsePropertyGroup.h"
#include "TextEffect.h"

class TextEntityItem : public EntityItem {
public:
    static EntityItemPointer factory(const EntityItemID& entityID, const EntityItemProperties& properties);

    TextEntityItem(const EntityItemID& entityItemID);

    ALLOW_INSTANTIATION // This class can be instantiated

    /// set dimensions in domain scale units (0.0 - 1.0) this will also reset radius appropriately
    virtual void setUnscaledDimensions(const glm::vec3& value) override;
    virtual ShapeType getShapeType() const override { return SHAPE_TYPE_BOX; }

    // methods for getting/setting all properties of an entity
    virtual EntityItemProperties getProperties(const EntityPropertyFlags& desiredProperties, bool allowEmptyDesiredProperties) const override;
    virtual bool setProperties(const EntityItemProperties& properties) override;

    virtual EntityPropertyFlags getEntityProperties(EncodeBitstreamParams& params) const override;

    virtual void appendSubclassData(OctreePacketData* packetData, EncodeBitstreamParams& params,
                                    EntityTreeElementExtraEncodeDataPointer entityTreeElementExtraEncodeData,
                                    EntityPropertyFlags& requestedProperties,
                                    EntityPropertyFlags& propertyFlags,
                                    EntityPropertyFlags& propertiesDidntFit,
                                    int& propertyCount,
                                    OctreeElement::AppendState& appendState) const override;

    virtual int readEntitySubclassDataFromBuffer(const unsigned char* data, int bytesLeftToRead,
                                                ReadBitstreamToTreeParams& args,
                                                EntityPropertyFlags& propertyFlags, bool overwriteLocalData,
                                                bool& somethingChanged) override;

    glm::vec3 getRaycastDimensions() const override;
    virtual bool supportsDetailedIntersection() const override { return true; }
    virtual bool findDetailedRayIntersection(const glm::vec3& origin, const glm::vec3& direction,
                         OctreeElementPointer& element, float& distance,
                         BoxFace& face, glm::vec3& surfaceNormal,
                         QVariantMap& extraInfo, bool precisionPicking) const override;
    virtual bool findDetailedParabolaIntersection(const glm::vec3& origin, const glm::vec3& velocity,
                         const glm::vec3& acceleration, OctreeElementPointer& element, float& parabolicDistance,
                         BoxFace& face, glm::vec3& surfaceNormal,
                         QVariantMap& extraInfo, bool precisionPicking) const override;

    static const QString DEFAULT_TEXT;
    void setText(const QString& value);
    QString getText() const;

    static const float DEFAULT_LINE_HEIGHT;
    void setLineHeight(float value);
    float getLineHeight() const;

    static const glm::u8vec3 DEFAULT_TEXT_COLOR;
    glm::u8vec3 getTextColor() const;
    void setTextColor(const glm::u8vec3& value);

    static const float DEFAULT_TEXT_ALPHA;
    float getTextAlpha() const;
    void setTextAlpha(float value);

    static const glm::u8vec3 DEFAULT_BACKGROUND_COLOR;
    glm::u8vec3 getBackgroundColor() const;
    void setBackgroundColor(const glm::u8vec3& value);

    float getBackgroundAlpha() const;
    void setBackgroundAlpha(float value);

    BillboardMode getBillboardMode() const;
    void setBillboardMode(BillboardMode value);

    static const float DEFAULT_MARGIN;
    float getLeftMargin() const;
    void setLeftMargin(float value);

    float getRightMargin() const;
    void setRightMargin(float value);

    float getTopMargin() const;
    void setTopMargin(float value);

    float getBottomMargin() const;
    void setBottomMargin(float value);

    bool getUnlit() const;
    void setUnlit(bool value);

    void setFont(const QString& value);
    QString getFont() const;

    TextEffect getTextEffect() const;
    void setTextEffect(TextEffect value);

    glm::u8vec3 getTextEffectColor() const;
    void setTextEffectColor(const glm::u8vec3& value);

    static const float DEFAULT_TEXT_EFFECT_THICKNESS;
    float getTextEffectThickness() const;
    void setTextEffectThickness(float value);

    PulsePropertyGroup getPulseProperties() const;

private:
    BillboardMode _billboardMode;

    QString _text;
    float _lineHeight;
    glm::u8vec3 _textColor;
    float _textAlpha;
    glm::u8vec3 _backgroundColor;
    float _backgroundAlpha;
    PulsePropertyGroup _pulseProperties;
    float _leftMargin;
    float _rightMargin;
    float _topMargin;
    float _bottomMargin;
    bool _unlit;

    QString _font;
    TextEffect _effect;
    glm::u8vec3 _effectColor;
    float _effectThickness;
};

#endif // hifi_TextEntityItem_h
