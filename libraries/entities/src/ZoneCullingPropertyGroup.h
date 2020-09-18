//
//  ZoneCullingPropertyGroup.h
//  libraries/entities/src
//
//  Created by Caitlyn Meeks on 12/23/2019
//  Copyright 2019, Tivoli Cloud VR


#ifndef hifi_ZoneCullingPropertyGroup_h
#define hifi_ZoneCullingPropertyGroup_h


#include <stdint.h>
#include <glm/glm.hpp>

#include <QtScript/QScriptEngine>

#include "PropertyGroup.h"
#include "EntityItemPropertiesMacros.h"

class EntityItemProperties;
class EncodeBitstreamParams;
class OctreePacketData;
class EntityTreeElementExtraEncodeData;
class ReadBitstreamToTreeParams;

/**jsdoc
 * Zone Culling is defined by the following properties:
 * @typedef {object} Entities.ZoneCulling
 */
class ZoneCullingPropertyGroup : public PropertyGroup {
public:
    // EntityItemProperty related helpers
    virtual void copyToScriptValue(const EntityPropertyFlags& desiredProperties,
                                   QScriptValue& properties,
                                   QScriptEngine* engine,
                                   bool skipDefaults,
                                   EntityItemProperties& defaultEntityProperties) const override;
    virtual void copyFromScriptValue(const QScriptValue& object, bool& _defaultSettings) override;

    void merge(const ZoneCullingPropertyGroup& other);

    virtual void debugDump() const override;
    virtual void listChangedProperties(QList<QString>& out) override;

    virtual bool appendToEditPacket(OctreePacketData* packetData,
                                    EntityPropertyFlags& requestedProperties,
                                    EntityPropertyFlags& propertyFlags,
                                    EntityPropertyFlags& propertiesDidntFit,
                                    int& propertyCount,
                                    OctreeElement::AppendState& appendState) const override;

    virtual bool decodeFromEditPacket(EntityPropertyFlags& propertyFlags,
                                      const unsigned char*& dataAt,
                                      int& processedBytes) override;
    virtual void markAllChanged() override;
    virtual EntityPropertyFlags getChangedProperties() const override;

    // EntityItem related helpers
    // methods for getting/setting all properties of an entity
    virtual void getProperties(EntityItemProperties& propertiesOut) const override;

    /// returns true if something changed
    virtual bool setProperties(const EntityItemProperties& properties) override;

    virtual EntityPropertyFlags getEntityProperties(EncodeBitstreamParams& params) const override;

    virtual void appendSubclassData(OctreePacketData* packetData,
                                    EncodeBitstreamParams& params,
                                    EntityTreeElementExtraEncodeDataPointer entityTreeElementExtraEncodeData,
                                    EntityPropertyFlags& requestedProperties,
                                    EntityPropertyFlags& propertyFlags,
                                    EntityPropertyFlags& propertiesDidntFit,
                                    int& propertyCount,
                                    OctreeElement::AppendState& appendState) const override;

    virtual int readEntitySubclassDataFromBuffer(const unsigned char* data,
                                                 int bytesLeftToRead,
                                                 ReadBitstreamToTreeParams& args,
                                                 EntityPropertyFlags& propertyFlags,
                                                 bool overwriteLocalData,
                                                 bool& somethingChanged) override;

};

#endif  // hifi_ZoneCullingPropertyGroup_h