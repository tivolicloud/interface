//
//  ComponentMode.h
//  libraries/entities/src
//
//  Created by Nissim Hadar on 9/21/17.
//  Copyright 2013 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#ifndef hifi_ComponentMode_h
#define hifi_ComponentMode_h

#include <QString>

enum ComponentMode {
    COMPONENT_MODE_INHERIT,
    COMPONENT_MODE_DISABLED,
    COMPONENT_MODE_ENABLED,

    COMPONENT_MODE_ITEM_COUNT
};

enum ZoneCullingComponentMode  // TIVOLI
{
    inherit,           // Do not change the skiplist
    onInclusive,      // Add my entities to existing skiplist.
    onExclusive,      // Overwrite skiplist with my entities.
    offExclusive,     // Clear skiplist completely.

    ZONECULLING_MODE_ITEM_COUNT
};

enum AvatarPriorityMode {
    AVATAR_PRIORITY_INHERIT,
    AVATAR_PRIORITY_CROWD,
    AVATAR_PRIORITY_HERO,

    AVATAR_PRIORITY_ITEM_COUNT
};

enum EntityPriorityComponentMode {
    ENTITY_PRIORITY_STATIC,
    ENTITY_PRIORITY_AUTOMATIC,
    ENTITY_PRIORITY_PRIORITIZED,

    ENTITY_PRIORITY_ITEM_COUNT
};

class ComponentModeHelpers {
public:
    static QString getNameForComponentMode(ComponentMode mode);
};

class ZoneCullingModeHelpers {
public:
    static QString getNameForZoneCullingComponentMode(ZoneCullingComponentMode mode);
};

class AvatarPriorityModeHelpers {
public:
    static QString getNameForAvatarPriorityMode(AvatarPriorityMode mode);
};

//class EntityPriorityModeHelpers {
//public:
//    static QString getNameForEntityPriorityComponentMode(EntityPriorityComponentMode mode);
//};

#endif // hifi_ComponentMode_h

