//
//  AssimpSerializer.cpp
//  libraries/fbx/src
//
//  Created by Maki Deprez on 12/20/20.
//  Copyright 2020 Tivoli Cloud VR, Inc.
//
//  Distributed under the GNU Affero General Public License, Version 3.0.
//  See the accompanying file LICENSE or https://www.gnu.org/licenses/agpl-3.0.en.html
//

#include "AssimpSerializer.h"

#include <BlendshapeConstants.h>
#include <ResourceManager.h>
#include <PathUtils.h>
#include <hfm/ModelFormatLogging.h>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/IOStream.hpp>
#include <assimp/IOSystem.hpp>
#include <assimp/pbrmaterial.h>
// #include <assimp/Exporter.hpp>

#include <QtGui/QImage>
#include <QtCore/QBuffer>

MediaType AssimpSerializer::getMediaType() const {
    MediaType mediaType;

    Assimp::Importer importer;
    aiString aiExts;
    importer.GetExtensionList(aiExts);

    auto exts = QString(aiExts.C_Str()).split(";");
    for (QString anyExt : exts) {
        QString ext = anyExt.replace("*.", "");
        if (ext == "fbx" || ext == "obj") continue;

        // qDebug(modelformat) << "Available format:" << ext; 
        mediaType.extensions.push_back(ext.toStdString());
        // mediaType.webMediaTypes.push_back("");
    }

    return mediaType;
}

std::unique_ptr<hfm::Serializer::Factory> AssimpSerializer::getFactory() const {
    return std::make_unique<hfm::Serializer::SimpleFactory<AssimpSerializer>>();
}

class TivoliIOStream : public Assimp::IOStream {
    friend class TivoliIOSystem;

protected:
    hifi::URL url;
    
    TivoliIOStream(hifi::URL url) : url(url) {}

public:
    ~TivoliIOStream() {}
    size_t Read(void* pvBuffer, size_t pSize, size_t pCount) override {
        // TODO: i think this function runs twice for some reason
        auto request = DependencyManager::get<ResourceManager>()->createResourceRequest(
            nullptr, url, true, -1, "TivoliIOSystem::Exists"
        );
        if (!request)  return 0;

        QEventLoop loop;
        QObject::connect(request, &ResourceRequest::finished, &loop, &QEventLoop::quit);
        request->send();
        loop.exec();

        if (request->getResult() == ResourceRequest::Success) {
            auto data = request->getData();
            memcpy(pvBuffer, data.data(), data.size());
            // return data.size();
            return pCount;
        } else {
            return 0;
        }
    }
    size_t Write(const void* pvBuffer, size_t pSize, size_t pCount) override {
        return 0;
    }
    aiReturn Seek(size_t pOffset, aiOrigin pOrigin) override {
        return aiReturn_FAILURE;
    }
    size_t Tell() const override {
        return 0;
    }
    size_t FileSize() const override {
        return 0;
    }
    void Flush () override {
    }
};

class TivoliIOSystem : public Assimp::IOSystem {
public:
    hifi::URL originalUrl;

    TivoliIOSystem(hifi::URL originalUrl) : originalUrl(originalUrl) {}
    ~TivoliIOSystem() {}

    bool Exists(const char* pFile) const override {
        return true;
    }
    char getOsSeparator() const override {
        return '/';
    }
    Assimp::IOStream* Open(const char* pFile, const char* pMode) override {
        return new TivoliIOStream(originalUrl.resolved(QString::fromLocal8Bit(pFile)));
    }
    void Close(Assimp::IOStream* pFile) override {
        delete pFile;
    }
};

bool isEmptyVec3(glm::vec3 v) { return v.x == 0 && v.y == 0 && v.z == 0; }

glm::vec3 asVec3(aiColor3D c) { return glm::vec3(c.r, c.g, c.b); }
glm::vec3 asVec3(aiVector3D v) { return glm::vec3(v.x, v.y, v.z); }
glm::mat4 asMat4(aiMatrix4x4 from) {
    glm::mat4 to;
    to[0][0] = from.a1; to[0][1] = from.b1; to[0][2] = from.c1; to[0][3] = from.d1;
    to[1][0] = from.a2; to[1][1] = from.b2; to[1][2] = from.c2; to[1][3] = from.d2;
    to[2][0] = from.a3; to[2][1] = from.b3; to[2][2] = from.c3; to[2][3] = from.d3;
    to[3][0] = from.a4; to[3][1] = from.b4; to[3][2] = from.c4; to[3][3] = from.d4;
    return to;
}

