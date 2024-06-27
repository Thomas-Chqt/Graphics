/*
 * ---------------------------------------------------
 * MeshLibrary.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/24 15:11:52
 * ---------------------------------------------------
 */

#include "MeshLibrary.hpp"
#include "MaterialLibrary.hpp"
#include "ShaderStructs.hpp"
#include "UtilsCPP/RuntimeError.hpp"
#include "UtilsCPP/String.hpp"
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

utils::UniquePtr<MeshLibrary> MeshLibrary::s_instance;

MeshLibrary::MeshLibrary(const utils::SharedPtr<gfx::GraphicAPI>& api) : m_api(api)
{
}

Mesh MeshLibrary::meshFromFile(const utils::String& filePath)
{
    if (m_meshes.contain(filePath))
        return m_meshes[filePath];

    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(filePath,
        aiProcess_JoinIdenticalVertices     |
        aiProcess_MakeLeftHanded            |
        aiProcess_Triangulate               |
        aiProcess_GenSmoothNormals          |
        aiProcess_PreTransformVertices      |
        aiProcess_ValidateDataStructure     |
        aiProcess_RemoveRedundantMaterials  |
        aiProcess_FixInfacingNormals        |
        aiProcess_FindInvalidData           |
        aiProcess_GenUVCoords               |
        aiProcess_FlipUVs                   |
        aiProcess_FlipWindingOrder
    );

    if (scene == nullptr)
        throw utils::RuntimeError("fail to load the model using assimp");
    
    Mesh newMesh = {
        "mesh " + utils::String::fromUInt(m_meshes.size()),
        utils::Array<SubMesh>(scene->mNumMeshes)
    };

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

        newMesh.subMeshes[meshIndex].vertexBuffer = m_api->newVertexBuffer(vertices);
        newMesh.subMeshes[meshIndex].indexBuffer = m_api->newIndexBuffer(indices);
        newMesh.subMeshes[meshIndex].material = MaterialLibrary::shared().materialFromAiMaterial(scene->mMaterials[aiMesh->mMaterialIndex]);
    }

    m_meshes.insert(filePath, newMesh);
    return newMesh;
}