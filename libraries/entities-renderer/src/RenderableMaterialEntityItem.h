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


#ifndef hifi_RenderableMaterialEntityItem_h
#define hifi_RenderableMaterialEntityItem_h

#include "RenderableEntityItem.h"

#include <MaterialEntityItem.h>

#include <procedural/ProceduralMaterialCache.h>

class NetworkMaterial;

namespace render { namespace entities { 

class MaterialEntityRenderer : public TypedEntityRenderer<MaterialEntityItem> {
    using Parent = TypedEntityRenderer<MaterialEntityItem>;
    using Pointer = std::shared_ptr<MaterialEntityRenderer>;
public:
    MaterialEntityRenderer(const EntityItemPointer& entity) : Parent(entity) {}
    ~MaterialEntityRenderer() { deleteMaterial(_parentID, _parentMaterialName); }

private:
    virtual bool needsRenderUpdate() const override;
    virtual bool needsRenderUpdateFromTypedEntity(const TypedEntityPointer& entity) const override;
    virtual void doRenderUpdateAsynchronousTyped(const TypedEntityPointer& entity) override;
    virtual void doRender(RenderArgs* args) override;

    ItemKey getKey() override;
    ShapeKey getShapeKey() override;

    QString _materialURL;
    QString _materialData;
    QString _parentMaterialName;
    quint16 _priority;
    QUuid _parentID;

    MaterialMappingMode _materialMappingMode { UNSET_MATERIAL_MAPPING_MODE };
    bool _materialRepeat { false };
    glm::vec2 _materialMappingPos;
    glm::vec2 _materialMappingScale;
    float _materialMappingRot;
    Transform _transform;
    glm::vec3 _dimensions;

    bool _texturesLoaded { false };
    bool _retryApply { false };

    std::shared_ptr<NetworkMaterial> getMaterial() const;
    void setCurrentMaterialName(const std::string& currentMaterialName);

    void applyTextureTransform(std::shared_ptr<NetworkMaterial>& material);
    void applyMaterial(const TypedEntityPointer& entity);
    void deleteMaterial(const QUuid& oldParentID, const QString& oldParentMaterialName);

    NetworkMaterialResourcePointer _networkMaterial;
    NetworkMaterialResource::ParsedMaterials _parsedMaterials;
    std::shared_ptr<NetworkMaterial> _appliedMaterial;
    std::string _currentMaterialName;

};

} } 
#endif // hifi_RenderableMaterialEntityItem_h
