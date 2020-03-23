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

#include "EntityItemID.h"
#include <QtCore/QObject>
#include <QDebug>

#include <BufferParser.h>
#include <udt/PacketHeaders.h>
#include <UUID.h>

#include "RegisteredMetaTypes.h"

int entityItemIDTypeID = qRegisterMetaType<EntityItemID>();

EntityItemID::EntityItemID() : QUuid()
{
}


EntityItemID::EntityItemID(const QUuid& id) : QUuid(id)
{
}

// EntityItemID::EntityItemID(const EntityItemID& other) : QUuid(other)
// {
// }

EntityItemID EntityItemID::readEntityItemIDFromBuffer(const unsigned char* data, int bytesLeftToRead) {
    EntityItemID result;
    if (bytesLeftToRead >= NUM_BYTES_RFC4122_UUID) {
        BufferParser(data, bytesLeftToRead).readUuid(result);
    }
    return result;
}

QScriptValue EntityItemID::toScriptValue(QScriptEngine* engine) const { 
    return EntityItemIDtoScriptValue(engine, *this); 
}

QScriptValue EntityItemIDtoScriptValue(QScriptEngine* engine, const EntityItemID& id) {
    return quuidToScriptValue(engine, id);
}

void EntityItemIDfromScriptValue(const QScriptValue &object, EntityItemID& id) {
    quuidFromScriptValue(object, id);
}

QVector<EntityItemID> qVectorEntityItemIDFromScriptValue(const QScriptValue& array) {
    if (!array.isArray()) {
        return QVector<EntityItemID>();
    }
    QVector<EntityItemID> newVector;
    int length = array.property("length").toInteger();
    newVector.reserve(length);
    for (int i = 0; i < length; i++) {
        QString uuidAsString = array.property(i).toString();
        EntityItemID fromString(uuidAsString);
        newVector << fromString;
    }
    return newVector;
}

size_t std::hash<EntityItemID>::operator()(const EntityItemID& id) const { return qHash(id); }
