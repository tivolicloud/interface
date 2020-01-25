//
//  Created by Caitlyn Meeks
//  (c) 2020 Tivoli Cloud VR
//


#include "EntityPriority.h"

const char* entityPriorityNames[] = {
    "static",
    "automatic",
    "prioritized"
};

static const size_t ENTITY_PRIORITY_NAMES = (sizeof(entityPriorityNames) / sizeof(entityPriorityNames[0]));

QString EntityPriorityHelpers::getNameForEntityPriority(EntityPriority mode) {
    if (((int)mode <= 0) || ((int)mode >= (int)ENTITY_PRIORITY_NAMES)) {
        mode = (EntityPriority)0;
    }

    return entityPriorityNames[(int)mode];
}