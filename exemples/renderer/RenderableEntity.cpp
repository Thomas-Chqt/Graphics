/*
 * ---------------------------------------------------
 * RenderableEntity.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/22 13:43:48
 * ---------------------------------------------------
 */

#include "Entity.hpp"
#include "RenderMethod.hpp"
#include "UtilsCPP/RuntimeError.hpp"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "stb_image.h"

static Material makeMaterial(aiMaterial* aiMaterial)
{
    aiColor3D ambiant, diffuse, specular, emissive;
    float shininess;
    
    aiMaterial->Get(AI_MATKEY_COLOR_AMBIENT, ambiant);
    aiMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
    aiMaterial->Get(AI_MATKEY_COLOR_SPECULAR, specular);
    aiMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, emissive);
    aiMaterial->Get(AI_MATKEY_SHININESS, shininess);
    return Material {
        math::rgb(ambiant.r, ambiant.g, ambiant.b),
        math::rgb(diffuse.r, diffuse.g, diffuse.b),
        math::rgb(specular.r, specular.g, specular.b),
        math::rgb(emissive.r, emissive.g, emissive.b),
        shininess
    };
}

static utils::SharedPtr<gfx::Texture> textureFromFile(const gfx::GraphicAPI& api, const utils::String& path)
{
    int width;
    int height;
    stbi_uc* imgBytes = stbi_load(path, &width, &height, nullptr, STBI_rgb_alpha);

    gfx::Texture::Descriptor textureDescriptor;
    textureDescriptor.width = width;
    textureDescriptor.height = height;

    utils::SharedPtr<gfx::Texture> texture = api.newTexture(textureDescriptor);
    texture->setBytes(imgBytes);

    stbi_image_free(imgBytes);

    return texture;
}

RenderableEntity::RenderableEntity(const gfx::GraphicAPI& api, const utils::String& filePath)
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

    subMeshes = utils::Array<RenderableEntity::SubMesh>(scene->mNumMeshes);

    for(utils::uint32 meshIndex = 0; meshIndex < scene->mNumMeshes; meshIndex++)
    {
        aiMesh* aiMesh = scene->mMeshes[meshIndex]; 

        utils::Array<Vertex> vertices(aiMesh->mNumVertices);
        utils::Array<utils::uint32> indices(aiMesh->mNumFaces * 3);

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

        subMeshes[meshIndex].vertexBuffer = api.newVertexBuffer(vertices);
        subMeshes[meshIndex].indexBuffer = api.newIndexBuffer(indices);
        
        aiMaterial* aiMaterial = scene->mMaterials[aiMesh->mMaterialIndex];
        subMeshes[meshIndex].material = makeMaterial(aiMaterial);

        if (aiMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0)
        {
            aiString path;
            if (aiMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &path) != AI_SUCCESS)
                throw utils::RuntimeError("Failed to get the texture");

            utils::String textureFilePath = filePath.substr(0, filePath.lastIndexOf('/')) + "/" + utils::String(path.data);

            subMeshes[meshIndex].diffuseTexture = textureFromFile(api, textureFilePath);
            subMeshes[meshIndex].renderMethod = utils::SharedPtr<RenderMethod>(new TexturedRenderMethod(api));
        }
        else
            subMeshes[meshIndex].renderMethod = utils::SharedPtr<RenderMethod>(new FlatColorRenderMethod(api));
    }

}