HFMTexture AssimpSerializer::getHfmTexture(aiString aiPath) {
    QString path = aiPath.C_Str();

    if (hfmTextures.contains(path)) {
        return hfmTextures[path];
    } else {
        HFMTexture texture = HFMTexture();

        QRegExp indexFinder = QRegExp("^\\*([0-9])+$");
        if (indexFinder.exactMatch(path)) {
            unsigned int index = indexFinder.cap(1).toInt();

            if (index < scene->mNumTextures) {
                auto aiTexture = scene->mTextures[index];
                unsigned int width = aiTexture->mWidth;
                unsigned int height = aiTexture->mHeight;

                texture.name = aiTexture->mFilename.C_Str();
                // texture.filename = url.toEncoded().append(index);
                texture.filename = QString::number(index).toLocal8Bit();

                if (height == 0) {
                    texture.content = QByteArray((char*)aiTexture->pcData, width);
                } else {
                    QImage image = QImage(width, height, QImage::Format::Format_RGBA8888);
                    auto aiTexel = aiTexture->pcData;
                    
                    for (unsigned int i = 0; i < width * height; i++) {
                        auto pixel = aiTexel[i];
                        image.setPixelColor(i % width, i / width, QColor(
                            pixel.r, pixel.g, pixel.b, pixel.a
                        ));
                    }

                    QByteArray imageData;
                    QBuffer imageBuffer(&imageData);
                    imageBuffer.open(QIODevice::WriteOnly);
                    image.save(&imageBuffer, "PNG");
                    imageBuffer.close();
                    texture.content = imageData;                    
                }
            }
        } else {
            QUrl textureUrl = url.resolved(path);
            texture.name = textureUrl.fileName();
            texture.filename = textureUrl.toEncoded();
        }

        hfmTextures[path] = texture;
        return texture;
    }
}

