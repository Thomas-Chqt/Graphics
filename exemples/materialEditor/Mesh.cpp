/*
 * ---------------------------------------------------
 * Mesh.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/07/06 18:00:03
 * ---------------------------------------------------
 */

#include "Mesh.hpp"
#include "Graphics/Buffer.hpp"
#include "Math/Vector.hpp"
#include "RenderMethod.hpp"
#include "UtilsCPP/RuntimeError.hpp"
#include "UtilsCPP/Types.hpp"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "assimp/types.h"

#define POST_PROCESSING_FLAGS         \
    aiProcess_JoinIdenticalVertices | \
    aiProcess_MakeLeftHanded        | \
    aiProcess_Triangulate           | \
    aiProcess_GenSmoothNormals      | \
    aiProcess_FixInfacingNormals    | \
    aiProcess_FlipUVs               | \
    aiProcess_FlipWindingOrder      | \
    aiProcess_CalcTangentSpace

template<>
utils::Array<Mesh> loadMeshes<PhongRenderMethod::Vertex>(gfx::GraphicAPI& api, const utils::String& filePath)
{
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(filePath, POST_PROCESSING_FLAGS);
    if (scene == nullptr)
        throw utils::RuntimeError("fail to load the model using assimp");

    utils::Array<SubMesh> allMeshes;

    for(utils::uint32 meshIndex = 0; meshIndex < scene->mNumMeshes; meshIndex++)
    {
        aiMesh* aiMesh = scene->mMeshes[meshIndex];

        SubMesh newSubMesh;

        newSubMesh.name = aiMesh->mName.C_Str();

        gfx::Buffer::Descriptor bufferDescriptor;
        bufferDescriptor.size = aiMesh->mNumVertices * sizeof(PhongRenderMethod::Vertex);
        newSubMesh.vertexBuffer = api.newBuffer(bufferDescriptor);
        auto* vertices = (PhongRenderMethod::Vertex*)newSubMesh.vertexBuffer->mapContent();

        bufferDescriptor.size = aiMesh->mNumFaces * 3UL * sizeof(utils::uint32);
        newSubMesh.indexBuffer = api.newBuffer(bufferDescriptor);
        auto* indices = (utils::uint32*)newSubMesh.indexBuffer->mapContent();

        for(utils::uint32 i = 0; i < aiMesh->mNumVertices; i++)
        {
            vertices[i].pos = {
                aiMesh->mVertices[i].x,
                aiMesh->mVertices[i].y,
                aiMesh->mVertices[i].z
            };
            if (aiMesh->mTextureCoords[0] != nullptr)
            {
                vertices[i].uv = {
                    aiMesh->mTextureCoords[0][i].x,
                    aiMesh->mTextureCoords[0][i].y
                };
            }
            if (aiMesh->mNormals != nullptr)
            {
                vertices[i].normal = {
                    aiMesh->mNormals[i].x,
                    aiMesh->mNormals[i].y,
                    aiMesh->mNormals[i].z
                };
            }
            if (aiMesh->mTangents != nullptr)
            {
                vertices[i].tangent = {
                    aiMesh->mTangents[i].x,
                    aiMesh->mTangents[i].y,
                    aiMesh->mTangents[i].z
                };
            }
        }

        for(utils::uint32 i = 0; i < aiMesh->mNumFaces; i++)
        {
            indices[i * 3 + 0] = aiMesh->mFaces[i].mIndices[0];
            indices[i * 3 + 1] = aiMesh->mFaces[i].mIndices[1];
            indices[i * 3 + 2] = aiMesh->mFaces[i].mIndices[2];
        }

        newSubMesh.vertexBuffer->unMapContent();
        newSubMesh.indexBuffer->unMapContent();

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
            addNode(aiNode->mChildren[i], newSubMesh.childs, math::mat4x4(1.0F));

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
            addNode(aiNode->mChildren[i], newMesh.subMeshes, math::mat4x4(1.0F));
        output.append(newMesh);
    }

    return output;
}
