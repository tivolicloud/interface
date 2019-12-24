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
    /*COPY_GROUP_PROPERTY_TO_QSCRIPTVALUE(PROP_BLOOM_INTENSITY, Bloom, bloom, BloomIntensity, bloomIntensity);
    COPY_GROUP_PROPERTY_TO_QSCRIPTVALUE(PROP_BLOOM_THRESHOLD, Bloom, bloom, BloomThreshold, bloomThreshold);
    COPY_GROUP_PROPERTY_TO_QSCRIPTVALUE(PROP_BLOOM_SIZE, Bloom, bloom, BloomSize, bloomSize);*/
}

void ZoneCullingPropertyGroup::copyFromScriptValue(const QScriptValue& object, bool& _defaultSettings) {
   /* COPY_GROUP_PROPERTY_FROM_QSCRIPTVALUE(bloom, bloomIntensity, float, setBloomIntensity);
    COPY_GROUP_PROPERTY_FROM_QSCRIPTVALUE(bloom, bloomThreshold, float, setBloomThreshold);
    COPY_GROUP_PROPERTY_FROM_QSCRIPTVALUE(bloom, bloomSize, float, setBloomSize);*/
}

void ZoneCullingPropertyGroup::merge(const ZoneCullingPropertyGroup& other) {
  /*  COPY_PROPERTY_IF_CHANGED(bloomIntensity);
    COPY_PROPERTY_IF_CHANGED(bloomThreshold);
    COPY_PROPERTY_IF_CHANGED(bloomSize);*/
}

void ZoneCullingPropertyGroup::debugDump() const {
    /*qCDebug(entities) << "   ZoneCullingPropertyGroup: ---------------------------------------------";
    qCDebug(entities) << "      _bloomIntensity:" << _bloomIntensity;
    qCDebug(entities) << "      _bloomThreshold:" << _bloomThreshold;
    qCDebug(entities) << "           _bloomSize:" << _bloomSize;*/
}

void ZoneCullingPropertyGroup::listChangedProperties(QList<QString>& out) {
    /*if (bloomIntensityChanged()) {
        out << "bloom-bloomIntensity";
    }
    if (bloomThresholdChanged()) {
        out << "bloom-bloomThreshold";
    }
    if (bloomSizeChanged()) {
        out << "bloom-bloomSize";
    }*/
}

bool ZoneCullingPropertyGroup::appendToEditPacket(OctreePacketData* packetData,
                                            EntityPropertyFlags& requestedProperties,
                                            EntityPropertyFlags& propertyFlags,
                                            EntityPropertyFlags& propertiesDidntFit,
                                            int& propertyCount, 
                                            OctreeElement::AppendState& appendState) const {
    bool successPropertyFits = true;

   /* APPEND_ENTITY_PROPERTY(PROP_BLOOM_INTENSITY, getBloomIntensity());
    APPEND_ENTITY_PROPERTY(PROP_BLOOM_THRESHOLD, getBloomThreshold());
    APPEND_ENTITY_PROPERTY(PROP_BLOOM_SIZE, getBloomSize());*/

    return true;
}

bool ZoneCullingPropertyGroup::decodeFromEditPacket(EntityPropertyFlags& propertyFlags,
                                                    const unsigned char*& dataAt,
                                                    int& processedBytes) {
    int bytesRead = 0;
    bool overwriteLocalData = true;
    bool somethingChanged = false;

   /* READ_ENTITY_PROPERTY(PROP_BLOOM_INTENSITY, float, setBloomIntensity);
    READ_ENTITY_PROPERTY(PROP_BLOOM_THRESHOLD, float, setBloomThreshold);
    READ_ENTITY_PROPERTY(PROP_BLOOM_SIZE, float, setBloomSize);

    DECODE_GROUP_PROPERTY_HAS_CHANGED(PROP_BLOOM_INTENSITY, BloomIntensity);
    DECODE_GROUP_PROPERTY_HAS_CHANGED(PROP_BLOOM_THRESHOLD, BloomThreshold);
    DECODE_GROUP_PROPERTY_HAS_CHANGED(PROP_BLOOM_SIZE, BloomSize);*/

    processedBytes += bytesRead;

    Q_UNUSED(somethingChanged);

    return true;
}

