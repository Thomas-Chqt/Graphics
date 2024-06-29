/*
 * ---------------------------------------------------
 * LoadFile.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/29 11:05:04
 * ---------------------------------------------------
 */

#include "Entity.hpp"
#include "Graphics/GraphicAPI.hpp"
#include "ShaderStructs.hpp"
#include "UtilsCPP/Array.hpp"
#include "UtilsCPP/Func.hpp"
#include "UtilsCPP/RuntimeError.hpp"
#include "UtilsCPP/String.hpp"
#include "UtilsCPP/Types.hpp"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

template<>
gfx::StructLayout gfx::getLayout<shaderStruct::Vertex>()
{
    return {
        { "pos",    Type::vec3f, (void*)offsetof(shaderStruct::Vertex, pos)    },
        { "uv",     Type::vec2f, (void*)offsetof(shaderStruct::Vertex, uv)     },
        { "normal", Type::vec3f, (void*)offsetof(shaderStruct::Vertex, normal) }
    };
}


utils::Array<RenderableEntity> loadFile(gfx::GraphicAPI& api, const utils::String& filePath)
{
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(filePath,
        aiProcess_JoinIdenticalVertices     |
        aiProcess_MakeLeftHanded            |
        aiProcess_Triangulate               |
        aiProcess_GenSmoothNormals          |
        aiProcess_FixInfacingNormals        |
        aiProcess_FlipUVs                   |
        aiProcess_FlipWindingOrder
    );
    
    if (scene == nullptr)
        throw utils::RuntimeError("fail to load the model using assimp");

    utils::Array<utils::SharedPtr<Material>> allMaterial;
    utils::Array<RenderableEntity::Mesh> allMeshes;

    for (utils::uint32 i = 0; i < scene->mNumMaterials; i++)
        allMaterial.append(MaterialLibrary::shared().materialFromAiMaterial(scene->mMaterials[i], filePath.substr(0, filePath.lastIndexOf('/'))));
    
    for(utils::uint32 meshIndex = 0; meshIndex < scene->mNumMeshes; meshIndex++)
    {
        aiMesh* aiMesh = scene->mMeshes[meshIndex]; 

        utils::Array<shaderStruct::Vertex> vertices(aiMesh->mNumVertices);
        utils::Array<utils::uint32> indices(aiMesh->mNumFaces * 3);

        for(utils::uint32 vertexIndex = 0; vertexIndex < aiMesh->mNumVertices; vertexIndex++)
        {
            vertices[vertexIndex].pos = {
                aiMesh->mVertices[vertexIndex].x,
                aiMesh->mVertices[vertexIndex].y,
                aiMesh->mVertices[vertexIndex].z
            };
            if (aiMesh->mTextureCoords[0])
            {
                vertices[vertexIndex].uv = {
                    aiMesh->mTextureCoords[0][vertexIndex].x,
                    aiMesh->mTextureCoords[0][vertexIndex].y
                };
            }
            if (aiMesh->mNormals)
            {
                vertices[vertexIndex].normal = {
                    aiMesh->mNormals[vertexIndex].x,
                    aiMesh->mNormals[vertexIndex].y,
                    aiMesh->mNormals[vertexIndex].z
                };
            }
        }

        for(utils::uint32 faceIndex = 0; faceIndex < aiMesh->mNumFaces; faceIndex++)
        {
            indices[faceIndex * 3 + 0] = aiMesh->mFaces[faceIndex].mIndices[0];
            indices[faceIndex * 3 + 1] = aiMesh->mFaces[faceIndex].mIndices[1];
            indices[faceIndex * 3 + 2] = aiMesh->mFaces[faceIndex].mIndices[2];
        }

        allMeshes.append({
            api.newVertexBuffer(vertices),
            api.newIndexBuffer(indices),
            allMaterial[aiMesh->mMaterialIndex]
        });
    }

    utils::Func<utils::uint32(aiNode*)> countNodes = [&](aiNode* aiNode) -> int {
        utils::uint32 count = 1;
        for (utils::uint32 i = 0; i < aiNode->mNumChildren; i++)
            count += countNodes(aiNode->mChildren[i]);
        return count;
    };

    utils::Array<RenderableEntity> output;
    output.setCapacity(countNodes(scene->mRootNode));

    utils::Func<RenderableEntity*(aiNode*, RenderableEntity*)> addEntity = [&](aiNode* aiNode, RenderableEntity* parent) -> RenderableEntity* {
        output.append(RenderableEntity());
        RenderableEntity& newRenderableEntity = output.last();

        newRenderableEntity.name = aiNode->mName.C_Str();

        newRenderableEntity.baseMat = math::mat4x4(
            aiNode->mTransformation.a1, aiNode->mTransformation.a2, aiNode->mTransformation.a3, aiNode->mTransformation.a4,
            aiNode->mTransformation.b1, aiNode->mTransformation.b2, aiNode->mTransformation.b3, aiNode->mTransformation.b4,
            aiNode->mTransformation.c1, aiNode->mTransformation.c2, aiNode->mTransformation.c3, aiNode->mTransformation.c4,
            aiNode->mTransformation.d1, aiNode->mTransformation.d2, aiNode->mTransformation.d3, aiNode->mTransformation.d4
        );

        for (utils::uint32 i = 0; i < aiNode->mNumMeshes; i++)
            newRenderableEntity.meshes.append(allMeshes[aiNode->mMeshes[i]]);

        newRenderableEntity.parentEntity = parent;

        for (utils::uint32 i = 0; i < aiNode->mNumChildren; i++)
            newRenderableEntity.subEntities.append(addEntity(aiNode->mChildren[i], &newRenderableEntity));

        return &newRenderableEntity;
    };

    addEntity(scene->mRootNode, nullptr);

    return output;
}