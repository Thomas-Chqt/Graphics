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
#include "Math/Matrix.hpp"
#include "ShaderStructs.hpp"
#include "UtilsCPP/Array.hpp"
#include "UtilsCPP/Func.hpp"
#include "UtilsCPP/RuntimeError.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "UtilsCPP/String.hpp"
#include "UtilsCPP/Types.hpp"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "assimp/vector3.h"

template<>
gfx::StructLayout gfx::getLayout<shaderStruct::Vertex>()
{
    return {
        { "pos",    Type::vec3f, (void*)offsetof(shaderStruct::Vertex, pos)    },
        { "uv",     Type::vec2f, (void*)offsetof(shaderStruct::Vertex, uv)     },
        { "normal", Type::vec3f, (void*)offsetof(shaderStruct::Vertex, normal) }
    };
}

utils::UniquePtr<ModelLibrary> ModelLibrary::s_instance;

ModelLibrary::ModelLibrary(const utils::SharedPtr<gfx::GraphicAPI>& api) : m_api(api)
{
}

Model ModelLibrary::modelFromFile(const utils::String& filePath)
{
    if (m_models.contain(filePath))
        return m_models[filePath];

    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(filePath,
        aiProcess_JoinIdenticalVertices     |
        aiProcess_MakeLeftHanded            |
        aiProcess_Triangulate               |
        aiProcess_GenSmoothNormals          |
        // aiProcess_PreTransformVertices      |
        // aiProcess_ValidateDataStructure     |
        // aiProcess_RemoveRedundantMaterials  |
        aiProcess_FixInfacingNormals        |
        // aiProcess_FindInvalidData           |
        // aiProcess_GenUVCoords               |
        aiProcess_FlipUVs                   |
        aiProcess_FlipWindingOrder
    );

    if (scene == nullptr)
        throw utils::RuntimeError("fail to load the model using assimp");
    
    utils::Array<utils::SharedPtr<Material>> allMaterial;
    utils::Array<Mesh> allMeshes;

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
            m_api->newVertexBuffer(vertices),
            m_api->newIndexBuffer(indices),
            allMaterial[aiMesh->mMaterialIndex]
        });
    }

    utils::Func<SubModel(aiNode* aiNode)> subModelFromNode = [&](aiNode* aiNode)
    {
        SubModel newSubModel;

        newSubModel.name = aiNode->mName.C_Str();

        newSubModel.defaultMat = math::mat4x4(
            aiNode->mTransformation.a1, aiNode->mTransformation.a2, aiNode->mTransformation.a3, aiNode->mTransformation.a4,
            aiNode->mTransformation.b1, aiNode->mTransformation.b2, aiNode->mTransformation.b3, aiNode->mTransformation.b4,
            aiNode->mTransformation.c1, aiNode->mTransformation.c2, aiNode->mTransformation.c3, aiNode->mTransformation.c4,
            aiNode->mTransformation.d1, aiNode->mTransformation.d2, aiNode->mTransformation.d3, aiNode->mTransformation.d4
        );

        for (utils::uint32 i = 0; i < aiNode->mNumMeshes; i++)
            newSubModel.meshes.append(allMeshes[aiNode->mMeshes[i]]);

        for (utils::uint32 i = 0; i < aiNode->mNumChildren; i++)
            newSubModel.subModels.append(subModelFromNode(aiNode->mChildren[i]));

        return newSubModel;
    };

    Model newModel;

    newModel.name = scene->mRootNode->mName.C_Str();

    for (utils::uint32 i = 0; i < scene->mRootNode->mNumMeshes; i++)
        newModel.meshes.append(allMeshes[scene->mRootNode->mMeshes[i]]);

    for (utils::uint32 i = 0; i < scene->mRootNode->mNumChildren; i++)
        newModel.subModels.append(subModelFromNode(scene->mRootNode->mChildren[i]));
    
    m_models.insert(filePath, newModel);
    return newModel;
}