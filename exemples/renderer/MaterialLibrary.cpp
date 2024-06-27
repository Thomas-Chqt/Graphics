/*
 * ---------------------------------------------------
 * MaterialLibrary.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/24 16:01:02
 * ---------------------------------------------------
 */

#include "MaterialLibrary.hpp"
#include "Math/Vector.hpp"
#include "TextureLibrary.hpp"
#include "UtilsCPP/RuntimeError.hpp"
#include "UtilsCPP/String.hpp"
#include "assimp/material.h"
#include "assimp/types.h"

utils::UniquePtr<MaterialLibrary> MaterialLibrary::s_instance;

const utils::SharedPtr<Material>& MaterialLibrary::newEmptyMaterial()
{
    utils::SharedPtr<Material> newMaterial = utils::SharedPtr<Material>(new Material());
    newMaterial->name = "material " + utils::String::fromUInt(m_materials.size());
    newMaterial->renderMethod = utils::SharedPtr<IRenderMethod>(new RenderMethod<Shader::universal3D, Shader::baseColor>(m_api));
    m_materials.insert(newMaterial->name, newMaterial);
    return m_materials[newMaterial->name];
}

const utils::SharedPtr<Material>& MaterialLibrary::materialFromAiMaterial(aiMaterial* aiMaterial, const utils::String& baseDir)
{
    if (m_materials.contain(utils::String(aiMaterial->GetName().C_Str())))
        return m_materials[utils::String(aiMaterial->GetName().C_Str())];
        
    utils::SharedPtr<Material> newMaterial = utils::SharedPtr<Material>(new Material());

    if (utils::String(aiMaterial->GetName().C_Str()).isEmpty() == false)
        newMaterial->name = utils::String(aiMaterial->GetName().C_Str());

    if (aiMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0)
    {
        newMaterial->renderMethod = utils::SharedPtr<IRenderMethod>(new RenderMethod<Shader::universal3D, Shader::baseTexture>(m_api));
        aiString path;
        if (aiMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &path) != AI_SUCCESS)
            throw utils::RuntimeError("Failed to get aiTextureType_DIFFUSE for idx 0");
        newMaterial->baseTexture = TextureLibrary::shared().textureFromFile(baseDir + "/" + utils::String(path.C_Str()));
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

    m_materials.insert(newMaterial->name, newMaterial);
    return m_materials[newMaterial->name];
}


MaterialLibrary::MaterialLibrary(const utils::SharedPtr<gfx::GraphicAPI>& api) : m_api(api)
{
}