void AssimpSerializer::processMaterials() {
    for(unsigned int materialIndex = 0; materialIndex < scene->mNumMaterials; materialIndex++) {
        aiMaterial* mMaterial = scene->mMaterials[materialIndex];

        // for(unsigned int propIndex = 0; propIndex < mMaterial->mNumProperties; propIndex++) {
        //     auto prop = mMaterial->mProperties[propIndex];
        //     qCDebug(modelformat)<<materialIndex<<prop->mKey.C_Str();
        // }

        hfmModel->materials.emplace_back();
        hfm::Material* materialPtr = &hfmModel->materials.back();

        materialPtr->name = materialPtr->materialID = mMaterial->GetName().C_Str();
        materialPtr->_material = std::make_shared<graphics::Material>();
        auto modelMaterial = materialPtr->_material;

        bool isGltf = ext == "gltf" || ext == "glb";
        if (isGltf) {
            materialPtr->isPBSMaterial = true;
        }

        // colors

        aiColor3D diffuse;
        if (mMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse) == AI_SUCCESS) {
            modelMaterial->setAlbedo(asVec3(diffuse));
        }

        float opacity;
        if (mMaterial->Get(AI_MATKEY_OPACITY, opacity) == AI_SUCCESS) {
            modelMaterial->setOpacity(opacity);
        }

        aiColor3D emissive;
        if (mMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, emissive) == AI_SUCCESS) {
            modelMaterial->setEmissive(asVec3(emissive));
        }

        aiColor4D baseColor;
        if (mMaterial->Get(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_BASE_COLOR_FACTOR, baseColor) == AI_SUCCESS) {
            modelMaterial->setAlbedo(glm::vec3(baseColor.r, baseColor.g, baseColor.b));
            modelMaterial->setOpacity(baseColor.a);
        }

        float roughnessFactor;
        if (mMaterial->Get(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_ROUGHNESS_FACTOR, roughnessFactor) == AI_SUCCESS) {
            materialPtr->roughness = roughnessFactor;
            modelMaterial->setRoughness(roughnessFactor);
        }

        float metallicFactor;
        if (mMaterial->Get(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_METALLIC_FACTOR, metallicFactor) == AI_SUCCESS) {
            materialPtr->metallic = metallicFactor;
            modelMaterial->setMetallic(metallicFactor);
        }

        // TODO: scattering!

        // textures

        aiString diffuseTexture;
        if (mMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &diffuseTexture) == AI_SUCCESS) {
            materialPtr->albedoTexture = getHfmTexture(diffuseTexture);
            materialPtr->opacityTexture = getHfmTexture(diffuseTexture);
            materialPtr->useAlbedoMap = true;
        }

        aiString emissiveTexture;
        if (mMaterial->GetTexture(aiTextureType_EMISSIVE, 0, &emissiveTexture) == AI_SUCCESS) {
            materialPtr->emissiveTexture = getHfmTexture(emissiveTexture);
            materialPtr->useEmissiveMap = true;
        }

        aiString occlusionTexture;
        if (mMaterial->GetTexture(aiTextureType_AMBIENT_OCCLUSION, 0, &occlusionTexture) == AI_SUCCESS) {
            materialPtr->occlusionTexture = getHfmTexture(occlusionTexture);
            materialPtr->useOcclusionMap = true;
        }

        // assimp maps occlusion as lightmap for gltf
        aiString lightmapTexture;
        if (mMaterial->GetTexture(aiTextureType_LIGHTMAP, 0, &lightmapTexture) == AI_SUCCESS) {
            // materialPtr->lightmapTexture = getHfmTexture(lightmapTexture); // probably should be avoided anyway as it breaks pbr
            materialPtr->occlusionTexture = getHfmTexture(lightmapTexture);
            materialPtr->useOcclusionMap = true;
            if (isGltf) {
                materialPtr->occlusionTexture.sourceChannel = image::ColorChannel::RED;
            }
        }

        aiString normalTexture;
        if (mMaterial->GetTexture(aiTextureType_NORMALS, 0, &normalTexture) == AI_SUCCESS) {
            materialPtr->normalTexture = getHfmTexture(normalTexture);
            materialPtr->useNormalMap = true;
        }

        aiString metallicTexture;
        if (mMaterial->GetTexture(aiTextureType_METALNESS, 0, &metallicTexture) == AI_SUCCESS) {
            materialPtr->metallicTexture = getHfmTexture(metallicTexture);
            materialPtr->useMetallicMap = true;
        }
        
        aiString metallicRoughnessTexture;
        if (mMaterial->GetTexture(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_METALLICROUGHNESS_TEXTURE, &metallicRoughnessTexture) == AI_SUCCESS) {          
            materialPtr->roughnessTexture = getHfmTexture(metallicRoughnessTexture);
            materialPtr->roughnessTexture.sourceChannel = image::ColorChannel::GREEN;
            materialPtr->useRoughnessMap = true;

            materialPtr->metallicTexture = getHfmTexture(metallicRoughnessTexture);
            materialPtr->metallicTexture.sourceChannel = image::ColorChannel::BLUE;
            materialPtr->useMetallicMap = true;
        }

        // extras
        
        int cullNone;
        if (mMaterial->Get(AI_MATKEY_TWOSIDED, cullNone) == AI_SUCCESS && cullNone) {
            modelMaterial->setCullFaceMode(graphics::MaterialKey::CullFaceMode::CULL_NONE);
        }

        int unlit;
        if (mMaterial->Get(AI_MATKEY_GLTF_UNLIT, unlit) == AI_SUCCESS && unlit) {
            modelMaterial->setUnlit(true);
        }

        aiString alphaMode;
        if (mMaterial->Get(AI_MATKEY_GLTF_ALPHAMODE, alphaMode) == AI_SUCCESS) {
            QString alphaModeStr = alphaMode.C_Str();
            modelMaterial->setOpacityMapMode(
                alphaModeStr == "BLEND" ?
                    graphics::MaterialKey::OPACITY_MAP_BLEND :
                alphaModeStr == "MASK" ?
                    graphics::MaterialKey::OPACITY_MAP_MASK :
                graphics::MaterialKey::OPACITY_MAP_OPAQUE
            );
        } else {
            modelMaterial->setOpacityMapMode(graphics::MaterialKey::OPACITY_MAP_OPAQUE);
        }

    }
}

