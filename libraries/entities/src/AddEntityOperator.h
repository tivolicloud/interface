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


#ifndef hifi_AddEntityOperator_h
#define hifi_AddEntityOperator_h

#include <memory>

#include <AABox.h>
#include <Octree.h>

#include "EntityTypes.h"

class EntityTree;
using EntityTreePointer = std::shared_ptr<EntityTree>;

class AddEntityOperator : public RecurseOctreeOperator {
public:
    AddEntityOperator(EntityTreePointer tree, EntityItemPointer newEntity);

    virtual bool preRecursion(const OctreeElementPointer& element) override;
    virtual bool postRecursion(const OctreeElementPointer& element) override;
    virtual OctreeElementPointer possiblyCreateChildAt(const OctreeElementPointer& element, int childIndex) override;
private:
    EntityTreePointer _tree;
    EntityItemPointer _newEntity;
    AABox _newEntityBox;
    bool _foundNew;
};


#endif // hifi_AddEntityOperator_h
