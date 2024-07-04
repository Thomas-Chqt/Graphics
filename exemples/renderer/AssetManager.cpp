/*
 * ---------------------------------------------------
 * AssetManager.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/07/01 09:56:16
 * ---------------------------------------------------
 */

#include "AssetManager.hpp"
#include "Graphics/StructLayout.hpp"
#include "Graphics/Texture.hpp"
#include "RenderMethod.hpp"
#include "ShaderStructs.hpp"
#include "UtilsCPP/Array.hpp"
#include "UtilsCPP/Func.hpp"
#include "UtilsCPP/RuntimeError.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "UtilsCPP/Types.hpp"
#include "assimp/Importer.hpp"
#include "assimp/material.h"
#include "assimp/scene.h"
#include "assimp/types.h"
#include "stb_image.h"
#include "assimp/postprocess.h"

#define POST_PROCESSING_FLAGS         \
    aiProcess_JoinIdenticalVertices | \
    aiProcess_MakeLeftHanded        | \
    aiProcess_Triangulate           | \
    aiProcess_GenSmoothNormals      | \
    aiProcess_FixInfacingNormals    | \
    aiProcess_FlipUVs               | \
    aiProcess_FlipWindingOrder

utils::UniquePtr<AssetManager> AssetManager::s_instance;

template<>
gfx::StructLayout gfx::getLayout<shaderStruct::Vertex>()
{
    return {
        { "pos",    Type::vec3f, (void*)offsetof(shaderStruct::Vertex, pos)    },
        { "uv",     Type::vec2f, (void*)offsetof(shaderStruct::Vertex, uv)     },
        { "normal", Type::vec3f, (void*)offsetof(shaderStruct::Vertex, normal) }
    };
}

utils::SharedPtr<gfx::Texture> AssetManager::texture(const utils::String& filePath)
{
    if (m_textures.contain(filePath))
        return m_textures[filePath];

    int width;
    int height;
    stbi_uc* imgBytes = stbi_load(filePath, &width, &height, nullptr, STBI_rgb_alpha);
    if (imgBytes == nullptr)
        throw utils::RuntimeError("fail to read texture at path: " + filePath);

    gfx::Texture::Descriptor textureDescriptor;
    textureDescriptor.width = width;
    textureDescriptor.height = height;

    utils::SharedPtr<gfx::Texture> texture = m_api->newTexture(textureDescriptor);
    texture->setBytes(imgBytes);

    stbi_image_free(imgBytes);

    m_textures.insert(filePath, texture);
    return texture;
}

utils::SharedPtr<Material> AssetManager::material(const utils::String& name)
{
    for (auto& mat : m_materials) {
        if (mat->name == name)
            return mat;
    }
    utils::SharedPtr<Material> newMaterial(new Material);
    newMaterial->name = name;
    newMaterial->renderMethod = utils::SharedPtr<IRenderMethod>(new RenderMethod<VertexShader::universal3D, FragmentShader::universal>(m_api));
    
    m_materials.append(newMaterial);
    return newMaterial;
}

