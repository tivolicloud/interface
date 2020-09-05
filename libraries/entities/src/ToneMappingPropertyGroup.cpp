//
//  ToneMappingPropertyGroup.h
//  libraries/entities/src
//
//  Created by Caitlyn Meeks on 8/28/2020
//  Copyright 2020 Tivoli Cloud VR, Inc.
//
//  Licensed under AGPL-3


#include "ToneMappingPropertyGroup.h"

#include <OctreePacketData.h>

#include "EntityItemProperties.h"
#include "EntityItemPropertiesMacros.h"

void ToneMappingPropertyGroup::copyToScriptValue(const EntityPropertyFlags& desiredProperties, QScriptValue& properties, QScriptEngine* engine, bool skipDefaults, EntityItemProperties& defaultEntityProperties) const {
    COPY_GROUP_PROPERTY_TO_QSCRIPTVALUE(PROP_TONE_MAPPING_EXPOSURE, ToneMapping, tonemapping, Exposure, exposure);
}

void ToneMappingPropertyGroup::copyFromScriptValue(const QScriptValue& object, bool& _defaultSettings) {
    COPY_GROUP_PROPERTY_FROM_QSCRIPTVALUE(toneMapping, exposure, float, setExposure);
}

void ToneMappingPropertyGroup::merge(const ToneMappingPropertyGroup& other) {
    COPY_PROPERTY_IF_CHANGED(exposure);
}

void ToneMappingPropertyGroup::debugDump() const {
    qCDebug(entities) << "   ToneMappingPropertyGroup: ---------------------------------------------";
    qCDebug(entities) << "      _exposure:" << _exposure;
}

void ToneMappingPropertyGroup::listChangedProperties(QList<QString>& out) {
    if (exposureChanged()) {
        out << "tonemapping-exposure";
    }
}

bool ToneMappingPropertyGroup::appendToEditPacket(OctreePacketData* packetData,
                                            EntityPropertyFlags& requestedProperties,
                                            EntityPropertyFlags& propertyFlags,
                                            EntityPropertyFlags& propertiesDidntFit,
                                            int& propertyCount, 
                                            OctreeElement::AppendState& appendState) const {
    bool successPropertyFits = true;
    APPEND_ENTITY_PROPERTY(PROP_TONE_MAPPING_EXPOSURE, getExposure());
    return true;
}

bool ToneMappingPropertyGroup::decodeFromEditPacket(EntityPropertyFlags& propertyFlags, const unsigned char*& dataAt , int& processedBytes) {
    int bytesRead = 0;
    bool overwriteLocalData = true;
    bool somethingChanged = false;
    READ_ENTITY_PROPERTY(PROP_TONE_MAPPING_EXPOSURE, float, setExposure);
    DECODE_GROUP_PROPERTY_HAS_CHANGED(PROP_TONE_MAPPING_EXPOSURE, Exposure);
    processedBytes += bytesRead;
    Q_UNUSED(somethingChanged);
    return true;
}

void ToneMappingPropertyGroup::markAllChanged() {
    _exposureChanged = true;
}

EntityPropertyFlags ToneMappingPropertyGroup::getChangedProperties() const {
    EntityPropertyFlags changedProperties;
    CHECK_PROPERTY_CHANGE(PROP_TONE_MAPPING_EXPOSURE, exposure);
    return changedProperties;
}

void ToneMappingPropertyGroup::getProperties(EntityItemProperties& properties) const {
    COPY_ENTITY_GROUP_PROPERTY_TO_PROPERTIES(ToneMapping, Exposure, getExposure);
}

bool ToneMappingPropertyGroup::setProperties(const EntityItemProperties& properties) {
    bool somethingChanged = false;
    SET_ENTITY_GROUP_PROPERTY_FROM_PROPERTIES(ToneMapping, Exposure, exposure, setExposure);
    return somethingChanged;
}

EntityPropertyFlags ToneMappingPropertyGroup::getEntityProperties(EncodeBitstreamParams& params) const {
    EntityPropertyFlags requestedProperties;
    requestedProperties += PROP_TONE_MAPPING_EXPOSURE;
    return requestedProperties;
}
    
void ToneMappingPropertyGroup::appendSubclassData(OctreePacketData* packetData, EncodeBitstreamParams& params,
                                            EntityTreeElementExtraEncodeDataPointer entityTreeElementExtraEncodeData,
                                            EntityPropertyFlags& requestedProperties,
                                            EntityPropertyFlags& propertyFlags,
                                            EntityPropertyFlags& propertiesDidntFit,
                                            int& propertyCount, 
                                            OctreeElement::AppendState& appendState) const {
    bool successPropertyFits = true;

    APPEND_ENTITY_PROPERTY(PROP_TONE_MAPPING_EXPOSURE, getExposure());
}

int ToneMappingPropertyGroup::readEntitySubclassDataFromBuffer(const unsigned char* data, int bytesLeftToRead,
                                                         ReadBitstreamToTreeParams& args,
                                                         EntityPropertyFlags& propertyFlags, bool overwriteLocalData,
                                                         bool& somethingChanged) {
    int bytesRead = 0;
    const unsigned char* dataAt = data;

    READ_ENTITY_PROPERTY(PROP_TONE_MAPPING_EXPOSURE, float, setExposure);

    return bytesRead;
}