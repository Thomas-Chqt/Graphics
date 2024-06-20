/*
 * ---------------------------------------------------
 * helpers.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/14 23:42:59
 * ---------------------------------------------------
 */

#include "helpers.hpp"
#include "Math/Constants.hpp"
#include "UtilsCPP/RuntimeError.hpp"
#include "UtilsCPP/String.hpp"
#include "UtilsCPP/Types.hpp"
#include "assimp/material.h"
#include "assimp/mesh.h"
#include "stb_image.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include "Vertex.hpp"

utils::SharedPtr<gfx::GraphicPipeline> makePipeline(utils::SharedPtr<gfx::GraphicAPI> api, const utils::String& shaderName)
{
    gfx::GraphicPipeline::Descriptor graphicPipelineDescriptor;
    #ifdef GFX_METAL_ENABLED
        graphicPipelineDescriptor.metalVSFunction = shaderName + "_vs";
        graphicPipelineDescriptor.metalFSFunction = shaderName + "_fs";
    #endif 
    #if GFX_OPENGL_ENABLED
        graphicPipelineDescriptor.openglVSCode = utils::String::contentOfFile(utils::String(OPENGL_SHADER_DIR) + utils::String("/") + shaderName + utils::String(".vs"));
        graphicPipelineDescriptor.openglFSCode = utils::String::contentOfFile(utils::String(OPENGL_SHADER_DIR) + utils::String("/") + shaderName + utils::String(".fs"));
    #endif

    return api->newGraphicsPipeline(graphicPipelineDescriptor);
}

utils::SharedPtr<gfx::Texture> textureFromFile(const utils::SharedPtr<gfx::GraphicAPI>& api, const utils::String& path)
{
    int width;
    int height;
    stbi_uc* imgBytes = stbi_load(path, &width, &height, nullptr, STBI_rgb_alpha);

    gfx::Texture::Descriptor textureDescriptor;
    textureDescriptor.width = width;
    textureDescriptor.height = height;

    utils::SharedPtr<gfx::Texture> texture = api->newTexture(textureDescriptor);
    texture->setBytes(imgBytes);

    stbi_image_free(imgBytes);

    return texture;
}

math::mat4x4 makeProjectionMatrix(const utils::SharedPtr<gfx::Window>& window)
{
    utils::uint32 w, h;
    window->getWindowSize(&w, &h);

    float fov = 60 * (PI / 180.0f);
    float aspectRatio = (float)w / (float)h;
    float zNear = 0.1f;
    float zFar = 10000;

    float ys = 1 / std::tan(fov * 0.5);
    float xs = ys / aspectRatio;
    float zs = zFar / (zFar - zNear);

    return math::mat4x4 (xs,   0,  0,           0,
                          0,  ys,  0,           0,
                          0,   0, zs, -zNear * zs,
                          0,   0,  1,           0);
}

utils::Array<SubMesh> loadModel(const utils::SharedPtr<gfx::GraphicAPI>& api, const utils::String& filePath)
{
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
    
    utils::Array<SubMesh> output(scene->mNumMeshes);

    for(utils::uint32 meshIndex = 0; meshIndex < scene->mNumMeshes; meshIndex++)
    {
        aiMesh* aiMesh = scene->mMeshes[meshIndex]; 

        utils::Array<Vertex> vertices(aiMesh->mNumVertices);
        utils::Array<utils::uint32> indices(aiMesh->mNumFaces * 3);
        utils::String textureFilePath;

        for(utils::uint32 vertexIndex = 0; vertexIndex < aiMesh->mNumVertices; vertexIndex++)
        {
            Vertex vertex;
            vertex.pos = {
                aiMesh->mVertices[vertexIndex].x,
                aiMesh->mVertices[vertexIndex].y,
                aiMesh->mVertices[vertexIndex].z
            };
            if (aiMesh->mTextureCoords[0])
            {
                vertex.uv = {
                    aiMesh->mTextureCoords[0][vertexIndex].x,
                    aiMesh->mTextureCoords[0][vertexIndex].y
                };
            }
            vertex.normal = {
                aiMesh->mNormals[vertexIndex].x,
                aiMesh->mNormals[vertexIndex].y,
                aiMesh->mNormals[vertexIndex].z
            };
            vertices[vertexIndex] = vertex;
        }

        for(utils::uint32 faceIndex = 0; faceIndex < aiMesh->mNumFaces; faceIndex++)
        {
            indices[faceIndex * 3 + 0] = aiMesh->mFaces[faceIndex].mIndices[0];
            indices[faceIndex * 3 + 1] = aiMesh->mFaces[faceIndex].mIndices[1];
            indices[faceIndex * 3 + 2] = aiMesh->mFaces[faceIndex].mIndices[2];
        }

        aiMaterial* aiMaterial = scene->mMaterials[aiMesh->mMaterialIndex];
        if (aiMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0)
        {
            aiString path;
            if (aiMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS)
                textureFilePath = filePath.substr(0, filePath.lastIndexOf('/')) + "/" + utils::String(path.data);
            else
                throw utils::RuntimeError("Failed to get the texture");
        }

        output[meshIndex].vertexBuffer = api->newVertexBuffer(vertices);
        output[meshIndex].indexBuffer = api->newIndexBuffer(indices);
        if (textureFilePath.isEmpty() == false)
            output[meshIndex].texture = textureFromFile(api, textureFilePath);
    }

    return output;
}
