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
#include "UtilsCPP/Dictionary.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "UtilsCPP/String.hpp"
#include "UtilsCPP/UniquePtr.hpp"

struct Material
{
    utils::String name;

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
    MaterialLibrary(const MaterialLibrary&) = delete;
    MaterialLibrary(MaterialLibrary&&)      = delete;

    static inline void init() { s_instance = utils::UniquePtr<MaterialLibrary>(new MaterialLibrary()); }
    static inline MaterialLibrary& shared() { return *s_instance; }
    static inline void terminate() { s_instance.clear(); }

    inline utils::SharedPtr<Material>& newMaterial(const utils::String& name) { m_materials.insert(name, utils::SharedPtr<Material>()); return m_materials[name]; }
    inline bool contain(const utils::String& name) { return m_materials.contain(name); }
    inline utils::SharedPtr<Material>& getMaterial(const utils::String& name) { return m_materials[name]; }

    ~MaterialLibrary() = default;

private:
    MaterialLibrary() = default;

    static utils::UniquePtr<MaterialLibrary> s_instance;

    utils::Dictionary<utils::String, utils::SharedPtr<Material>> m_materials;

public:
    MaterialLibrary& operator = (const MaterialLibrary&) = delete;
    MaterialLibrary& operator = (MaterialLibrary&&)      = delete;
};

#endif // MATERIALLIBRARY_HPP