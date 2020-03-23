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


#ifndef hifi_GridEntityItem_h
#define hifi_GridEntityItem_h

#include "EntityItem.h"

#include "PulsePropertyGroup.h"

class GridEntityItem : public EntityItem {
    using Pointer = std::shared_ptr<GridEntityItem>;
public:
    static EntityItemPointer factory(const EntityItemID& entityID, const EntityItemProperties& properties);

    GridEntityItem(const EntityItemID& entityItemID);

    ALLOW_INSTANTIATION // This class can be instantiated

    virtual void setUnscaledDimensions(const glm::vec3& value) override;

    // methods for getting/setting all properties of an entity
    EntityItemProperties getProperties(const EntityPropertyFlags& desiredProperties, bool allowEmptyDesiredProperties) const override;
    bool setProperties(const EntityItemProperties& properties) override;

    EntityPropertyFlags getEntityProperties(EncodeBitstreamParams& params) const override;

    void appendSubclassData(OctreePacketData* packetData, EncodeBitstreamParams& params,
                            EntityTreeElementExtraEncodeDataPointer entityTreeElementExtraEncodeData,
                            EntityPropertyFlags& requestedProperties,
                            EntityPropertyFlags& propertyFlags,
                            EntityPropertyFlags& propertiesDidntFit,
                            int& propertyCount,
                            OctreeElement::AppendState& appendState) const override;

    int readEntitySubclassDataFromBuffer(const unsigned char* data, int bytesLeftToRead,
                                         ReadBitstreamToTreeParams& args,
                                         EntityPropertyFlags& propertyFlags, bool overwriteLocalData,
                                         bool& somethingChanged) override;

    static const uint32_t DEFAULT_MAJOR_GRID_EVERY;
    static const float DEFAULT_MINOR_GRID_EVERY;

    void setColor(const glm::u8vec3& color);
    glm::u8vec3 getColor() const;

    void setAlpha(float alpha);
    float getAlpha() const;

    void setFollowCamera(bool followCamera);
    bool getFollowCamera() const;

    void setMajorGridEvery(uint32_t majorGridEvery);
    uint32_t getMajorGridEvery() const;

    void setMinorGridEvery(float minorGridEvery);
    float getMinorGridEvery() const;

    PulsePropertyGroup getPulseProperties() const;

protected:
    glm::u8vec3 _color;
    float _alpha;
    PulsePropertyGroup _pulseProperties;

    bool _followCamera { true };
    uint32_t _majorGridEvery { DEFAULT_MAJOR_GRID_EVERY };
    float _minorGridEvery { DEFAULT_MINOR_GRID_EVERY };

};

#endif // hifi_GridEntityItem_h
