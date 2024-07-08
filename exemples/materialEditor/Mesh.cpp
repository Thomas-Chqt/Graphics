/*
 * ---------------------------------------------------
 * Mesh.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/07/06 18:00:03
 * ---------------------------------------------------
 */

#include "Mesh.hpp"
#include "UtilsCPP/RuntimeError.hpp"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "assimp/types.h"
#include "shaderShared.hpp"

#define POST_PROCESSING_FLAGS         \
    aiProcess_JoinIdenticalVertices | \
    aiProcess_MakeLeftHanded        | \
    aiProcess_Triangulate           | \
    aiProcess_GenSmoothNormals      | \
    aiProcess_FixInfacingNormals    | \
    aiProcess_FlipUVs               | \
    aiProcess_FlipWindingOrder

utils::Array<Mesh> loadMeshes(gfx::GraphicAPI& api, const utils::String& filePath)
{
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(filePath, POST_PROCESSING_FLAGS);
    if (scene == nullptr)
        throw utils::RuntimeError("fail to load the model using assimp");

    utils::Array<SubMesh> allMeshes;
    
    for(utils::uint32 meshIndex = 0; meshIndex < scene->mNumMeshes; meshIndex++)
    {
        aiMesh* aiMesh = scene->mMeshes[meshIndex]; 

        utils::Array<shaderShared::Vertex> vertices(aiMesh->mNumVertices);
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

        SubMesh newSubMesh;
        newSubMesh.name = aiMesh->mName.C_Str();
        newSubMesh.vertexBuffer = api.newVertexBuffer(vertices);
        newSubMesh.indexBuffer = api.newIndexBuffer(indices);
        allMeshes.append(newSubMesh);
    }

    utils::Array<Mesh> output;

    utils::Func<void(aiNode*, utils::Array<SubMesh>&, math::mat4x4)> addNode = [&](aiNode* aiNode, utils::Array<SubMesh>& dst, math::mat4x4 additionalTransform) {
        SubMesh newSubMesh;

        newSubMesh.name = aiNode->mName.C_Str();

        newSubMesh.transform = additionalTransform * math::mat4x4(
            aiNode->mTransformation.a1, aiNode->mTransformation.a2, aiNode->mTransformation.a3, aiNode->mTransformation.a4,
            aiNode->mTransformation.b1, aiNode->mTransformation.b2, aiNode->mTransformation.b3, aiNode->mTransformation.b4,
            aiNode->mTransformation.c1, aiNode->mTransformation.c2, aiNode->mTransformation.c3, aiNode->mTransformation.c4,
            aiNode->mTransformation.d1, aiNode->mTransformation.d2, aiNode->mTransformation.d3, aiNode->mTransformation.d4
        );

        for (utils::uint32 i = 0; i < aiNode->mNumMeshes; i++)
            newSubMesh.childs.append(allMeshes[aiNode->mMeshes[i]]);

        for (utils::uint32 i = 0; i < aiNode->mNumChildren; i++)
            addNode(aiNode->mChildren[i], newSubMesh.childs, math::mat4x4(1.0f));

        dst.append(newSubMesh);
    };
    
    if (scene->mRootNode->mNumMeshes > 0)
    {
        Mesh newMesh;
        newMesh.name = scene->mRootNode->mName.C_Str();
        for (utils::uint32 i = 0; i < scene->mRootNode->mNumMeshes; i++)
            newMesh.subMeshes.append(allMeshes[scene->mRootNode->mMeshes[i]]);
        output.append(newMesh);
    }

    for (utils::uint32 i = 0; i < scene->mRootNode->mNumChildren; i++)
    {
        aiNode* aiNode = scene->mRootNode->mChildren[i];

        Mesh newMesh;
        newMesh.name = aiNode->mName.C_Str();
        for (utils::uint32 i = 0; i < aiNode->mNumMeshes; i++)
            newMesh.subMeshes.append(allMeshes[aiNode->mMeshes[i]]);

        for (utils::uint32 i = 0; i < aiNode->mNumChildren; i++)
            addNode(aiNode->mChildren[i], newMesh.subMeshes, math::mat4x4(1.0f));
        output.append(newMesh);
    }

    return output;
}