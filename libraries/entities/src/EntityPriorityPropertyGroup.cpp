//
//  EntityPriorityPropertyGroup.CPP
//  libraries/entities/src
//
//  Created by Caitlyn Meeks on 12/23/2019
//  Copyright 2019, Tivoli Cloud VR
//  Licensed under AGPL-3

#include "EntityPriorityPropertyGroup.h"

#include <OctreePacketData.h>

#include "EntityItemProperties.h"
#include "EntityItemPropertiesMacros.h"

void EntityPriorityPropertyGroup::copyToScriptValue(const EntityPropertyFlags& desiredProperties, QScriptValue& properties, QScriptEngine* engine, bool skipDefaults, EntityItemProperties& defaultEntityProperties) const {
}

void EntityPriorityPropertyGroup::copyFromScriptValue(const QScriptValue& object, bool& _defaultSettings) {
}

void EntityPriorityPropertyGroup::merge(const EntityPriorityPropertyGroup& other) {
}

void EntityPriorityPropertyGroup::debugDump() const {
}

void EntityPriorityPropertyGroup::listChangedProperties(QList<QString>& out) {
}

bool EntityPriorityPropertyGroup::appendToEditPacket(OctreePacketData* packetData,
                                            EntityPropertyFlags& requestedProperties,
                                            EntityPropertyFlags& propertyFlags,
                                            EntityPropertyFlags& propertiesDidntFit,
                                            int& propertyCount, 
                                            OctreeElement::AppendState& appendState) const {
    return true;
}

bool EntityPriorityPropertyGroup::decodeFromEditPacket(EntityPropertyFlags& propertyFlags,
                                                    const unsigned char*& dataAt,
                                                    int& processedBytes) {
    int bytesRead = 0;
    processedBytes += bytesRead;
    return true;
}

void EntityPriorityPropertyGroup::markAllChanged() {
}

EntityPropertyFlags EntityPriorityPropertyGroup::getChangedProperties() const {
    EntityPropertyFlags changedProperties;
    return changedProperties;
}

void EntityPriorityPropertyGroup::getProperties(EntityItemProperties& properties) const {
}

bool EntityPriorityPropertyGroup::setProperties(const EntityItemProperties& properties) {
    bool somethingChanged = false;
    return somethingChanged;
}

EntityPropertyFlags EntityPriorityPropertyGroup::getEntityProperties(EncodeBitstreamParams& params) const {
    EntityPropertyFlags requestedProperties;
    return requestedProperties;
}
    
void EntityPriorityPropertyGroup::appendSubclassData(OctreePacketData* packetData, EncodeBitstreamParams& params,
                                            EntityTreeElementExtraEncodeDataPointer entityTreeElementExtraEncodeData,
                                            EntityPropertyFlags& requestedProperties,
                                            EntityPropertyFlags& propertyFlags,
                                            EntityPropertyFlags& propertiesDidntFit,
                                            int& propertyCount, 
                                            OctreeElement::AppendState& appendState) const {
}

int EntityPriorityPropertyGroup::readEntitySubclassDataFromBuffer(const unsigned char* data, int bytesLeftToRead,
                                                         ReadBitstreamToTreeParams& args,
                                                         EntityPropertyFlags& propertyFlags, bool overwriteLocalData,
                                                         bool& somethingChanged) {
    int bytesRead = 0;
    return bytesRead;
}