utils::SharedPtr<Material> AssetManager::material(aiMaterial* aiMaterial, const utils::String& baseDir)
{
    utils::SharedPtr<Material> newMaterial = material(aiMaterial->GetName().length == 0 ? "Material" : aiMaterial->GetName().C_Str());

    aiColor3D ambientColor  = { 1, 1, 1 };
    aiColor3D diffuseColor  = { 1, 1, 1 };
    aiColor3D specularColor = { 0, 0, 0 };
    aiColor3D emissiveColor = { 0, 0, 0 };

    aiMaterial->Get(AI_MATKEY_COLOR_AMBIENT, ambientColor);
    newMaterial->ambientColor  = { ambientColor.r,  ambientColor.g,  ambientColor.b  };

    aiMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor);
    newMaterial->diffuseColor  = { diffuseColor.r,  diffuseColor.g,  diffuseColor.b  };

    aiMaterial->Get(AI_MATKEY_COLOR_SPECULAR, specularColor);
    newMaterial->specularColor = { specularColor.r, specularColor.g, specularColor.b };

    aiMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, emissiveColor);
    newMaterial->emissiveColor = { emissiveColor.r, emissiveColor.g, emissiveColor.b };

    aiMaterial->Get(AI_MATKEY_SHININESS, newMaterial->shininess);

    if (aiMaterial->GetTextureCount(aiTextureType_AMBIENT) > 0)
    {
        aiString path;
        if (aiMaterial->GetTexture(aiTextureType_AMBIENT, 0, &path) != AI_SUCCESS)
            throw utils::RuntimeError("Failed to get aiTextureType_AMBIENT for idx 0");
        newMaterial->ambientTexture = texture(baseDir + "/" + utils::String(path.C_Str()));
    }
    if (aiMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0)
    {
        aiString path;
        if (aiMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &path) != AI_SUCCESS)
            throw utils::RuntimeError("Failed to get aiTextureType_DIFFUSE for idx 0");
        newMaterial->diffuseTexture = texture(baseDir + "/" + utils::String(path.C_Str()));
    }
    if (aiMaterial->GetTextureCount(aiTextureType_SPECULAR) > 0)
    {
        aiString path;
        if (aiMaterial->GetTexture(aiTextureType_SPECULAR, 0, &path) != AI_SUCCESS)
            throw utils::RuntimeError("Failed to get aiTextureType_SPECULAR for idx 0");
        newMaterial->specularTexture = texture(baseDir + "/" + utils::String(path.C_Str()));
    }
    if (aiMaterial->GetTextureCount(aiTextureType_EMISSIVE) > 0)
    {
        aiString path;
        if (aiMaterial->GetTexture(aiTextureType_EMISSIVE, 0, &path) != AI_SUCCESS)
            throw utils::RuntimeError("Failed to get aiTextureType_EMISSIVE for idx 0");
        newMaterial->emissiveTexture = texture(baseDir + "/" + utils::String(path.C_Str()));
    }
    if (aiMaterial->GetTextureCount(aiTextureType_SHININESS) > 0)
    {
        aiString path;
        if (aiMaterial->GetTexture(aiTextureType_EMISSIVE, 0, &path) != AI_SUCCESS)
            throw utils::RuntimeError("Failed to get aiTextureType_EMISSIVE for idx 0");
        newMaterial->emissiveTexture = texture(baseDir + "/" + utils::String(path.C_Str()));
    }
    
    newMaterial->renderMethod = utils::SharedPtr<IRenderMethod>(new RenderMethod<VertexShader::universal3D, FragmentShader::universal>(m_api));

    return newMaterial;
}

utils::Array<Mesh> AssetManager::scene(const utils::String& filePath)
{
    if (m_scenes.contain(filePath))
        return m_scenes[filePath];

    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(filePath, POST_PROCESSING_FLAGS);
    if (scene == nullptr)
        throw utils::RuntimeError("fail to load the model using assimp");

    utils::Array<utils::SharedPtr<Material>> allMaterial;
    utils::Array<SubMesh> allMeshes;

    for (utils::uint32 i = 0; i < scene->mNumMaterials; i++)
        allMaterial.append(material(scene->mMaterials[i], filePath.substr(0, filePath.lastIndexOf('/'))));
    
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

        SubMesh newSubMesh;
        newSubMesh.name = aiMesh->mName.C_Str();
        newSubMesh.vertexBuffer = m_api->newVertexBuffer(vertices);
        newSubMesh.indexBuffer = m_api->newIndexBuffer(indices);
        newSubMesh.material = allMaterial[aiMesh->mMaterialIndex];
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

        for (utils::uint32 i = 0; aiNode->mNumMeshes > 1 && i < aiNode->mNumMeshes; i++)
            newSubMesh.childs.append(allMeshes[aiNode->mMeshes[i]]);

        for (utils::uint32 i = 0; i < aiNode->mNumChildren; i++)
            addNode(aiNode->mChildren[i], newSubMesh.childs, math::mat4x4(1.0f));
    };
    
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

    m_scenes.insert(filePath, output);
    return output;
}

AssetManager::AssetManager(const utils::SharedPtr<gfx::GraphicAPI>& api) : m_api(api)
{
}