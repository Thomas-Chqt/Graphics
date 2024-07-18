/*
 * ---------------------------------------------------
 * AssetManager.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/07/18 14:55:27
 * ---------------------------------------------------
 */

#include "AssetManager.hpp"
#include "RenderMethod.hpp"
#include "UtilsCPP/Array.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include <utility>
#include "Graphics/Buffer.hpp"
#include "Math/Vector.hpp"
#include "RenderMethod.hpp"
#include "UtilsCPP/RuntimeError.hpp"
#include "UtilsCPP/String.hpp"
#include "UtilsCPP/Types.hpp"
#include "UtilsCPP/UniquePtr.hpp"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "assimp/types.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define POST_PROCESSING_FLAGS         \
    aiProcess_JoinIdenticalVertices | \
    aiProcess_MakeLeftHanded        | \
    aiProcess_Triangulate           | \
    aiProcess_GenSmoothNormals      | \
    aiProcess_FixInfacingNormals    | \
    aiProcess_FlipUVs               | \
    aiProcess_FlipWindingOrder      | \
    aiProcess_CalcTangentSpace

utils::UniquePtr<AssetManager> AssetManager::s_sharedInstance;

const utils::SharedPtr<RenderMethod>& AssetManager::renderMethod(const utils::String& name)
{
    if (m_renderMethods.contain(name))
        return m_renderMethods[name];
    // if (name == utils::String("default"))
    return m_renderMethods.insert(name, utils::makeShared<DefaultRenderMethod>(m_graphicAPI).staticCast<RenderMethod>())->val;
}

const utils::SharedPtr<gfx::Texture>& AssetManager::texture(const utils::String& filepath)
{
    if (m_textures.contain(filepath))
        return m_textures[filepath];

    int width;
    int height;
    stbi_uc* imgBytes = stbi_load(filepath, &width, &height, nullptr, STBI_rgb_alpha);
    if (imgBytes == nullptr)
        throw utils::RuntimeError("fail to read texture at path: " + filepath);

    gfx::Texture::Descriptor textureDescriptor;
    textureDescriptor.width = width;
    textureDescriptor.height = height;

    utils::SharedPtr<gfx::Texture> texture = m_graphicAPI->newTexture(textureDescriptor);
    texture->replaceContent(imgBytes);

    stbi_image_free(imgBytes);

    return m_textures.insert(filepath, texture)->val;
}

const utils::SharedPtr<Material>& AssetManager::material(const utils::String& name)
{
    if (m_materials.contain(name))
        return m_materials[name];

    utils::SharedPtr<Material> newMaterial(new Material);
    newMaterial->name = name;
    
    return m_materials.insert(name, newMaterial)->val;
}


const utils::SharedPtr<Material>& AssetManager::material(aiMaterial* aiMaterial, const utils::String& baseDir)
{
    const utils::SharedPtr<Material>& newMaterial = material(aiMaterial->GetName().length == 0 ? "Material" : aiMaterial->GetName().C_Str());

    aiColor3D diffuseColor  = { 1, 1, 1 };
    aiColor3D specularColor = { 0, 0, 0 };
    aiColor3D emissiveColor = { 0, 0, 0 };

    aiMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor);
    newMaterial->diffuse.value  = { diffuseColor.r,  diffuseColor.g,  diffuseColor.b  };

    aiMaterial->Get(AI_MATKEY_COLOR_SPECULAR, specularColor);
    newMaterial->specular.value = { specularColor.r, specularColor.g, specularColor.b };

    aiMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, emissiveColor);
    newMaterial->emissive.value = { emissiveColor.r, emissiveColor.g, emissiveColor.b };

    aiMaterial->Get(AI_MATKEY_SHININESS, newMaterial->shininess);

    if (aiMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0)
    {
        aiString path;
        if (aiMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &path) != AI_SUCCESS)
            throw utils::RuntimeError("Failed to get aiTextureType_DIFFUSE for idx 0");
        newMaterial->diffuse.texture = texture(baseDir + "/" + utils::String(path.C_Str()));
    }
    if (aiMaterial->GetTextureCount(aiTextureType_SPECULAR) > 0)
    {
        aiString path;
        if (aiMaterial->GetTexture(aiTextureType_SPECULAR, 0, &path) != AI_SUCCESS)
            throw utils::RuntimeError("Failed to get aiTextureType_SPECULAR for idx 0");
        newMaterial->specular.texture = texture(baseDir + "/" + utils::String(path.C_Str()));
    }
    if (aiMaterial->GetTextureCount(aiTextureType_EMISSIVE) > 0)
    {
        aiString path;
        if (aiMaterial->GetTexture(aiTextureType_EMISSIVE, 0, &path) != AI_SUCCESS)
            throw utils::RuntimeError("Failed to get aiTextureType_EMISSIVE for idx 0");
        newMaterial->emissive.texture = texture(baseDir + "/" + utils::String(path.C_Str()));
    }
    
    newMaterial->renderMethod = renderMethod("default");

    return newMaterial;
}

template<>
const utils::Array<Mesh>& AssetManager::mesh<DefaultRenderMethod::Vertex>(const utils::String& filePath)
{
    if (m_meshScenes.contain(filePath))
        return m_meshScenes[filePath];
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(filePath, POST_PROCESSING_FLAGS);
    if (scene == nullptr)
        throw utils::RuntimeError("fail to load the model using assimp");

    utils::Array<SubMesh> allMeshes;
    utils::Array<utils::SharedPtr<Material>> allMaterials;

    for (utils::uint32 i = 0; i < scene->mNumMaterials; i++)
        allMaterials.append(material(scene->mMaterials[i], filePath.substr(0, filePath.lastIndexOf('/'))));

    for(utils::uint32 meshIndex = 0; meshIndex < scene->mNumMeshes; meshIndex++)
    {
        aiMesh* aiMesh = scene->mMeshes[meshIndex];

        SubMesh newSubMesh;

        newSubMesh.name = aiMesh->mName.C_Str();

        gfx::Buffer::Descriptor bufferDescriptor;
        bufferDescriptor.size = aiMesh->mNumVertices * sizeof(DefaultRenderMethod::Vertex);
        newSubMesh.vertexBuffer = m_graphicAPI->newBuffer(bufferDescriptor);
        auto* vertices = (DefaultRenderMethod::Vertex*)newSubMesh.vertexBuffer->mapContent();

        bufferDescriptor.size = aiMesh->mNumFaces * 3UL * sizeof(utils::uint32);
        newSubMesh.indexBuffer = m_graphicAPI->newBuffer(bufferDescriptor);
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
        newSubMesh.material = allMaterials[aiMesh->mMaterialIndex];

        allMeshes.append(newSubMesh);
    }

    utils::Dictionary<utils::String, utils::Array<Mesh>>::Iterator output = m_meshScenes.insert(filePath, utils::Array<Mesh>());

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
        output->val.append(newMesh);
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
        output->val.append(newMesh);
    }

    return output->val;
}

AssetManager::AssetManager(utils::SharedPtr<gfx::GraphicAPI> api) : m_graphicAPI(std::move(api))
{
}