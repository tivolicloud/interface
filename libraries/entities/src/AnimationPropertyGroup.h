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



#ifndef hifi_AnimationPropertyGroup_h
#define hifi_AnimationPropertyGroup_h

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

class AnimationPropertyGroup : public PropertyGroup {
public:
    static const float MAXIMUM_POSSIBLE_FRAME;

    // EntityItemProperty related helpers
    virtual void copyToScriptValue(const EntityPropertyFlags& desiredProperties, QScriptValue& properties,
                                   QScriptEngine* engine, bool skipDefaults,
                                   EntityItemProperties& defaultEntityProperties) const override;
    virtual void copyFromScriptValue(const QScriptValue& object, bool& _defaultSettings) override;

    void merge(const AnimationPropertyGroup& other);

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

    float getNumFrames() const;
    float computeLoopedFrame(float frame) const;
    bool isValidAndRunning() const;

    DEFINE_PROPERTY_REF(PROP_ANIMATION_URL, URL, url, QString, "");
    DEFINE_PROPERTY(PROP_ANIMATION_FPS, FPS, fps, float, 30.0f);
    DEFINE_PROPERTY(PROP_ANIMATION_FRAME_INDEX, CurrentFrame, currentFrame, float, 0.0f);
    DEFINE_PROPERTY(PROP_ANIMATION_PLAYING, Running, running, bool, false); // was animationIsPlaying
    DEFINE_PROPERTY(PROP_ANIMATION_LOOP, Loop, loop, bool, true); // was animationSettings.loop
    DEFINE_PROPERTY(PROP_ANIMATION_FIRST_FRAME, FirstFrame, firstFrame, float, 0.0f); // was animationSettings.firstFrame
    DEFINE_PROPERTY(PROP_ANIMATION_LAST_FRAME, LastFrame, lastFrame, float, MAXIMUM_POSSIBLE_FRAME); // was animationSettings.lastFrame
    DEFINE_PROPERTY(PROP_ANIMATION_HOLD, Hold, hold, bool, false); // was animationSettings.hold
    DEFINE_PROPERTY(PROP_ANIMATION_ALLOW_TRANSLATION, AllowTranslation, allowTranslation, bool, true); 

protected:
    friend bool operator==(const AnimationPropertyGroup& a, const AnimationPropertyGroup& b);
    friend bool operator!=(const AnimationPropertyGroup& a, const AnimationPropertyGroup& b);
    void setFromOldAnimationSettings(const QString& value);
};

#endif // hifi_AnimationPropertyGroup_h
