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

Material::Material()
{
    m_useDiffuseTexture  = false;
    m_diffuseColor       = WHITE3;
    m_specularColor      = BLACK3;
    m_emissiveColor      = BLACK3;
    m_shininess          = 1.0;
}

Material::Material(aiMaterial* aiMaterial)
{
    m_name = utils::String(aiMaterial->GetName().C_Str());

    if (aiMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0)
    {
        aiString path;
        if (aiMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &path) != AI_SUCCESS)
            throw utils::RuntimeError("Failed to get aiTextureType_DIFFUSE for idx 0");

        setDiffuseTexture(TextureLibrary::shared().textureFromFile(utils::String(RESSOURCES_DIR) + "/" + utils::String(path.C_Str())));
    }
    else
    {
        aiColor3D diffuse  = {1, 1, 1};
        aiMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
        setDiffuseColor({diffuse.r, diffuse.g, diffuse.b});
    }

    aiColor3D specular = {0, 0, 0};
    aiMaterial->Get(AI_MATKEY_COLOR_SPECULAR, specular);
    m_specularColor = {specular.r, specular.g, specular.b};

    aiColor3D emissive = {0, 0, 0};
    aiMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, emissive);
    m_emissiveColor = {emissive.r, emissive.g, emissive.b};

    aiMaterial->Get(AI_MATKEY_SHININESS, m_shininess);
}

void Material::setDiffuseColor(math::rgb color)
{
    m_useDiffuseTexture = false;
    m_diffuseColor = color;
    m_diffuseTexture.clear();
}

void Material::setDiffuseTexture(utils::SharedPtr<gfx::Texture> texture)
{
    m_useDiffuseTexture = true;
    m_diffuseTexture = texture;
}

shaderData::Material Material::getShaderMaterial()
{
    shaderData::Material shaderMat;
    if (m_useDiffuseTexture == false)
    {
        shaderMat.ambiant = m_diffuseColor;
        shaderMat.diffuse = m_diffuseColor;
    }
    shaderMat.specular = m_specularColor;
    shaderMat.emissive = m_emissiveColor;
    shaderMat.shininess = m_shininess;
    shaderMat.useDiffuseMap = m_useDiffuseTexture;
    return shaderMat;
}

Material::~Material()
{
    if (m_useDiffuseTexture)
        m_diffuseTexture.clear();
}

const utils::SharedPtr<Material>& MaterialLibrary::newEmptyMaterial()
{
    m_materials.append(utils::SharedPtr<Material>(new Material()));
    if (m_materials.last()->name().isEmpty())
        m_materials.last()->setName("material " + utils::String::fromUInt(m_materials.length()));
    return m_materials.last();
}

const utils::SharedPtr<Material>& MaterialLibrary::materialFromAiMaterial(aiMaterial* aiMaterial)
{
    m_materials.append(utils::SharedPtr<Material>(new Material(aiMaterial)));
    if (m_materials.last()->name().isEmpty())
        m_materials.last()->setName("material " + utils::String::fromUInt(m_materials.length()));
    return m_materials.last();
}