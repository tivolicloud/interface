////
//  ZoneCullingPropertyGroup.CPP
//  libraries/entities/src
//
//  Created by Caitlyn Meeks on 12/23/2019
//  Copyright 2019, Tivoli Cloud VR

#include "ZoneCullingPropertyGroup.h"

#include <OctreePacketData.h>

#include "EntityItemProperties.h"
#include "EntityItemPropertiesMacros.h"

void ZoneCullingPropertyGroup::copyToScriptValue(const EntityPropertyFlags& desiredProperties, QScriptValue& properties, QScriptEngine* engine, bool skipDefaults, EntityItemProperties& defaultEntityProperties) const {
}

void ZoneCullingPropertyGroup::copyFromScriptValue(const QScriptValue& object, bool& _defaultSettings) {
}

void ZoneCullingPropertyGroup::merge(const ZoneCullingPropertyGroup& other) {
}

void ZoneCullingPropertyGroup::debugDump() const {
}

void ZoneCullingPropertyGroup::listChangedProperties(QList<QString>& out) {
}

bool ZoneCullingPropertyGroup::appendToEditPacket(OctreePacketData* packetData,
                                            EntityPropertyFlags& requestedProperties,
                                            EntityPropertyFlags& propertyFlags,
                                            EntityPropertyFlags& propertiesDidntFit,
                                            int& propertyCount, 
                                            OctreeElement::AppendState& appendState) const {
    return true;
}

bool ZoneCullingPropertyGroup::decodeFromEditPacket(EntityPropertyFlags& propertyFlags,
                                                    const unsigned char*& dataAt,
                                                    int& processedBytes) {
    int bytesRead = 0;
    processedBytes += bytesRead;
    return true;
}

void ZoneCullingPropertyGroup::markAllChanged() {
}

EntityPropertyFlags ZoneCullingPropertyGroup::getChangedProperties() const {
    EntityPropertyFlags changedProperties;
    return changedProperties;
}

void ZoneCullingPropertyGroup::getProperties(EntityItemProperties& properties) const {
}

bool ZoneCullingPropertyGroup::setProperties(const EntityItemProperties& properties) {
    bool somethingChanged = false;
    return somethingChanged;
}

EntityPropertyFlags ZoneCullingPropertyGroup::getEntityProperties(EncodeBitstreamParams& params) const {
    EntityPropertyFlags requestedProperties;
    return requestedProperties;
}
    
void ZoneCullingPropertyGroup::appendSubclassData(OctreePacketData* packetData, EncodeBitstreamParams& params,
                                            EntityTreeElementExtraEncodeDataPointer entityTreeElementExtraEncodeData,
                                            EntityPropertyFlags& requestedProperties,
                                            EntityPropertyFlags& propertyFlags,
                                            EntityPropertyFlags& propertiesDidntFit,
                                            int& propertyCount, 
                                            OctreeElement::AppendState& appendState) const {
}

int ZoneCullingPropertyGroup::readEntitySubclassDataFromBuffer(const unsigned char* data, int bytesLeftToRead,
                                                         ReadBitstreamToTreeParams& args,
                                                         EntityPropertyFlags& propertyFlags, bool overwriteLocalData,
                                                         bool& somethingChanged) {
    int bytesRead = 0;
    return bytesRead;
}