void AssimpSerializer::processMeshes() {
    for (unsigned int meshIndex = 0; meshIndex < scene->mNumMeshes; meshIndex++) {
        aiMesh* mMesh = scene->mMeshes[meshIndex];

        hfmModel->meshes.emplace_back();
        hfm::Mesh* meshPtr = &hfmModel->meshes.back();

        meshPtr->parts.emplace_back();
        hfm::MeshPart* meshPartPtr = &meshPtr->parts.back();

        meshPtr->meshIndex = meshIndex;

        for (unsigned int faceIndex = 0; faceIndex < mMesh->mNumFaces; faceIndex++) {
            aiFace face = mMesh->mFaces[faceIndex];
            if (face.mNumIndices != 3) continue; // must be a triangle

            for (unsigned int i = 0; i < face.mNumIndices; i++) {
                auto vertexIndex = face.mIndices[i];

                auto v = mMesh->mVertices[vertexIndex];
                meshPartPtr->triangleIndices.push_back(meshPtr->vertices.count());
                meshPtr->vertices.push_back(asVec3(v));

                auto n = mMesh->mNormals[vertexIndex];
                meshPtr->normals.push_back(asVec3(n));

                if (mMesh->HasTangentsAndBitangents()) {
                    auto t = mMesh->mTangents[vertexIndex];
                    meshPtr->tangents.push_back(asVec3(t));
                }

                // TODO: fix https://github.com/assimp/assimp/issues/1702
                if (mMesh->HasVertexColors(0)) {
                    auto c = mMesh->mColors[0][vertexIndex];
                    meshPtr->colors.push_back(glm::vec3(c.r, c.g, c.b));
                }

                for (unsigned int uvI = 0; uvI <= 1; uvI++) {
                    if (!mMesh->HasTextureCoords(uvI)) break;
                    auto uv = mMesh->mTextureCoords[uvI][vertexIndex];
                    if (uvI == 0) {
                        meshPtr->texCoords.push_back(glm::vec2(uv.x, uv.y));
                    } else {
                        meshPtr->texCoords1.push_back(glm::vec2(uv.x, uv.y));
                    }
                }
            }
        }

        if (mMesh->mNumAnimMeshes > 0) {
            bool isBlendshapeChannelNamesEmpty = hfmModel->blendshapeChannelNames.size() == 0;
            for (int i = 0; i < (int)Blendshapes::BlendshapeCount; i++) {
                meshPtr->blendshapes.push_back(hfm::Blendshape());
                if (isBlendshapeChannelNamesEmpty) {
                    hfmModel->blendshapeChannelNames.push_back(BLENDSHAPE_NAMES[i]);
                }
            }
        }

        for (unsigned int blendshapeIndex = 0; blendshapeIndex < mMesh->mNumAnimMeshes; blendshapeIndex++) {
            aiAnimMesh* animMesh = mMesh->mAnimMeshes[blendshapeIndex];

            QString name = animMesh->mName.C_Str();
            if (!BLENDSHAPE_LOOKUP_MAP.contains(name)) {
                continue;
            }
            
            int globalBlendshapeIndex = BLENDSHAPE_LOOKUP_MAP[name];
            hfm::Blendshape* blendshapePtr = &meshPtr->blendshapes[globalBlendshapeIndex];

            int indexCount = 0;
            for (unsigned int faceIndex = 0; faceIndex < mMesh->mNumFaces; faceIndex++) {
                aiFace face = mMesh->mFaces[faceIndex];
                if (face.mNumIndices != 3) continue; // must be a triangle

                for (unsigned int i = 0; i < face.mNumIndices; i++) {
                    auto vertexIndex = face.mIndices[i];

                    auto v = (
                        asVec3(animMesh->mVertices[vertexIndex]) -
                        asVec3(mMesh->mVertices[vertexIndex])
                    );
                    auto n = (
                        asVec3(animMesh->mNormals[vertexIndex]) -
                        asVec3(mMesh->mNormals[vertexIndex])
                    );
                    auto t = animMesh->HasTangentsAndBitangents() ? (
                        asVec3(animMesh->mTangents[vertexIndex]) -
                        asVec3(mMesh->mTangents[vertexIndex])
                    ) : vec3(0.0f);
                    
                    if (isEmptyVec3(v) && isEmptyVec3(n) && isEmptyVec3(t)) {
                        indexCount++;
                        continue;
                    }

                    blendshapePtr->indices.push_back(indexCount);
                    indexCount++;
                    
                    blendshapePtr->vertices.push_back(v);
                    blendshapePtr->normals.push_back(n);
                    if (animMesh->HasTangentsAndBitangents()) {
                        blendshapePtr->tangents.push_back(t);
                    }
                }
            }
        }
    }
}

