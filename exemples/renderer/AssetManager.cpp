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
#include "UtilsCPP/RuntimeError.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
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
    if (m_materials.contain(name))
        return m_materials[name];
    utils::SharedPtr<Material> newMaterial(new Material);
    newMaterial->name = name;
    m_materials.insert(name, newMaterial);
    return newMaterial;
}

utils::SharedPtr<Material> AssetManager::material(aiMaterial* aiMaterial, const utils::String& baseDir)
{
    if (m_materials.contain(aiMaterial->GetName().C_Str()))
        return m_materials[aiMaterial->GetName().C_Str()];

    utils::SharedPtr<Material> newMaterial = material(aiMaterial->GetName().C_Str());

    newMaterial->name = utils::String(aiMaterial->GetName().C_Str());

    if (aiMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0)
    {
        // newMaterial->renderMethod = utils::SharedPtr<IRenderMethod>(new RenderMethod<Shader::universal3D, Shader::baseTexture>(m_api));
        newMaterial->renderMethod = utils::SharedPtr<IRenderMethod>(new RenderMethod<Shader::universal3D, Shader::baseColor>(m_api));
        aiString path;
        if (aiMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &path) != AI_SUCCESS)
            throw utils::RuntimeError("Failed to get aiTextureType_DIFFUSE for idx 0");
        // newMaterial->baseTexture = texture(baseDir + "/" + utils::String(path.C_Str()));
    }
    else
    {
        newMaterial->renderMethod = utils::SharedPtr<IRenderMethod>(new RenderMethod<Shader::universal3D, Shader::baseColor>(m_api));
        aiColor3D diffuse  = {1, 1, 1};
        aiMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
        newMaterial->baseColor = { diffuse.r, diffuse.g, diffuse.b };
    }

    aiColor3D specular = {0, 0, 0};
    aiMaterial->Get(AI_MATKEY_COLOR_SPECULAR, specular);
    newMaterial->specularColor = { specular.r, specular.g, specular.b };

    aiColor3D emissive = {0, 0, 0};
    aiMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, emissive);
    newMaterial->emissiveColor = { emissive.r, emissive.g, emissive.b };

    aiMaterial->Get(AI_MATKEY_SHININESS, newMaterial->shininess);

    return newMaterial;
}

Mesh AssetManager::mesh(const utils::String& filePath)
{
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(filePath, POST_PROCESSING_FLAGS);
    if (scene == nullptr)
        throw utils::RuntimeError("fail to load the model using assimp");

    utils::Array<utils::SharedPtr<Material>> allMaterial;
    utils::Array<Mesh> allMeshes;

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

        allMeshes.append({
            aiMesh->mName.C_Str(),
            math::mat4x4(1.0f),
            m_api->newVertexBuffer(vertices),
            m_api->newIndexBuffer(indices),
            allMaterial[aiMesh->mMaterialIndex],
            utils::Array<Mesh>()
        });
    }

    Mesh output;

    utils::Func<Mesh(aiNode*)> meshFromNode = [&](aiNode* aiNode) {
        Mesh newMesh;

        newMesh.name = aiNode->mName.C_Str();
        
        newMesh.modelMatrix = math::mat4x4(
            aiNode->mTransformation.a1, aiNode->mTransformation.a2, aiNode->mTransformation.a3, aiNode->mTransformation.a4,
            aiNode->mTransformation.b1, aiNode->mTransformation.b2, aiNode->mTransformation.b3, aiNode->mTransformation.b4,
            aiNode->mTransformation.c1, aiNode->mTransformation.c2, aiNode->mTransformation.c3, aiNode->mTransformation.c4,
            aiNode->mTransformation.d1, aiNode->mTransformation.d2, aiNode->mTransformation.d3, aiNode->mTransformation.d4
        );

        for (utils::uint32 i = 0; i < aiNode->mNumMeshes; i++)
            newMesh.childs.append(allMeshes[aiNode->mMeshes[i]]);

        for (utils::uint32 i = 0; i < aiNode->mNumChildren; i++)
            newMesh.childs.append(meshFromNode(aiNode->mChildren[i]));

        return newMesh;
    };

    output = meshFromNode(scene->mRootNode);
    return output;
}

AssetManager::AssetManager(const utils::SharedPtr<gfx::GraphicAPI>& api) : m_api(api)
{
}