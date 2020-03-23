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


#ifndef hifi_GizmoEntityItem_h
#define hifi_GizmoEntityItem_h

#include "EntityItem.h"

#include "RingGizmoPropertyGroup.h"

class GizmoEntityItem : public EntityItem {
    using Pointer = std::shared_ptr<GizmoEntityItem>;
public:
    static EntityItemPointer factory(const EntityItemID& entityID, const EntityItemProperties& properties);

    GizmoEntityItem(const EntityItemID& entityItemID);

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

    bool supportsDetailedIntersection() const override;
    bool findDetailedRayIntersection(const glm::vec3& origin, const glm::vec3& direction,
        OctreeElementPointer& element, float& distance,
        BoxFace& face, glm::vec3& surfaceNormal,
        QVariantMap& extraInfo, bool precisionPicking) const override;
    bool findDetailedParabolaIntersection(const glm::vec3& origin, const glm::vec3& velocity,
        const glm::vec3& acceleration, OctreeElementPointer& element, float& parabolicDistance,
        BoxFace& face, glm::vec3& surfaceNormal,
        QVariantMap& extraInfo, bool precisionPicking) const override;

    GizmoType getGizmoType() const;
    void setGizmoType(GizmoType value);

    RingGizmoPropertyGroup getRingProperties() const;

protected:
    GizmoType _gizmoType;
    RingGizmoPropertyGroup _ringProperties;

};

#endif // hifi_GizmoEntityItem_h