void AssimpSerializer::processNode(const aiNode* aiNode, int parentIndex) {
    int nodeIndex = hfmModel->joints.size();
    hfmModel->joints.emplace_back();
    hfmModel->jointIndices["x"]++;
    hfm::Joint* nodePtr = &hfmModel->joints.back();

    // set up node (joint)
    nodePtr->name = aiNode->mName.C_Str();
    nodePtr->parentIndex = parentIndex;
    nodePtr->isSkeletonJoint = false;

    nodePtr->transform = asMat4(aiNode->mTransformation);
    nodePtr->translation = extractTranslation(nodePtr->transform);
    nodePtr->rotation = glmExtractRotation(nodePtr->transform);
    glm::vec3 scale = extractScale(nodePtr->transform);
    nodePtr->postTransform = glm::scale(glm::mat4(), scale);

    nodePtr->globalTransform = nodePtr->transform;
    if (nodePtr->parentIndex != -1) {
        const auto& parentNode = hfmModel->joints[nodePtr->parentIndex];
        nodePtr->transform = parentNode.transform * nodePtr->transform;
        nodePtr->globalTransform = nodePtr->globalTransform * parentNode.globalTransform;
    } else {
        // TODO: offset as in tx,y,z rx,y,z from fst?
        // joint.globalTransform = hfmModel.offset * joint.globalTransform;
    }

    // for all meshes in node, create shape
    for (unsigned int i = 0; i < aiNode->mNumMeshes; i++) {
        auto meshIndex = aiNode->mMeshes[i];

        hfmModel->shapes.emplace_back();
        hfm::Shape* shapePtr = &hfmModel->shapes.back();

        shapePtr->joint = nodeIndex;
        shapePtr->mesh = meshIndex;
        shapePtr->meshPart = 0;

        // TODO: fixes bounding box? seems not for https://files.tivolicloud.com/caitlyn/fun/cu-cat/cu-cat.glb
        hfmModel->meshes[meshIndex].modelTransform = nodePtr->globalTransform;

        unsigned int materialIndex = scene->mMeshes[meshIndex]->mMaterialIndex;
        if (materialIndex < hfmModel->materials.size()) {
            shapePtr->material = materialIndex;
        }
    }

    // process children
    for (unsigned int i = 0; i < aiNode->mNumChildren; i++) {
        processNode(aiNode->mChildren[i], nodeIndex);
    }
}

void AssimpSerializer::processScene() {
    hfmModel = std::make_shared<HFMModel>();
    hfmModel->originalURL = url.toString();

    processMaterials();
    processMeshes();

    hfmModel->jointIndices["x"] = 0;
    processNode(scene->mRootNode);
}

HFMModel::Pointer AssimpSerializer::read(const hifi::ByteArray& data, const hifi::VariantHash& mapping, const hifi::URL& inputUrl) {
	url = inputUrl;

    // normalize url for local files
    hifi::URL normalizeUrl = DependencyManager::get<ResourceManager>()->normalizeURL(url);
    if (normalizeUrl.scheme().isEmpty() || (normalizeUrl.scheme() == "file")) {
        QString localFileName = PathUtils::expandToLocalDataAbsolutePath(normalizeUrl).toLocalFile();
        url = hifi::URL(QFileInfo(localFileName).absoluteFilePath());
    }

    ext = QFileInfo(url.path()).completeSuffix();

    qCDebug(modelformat) << "AssimpSerializer::read url"<<url<<ext;

    Assimp::Importer importer;
    importer.SetIOHandler(new TivoliIOSystem(url));
    
    scene = importer.ReadFileFromMemory(
        data.constData(), data.size(),
        // aiProcess_CalcTangentSpace |
        aiProcess_JoinIdenticalVertices | 
        aiProcess_Triangulate |
        aiProcess_GenNormals | 
        // aiProcess_SplitLargeMeshes |
        // aiProcess_RemoveRedundantMaterials | // ends up removing necessary materials...
        // aiProcess_OptimizeMeshes |
        // aiProcess_OptimizeGraph,
        aiProcess_FlipUVs,
        ext.toLocal8Bit()
    );
    
    if (!scene) {
        qCDebug(modelformat) << "AssimpSerializer::read Error parsing model file"<<importer.GetErrorString();
        return nullptr;
    }

    processScene();

    // hfmModel->debugDump();

    // for (size_t i = 0; i < aiGetExportFormatCount(); i++) {
    //     auto desc = aiGetExportFormatDescription(i);
    //     qDebug() << desc->id<<desc->fileExtension<<desc->description;
    // }

    // QString fileName = QFileInfo(url.path()).fileName();
    // aiExportScene(
    //     scene, "assbin",
    //     QString("/home/maki/assimp/"+fileName.replace("."+ext, ".assbin")).toLocal8Bit(),
    //     0
    // );
    // aiExportScene(
    //     scene, "json",
    //     QString("/home/maki/assimp/"+fileName.replace("."+ext, ".json")).toLocal8Bit(),
    //     0
    // );

    return hfmModel;
}
