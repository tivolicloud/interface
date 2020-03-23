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

#ifndef hifi_KeyLightPropertyGroup_h
#define hifi_KeyLightPropertyGroup_h

#include <stdint.h>

#include <glm/glm.hpp>

#include <QtScript/QScriptEngine>
#include "EntityItemPropertiesMacros.h"
#include "PropertyGroup.h"

class EntityItemProperties;
class EncodeBitstreamParams;
class OctreePacketData;
class EntityTreeElementExtraEncodeData;
class ReadBitstreamToTreeParams;

/**jsdoc
 * A key light is defined by the following properties:
 * @typedef {object} Entities.KeyLight
 * @property {Color} color=255,255,255 - The color of the light.
 * @property {number} intensity=1 - The intensity of the light.
 * @property {Vec3} direction=0,-1,0 - The direction the light is shining.
 * @property {boolean} castShadows=false - <code>true</code> if shadows are cast, <code>false</code> if they aren't. Shadows 
 *     are cast by avatars, plus {@link Entities.EntityProperties-Model|Model} and 
 *     {@link Entities.EntityProperties-Shape|Shape} entities that have their 
 *     <code>{@link Entities.EntityProperties|canCastShadow}</code> property set to <code>true</code>.
 * @property {number} shadowBias=0.5 - The bias of the shadows cast by the light, range <code>0.0</code> &ndash; 
 *     <code>1.0</code>. This fine-tunes shadows cast by the light, to prevent shadow acne and peter panning.
 * @property {number} shadowMaxDistance=40.0 - The maximum distance from the camera position at which shadows will be computed, 
 *     range <code>1.0</code> &ndash; <code>250.0</code>. Higher values cover more of the scene but with less precision.
 */
class KeyLightPropertyGroup : public PropertyGroup {
public:
    // EntityItemProperty related helpers
    virtual void copyToScriptValue(const EntityPropertyFlags& desiredProperties, QScriptValue& properties,
                                   QScriptEngine* engine, bool skipDefaults,
                                   EntityItemProperties& defaultEntityProperties) const override;
    virtual void copyFromScriptValue(const QScriptValue& object, bool& _defaultSettings) override;

    void merge(const KeyLightPropertyGroup& other);

    virtual void debugDump() const override;
    virtual void listChangedProperties(QList<QString>& out) override;

    virtual bool appendToEditPacket(OctreePacketData* packetData,
                                    EntityPropertyFlags& requestedProperties,
                                    EntityPropertyFlags& propertyFlags,
                                    EntityPropertyFlags& propertiesDidntFit,
                                    int& propertyCount,
                                    OctreeElement::AppendState& appendState) const override;

    virtual bool decodeFromEditPacket(EntityPropertyFlags& propertyFlags,
                                      const unsigned char*& dataAt, int& processedBytes) override;
    virtual void markAllChanged() override;
    virtual EntityPropertyFlags getChangedProperties() const override;

    // EntityItem related helpers
    // methods for getting/setting all properties of an entity
    virtual void getProperties(EntityItemProperties& propertiesOut) const override;

    /// returns true if something changed
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

    static const glm::u8vec3 DEFAULT_KEYLIGHT_COLOR;
    static const float DEFAULT_KEYLIGHT_INTENSITY;
    static const float DEFAULT_KEYLIGHT_AMBIENT_INTENSITY;
    static const glm::vec3 DEFAULT_KEYLIGHT_DIRECTION;
    static const bool DEFAULT_KEYLIGHT_CAST_SHADOWS;
    static const float DEFAULT_KEYLIGHT_SHADOW_BIAS;
    static const float DEFAULT_KEYLIGHT_SHADOW_MAX_DISTANCE;

    DEFINE_PROPERTY_REF(PROP_KEYLIGHT_COLOR, Color, color, glm::u8vec3, DEFAULT_KEYLIGHT_COLOR);
    DEFINE_PROPERTY(PROP_KEYLIGHT_INTENSITY, Intensity, intensity, float, DEFAULT_KEYLIGHT_INTENSITY);
    DEFINE_PROPERTY_REF(PROP_KEYLIGHT_DIRECTION, Direction, direction, glm::vec3, DEFAULT_KEYLIGHT_DIRECTION);
    DEFINE_PROPERTY(PROP_KEYLIGHT_CAST_SHADOW, CastShadows, castShadows, bool, DEFAULT_KEYLIGHT_CAST_SHADOWS);
    DEFINE_PROPERTY(PROP_KEYLIGHT_SHADOW_BIAS, ShadowBias, shadowBias, float, DEFAULT_KEYLIGHT_SHADOW_BIAS);
    DEFINE_PROPERTY(PROP_KEYLIGHT_SHADOW_MAX_DISTANCE, ShadowMaxDistance, shadowMaxDistance, float, DEFAULT_KEYLIGHT_SHADOW_MAX_DISTANCE);
};

#endif // hifi_KeyLightPropertyGroup_h