void ZoneCullingPropertyGroup::markAllChanged() {
   /* _bloomIntensityChanged = true;
    _bloomThresholdChanged = true;
    _bloomSizeChanged = true;*/
}

EntityPropertyFlags ZoneCullingPropertyGroup::getChangedProperties() const {
    EntityPropertyFlags changedProperties;

    /*CHECK_PROPERTY_CHANGE(PROP_BLOOM_INTENSITY, bloomIntensity);
    CHECK_PROPERTY_CHANGE(PROP_BLOOM_THRESHOLD, bloomThreshold);
    CHECK_PROPERTY_CHANGE(PROP_BLOOM_SIZE, bloomSize);*/

    return changedProperties;
}

void ZoneCullingPropertyGroup::getProperties(EntityItemProperties& properties) const {
    /*COPY_ENTITY_GROUP_PROPERTY_TO_PROPERTIES(Bloom, BloomIntensity, getBloomIntensity);
    COPY_ENTITY_GROUP_PROPERTY_TO_PROPERTIES(Bloom, BloomThreshold, getBloomThreshold);
    COPY_ENTITY_GROUP_PROPERTY_TO_PROPERTIES(Bloom, BloomSize, getBloomSize);*/
}

bool ZoneCullingPropertyGroup::setProperties(const EntityItemProperties& properties) {
    bool somethingChanged = false;

    /*SET_ENTITY_GROUP_PROPERTY_FROM_PROPERTIES(Bloom, BloomIntensity, bloomIntensity, setBloomIntensity);
    SET_ENTITY_GROUP_PROPERTY_FROM_PROPERTIES(Bloom, BloomThreshold, bloomThreshold, setBloomThreshold);
    SET_ENTITY_GROUP_PROPERTY_FROM_PROPERTIES(Bloom, BloomSize, bloomSize, setBloomSize);*/

    return somethingChanged;
}

EntityPropertyFlags ZoneCullingPropertyGroup::getEntityProperties(EncodeBitstreamParams& params) const {
    EntityPropertyFlags requestedProperties;

    /*requestedProperties += PROP_BLOOM_INTENSITY;
    requestedProperties += PROP_BLOOM_THRESHOLD;
    requestedProperties += PROP_BLOOM_SIZE;*/

    return requestedProperties;
}
    
void ZoneCullingPropertyGroup::appendSubclassData(OctreePacketData* packetData, EncodeBitstreamParams& params,
                                            EntityTreeElementExtraEncodeDataPointer entityTreeElementExtraEncodeData,
                                            EntityPropertyFlags& requestedProperties,
                                            EntityPropertyFlags& propertyFlags,
                                            EntityPropertyFlags& propertiesDidntFit,
                                            int& propertyCount, 
                                            OctreeElement::AppendState& appendState) const {
    bool successPropertyFits = true;

    /*APPEND_ENTITY_PROPERTY(PROP_BLOOM_INTENSITY, getBloomIntensity());
    APPEND_ENTITY_PROPERTY(PROP_BLOOM_THRESHOLD, getBloomThreshold());
    APPEND_ENTITY_PROPERTY(PROP_BLOOM_SIZE, getBloomSize());*/
}

int ZoneCullingPropertyGroup::readEntitySubclassDataFromBuffer(const unsigned char* data, int bytesLeftToRead,
                                                         ReadBitstreamToTreeParams& args,
                                                         EntityPropertyFlags& propertyFlags, bool overwriteLocalData,
                                                         bool& somethingChanged) {
    int bytesRead = 0;
    const unsigned char* dataAt = data;
/*
    READ_ENTITY_PROPERTY(PROP_BLOOM_INTENSITY, float, setBloomIntensity);
    READ_ENTITY_PROPERTY(PROP_BLOOM_THRESHOLD, float, setBloomThreshold);
    READ_ENTITY_PROPERTY(PROP_BLOOM_SIZE, float, setBloomSize);*/

    return bytesRead;
}
