/*
 * ---------------------------------------------------
 * MaterialLibrary.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/24 15:43:55
 * ---------------------------------------------------
 */

#ifndef MATERIALLIBRARY_HPP
# define MATERIALLIBRARY_HPP

#include "Graphics/Texture.hpp"
#include "Math/Vector.hpp"
#include "ShaderDatas.hpp"
#include "UtilsCPP/Array.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "UtilsCPP/String.hpp"
#include "UtilsCPP/UniquePtr.hpp"
#include "assimp/material.h"

class Material
{
public:
    Material();
    Material(aiMaterial*);

    inline const utils::String& name() { return m_name; }
    inline void setName(const utils::String& name) { m_name = name; }

    inline math::rgb diffuseColor() { return m_diffuseColor; };
    void setDiffuseColor(math::rgb);

    inline utils::SharedPtr<gfx::Texture> diffuseTexture() { return m_diffuseTexture; };
    void setDiffuseTexture(utils::SharedPtr<gfx::Texture>);

    inline math::rgb specularColor() { return m_specularColor; }
    inline void setSpecularColor(math::rgb color) { m_specularColor = color; }

    inline math::rgb emissiveColor() { return m_emissiveColor; }
    inline void setEmissiveColor(math::rgb color) { m_emissiveColor = color; }

    inline float shininess() { return m_shininess; }
    inline void setShininess(float f) { m_shininess = f; }

    shaderData::Material getShaderMaterial();

    ~Material();

private:
    utils::String m_name;

    bool m_useDiffuseTexture;
    math::rgb m_diffuseColor;
    utils::SharedPtr<gfx::Texture> m_diffuseTexture;

    math::rgb m_specularColor;
    math::rgb m_emissiveColor;
    float m_shininess;
};

class MaterialLibrary
{
public:
    MaterialLibrary(const MaterialLibrary&) = delete;
    MaterialLibrary(MaterialLibrary&&)      = delete;

    static inline void init() { s_instance = utils::UniquePtr<MaterialLibrary>(new MaterialLibrary()); }
    static inline MaterialLibrary& shared() { return *s_instance; }
    static inline void terminate() { s_instance.clear(); }

    const utils::SharedPtr<Material>& newEmptyMaterial();
    const utils::SharedPtr<Material>& materialFromAiMaterial(aiMaterial*);

    ~MaterialLibrary() = default;

private:
    MaterialLibrary() = default;

    static utils::UniquePtr<MaterialLibrary> s_instance;

    utils::Array<utils::SharedPtr<Material>> m_materials;

public:
    MaterialLibrary& operator = (const MaterialLibrary&) = delete;
    MaterialLibrary& operator = (MaterialLibrary&&)      = delete;
};

#endif // MATERIALLIBRARY_HPP