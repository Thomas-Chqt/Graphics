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
    m_materials.append(utils::SharedPtr<Material>(new Material()));
    m_materials.last()->name = "material " + utils::String::fromUInt(m_materials.length());
    return m_materials.last();
}

const utils::SharedPtr<Material>& MaterialLibrary::materialFromAiMaterial(aiMaterial* aiMaterial)
{
    newEmptyMaterial();
    Material& newMaterial = *m_materials.last();

    if (utils::String(aiMaterial->GetName().C_Str()).isEmpty() == false)
        newMaterial.name = utils::String(aiMaterial->GetName().C_Str());

    if (aiMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0)
    {
        aiString path;
        if (aiMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &path) != AI_SUCCESS)
            throw utils::RuntimeError("Failed to get aiTextureType_DIFFUSE for idx 0");
        newMaterial.baseTexture = TextureLibrary::shared().textureFromFile(utils::String(RESSOURCES_DIR) + "/" + utils::String(path.C_Str()));
    }
    else
    {
        aiColor3D diffuse  = {1, 1, 1};
        aiMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
        newMaterial.baseColor = { diffuse.r, diffuse.g, diffuse.b };
    }

    aiColor3D specular = {0, 0, 0};
    aiMaterial->Get(AI_MATKEY_COLOR_SPECULAR, specular);
    newMaterial.specularColor = { specular.r, specular.g, specular.b };

    aiColor3D emissive = {0, 0, 0};
    aiMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, emissive);
    newMaterial.emissiveColor = { emissive.r, emissive.g, emissive.b };

    aiMaterial->Get(AI_MATKEY_SHININESS, newMaterial.shininess);

    return m_materials.last();
}