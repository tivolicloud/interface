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

#ifndef hifi_EntityItemID_h
#define hifi_EntityItemID_h

#include <stdint.h>

#include <QDebug>
#include <QObject>
#include <QHash>
#include <QScriptEngine>
#include <QUuid>

const QUuid UNKNOWN_ENTITY_ID; // null uuid

/// Abstract ID for editing model items. Used in EntityItem JS API.
class EntityItemID : public QUuid {
public:
    EntityItemID();
    EntityItemID(const QUuid& id);
    // EntityItemID(const EntityItemID& other);
    static EntityItemID readEntityItemIDFromBuffer(const unsigned char* data, int bytesLeftToRead);
    QScriptValue toScriptValue(QScriptEngine* engine) const;

    bool isInvalidID() const { return *this == UNKNOWN_ENTITY_ID; }
};

inline QDebug operator<<(QDebug debug, const EntityItemID& id) {
    debug << "[entity-id:" << id.toString() << "]";
    return debug;
}

Q_DECLARE_METATYPE(EntityItemID);
Q_DECLARE_METATYPE(QVector<EntityItemID>);
QScriptValue EntityItemIDtoScriptValue(QScriptEngine* engine, const EntityItemID& properties);
void EntityItemIDfromScriptValue(const QScriptValue &object, EntityItemID& properties);
QVector<EntityItemID> qVectorEntityItemIDFromScriptValue(const QScriptValue& array);

// Allow the use of std::unordered_map with QUuid keys
namespace std { template<> struct hash<EntityItemID> { size_t operator()(const EntityItemID& id) const; }; }

#endif // hifi_EntityItemID_h
