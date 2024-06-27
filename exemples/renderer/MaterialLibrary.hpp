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
#include "RenderMethod.hpp"
#include "UtilsCPP/Array.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "UtilsCPP/String.hpp"
#include "UtilsCPP/UniquePtr.hpp"
#include "assimp/material.h"

struct Material
{
    utils::String name;

    utils::SharedPtr<IRenderMethod> renderMethod;

    math::rgb baseColor = WHITE3;
    utils::SharedPtr<gfx::Texture> baseTexture;
    
    math::rgb specularColor;
    utils::SharedPtr<gfx::Texture> specularTexture;
    
    math::rgb emissiveColor;
    utils::SharedPtr<gfx::Texture> emissiveTexture;
    
    float shininess = 1.0f;
};

class MaterialLibrary
{
public:
    using Iterator = utils::Array<utils::SharedPtr<Material>>::Iterator;

public:
    MaterialLibrary(const MaterialLibrary&) = delete;
    MaterialLibrary(MaterialLibrary&&)      = delete;

    static inline void init(const utils::SharedPtr<gfx::GraphicAPI>& api) { s_instance = utils::UniquePtr<MaterialLibrary>(new MaterialLibrary(api)); }
    static inline MaterialLibrary& shared() { return *s_instance; }
    static inline void terminate() { s_instance.clear(); }

    const utils::SharedPtr<Material>& newEmptyMaterial();
    const utils::SharedPtr<Material>& materialFromAiMaterial(aiMaterial*);

    inline Iterator begin() { return m_materials.begin(); }
    inline Iterator end() { return m_materials.end(); }

    ~MaterialLibrary() = default;

private:
    MaterialLibrary(const utils::SharedPtr<gfx::GraphicAPI>& api);

    static utils::UniquePtr<MaterialLibrary> s_instance;

    utils::SharedPtr<gfx::GraphicAPI> m_api;
    utils::Array<utils::SharedPtr<Material>> m_materials;

public:
    MaterialLibrary& operator = (const MaterialLibrary&) = delete;
    MaterialLibrary& operator = (MaterialLibrary&&)      = delete;
};

#endif // MATERIALLIBRARY_HPP