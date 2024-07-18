/*
 * ---------------------------------------------------
 * AssetManager.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/07/18 14:47:15
 * ---------------------------------------------------
 */

#ifndef ASSETMANAGER_HPP
# define ASSETMANAGER_HPP

#include "Graphics/Buffer.hpp"
#include "Graphics/GraphicAPI.hpp"
#include "Graphics/Texture.hpp"
#include "Math/Matrix.hpp"
#include "RenderMethod.hpp"
#include "UtilsCPP/Array.hpp"
#include "UtilsCPP/Dictionary.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "UtilsCPP/String.hpp"
#include "UtilsCPP/UniquePtr.hpp"
#include "assimp/material.h"

struct Material
{
    struct ColorInput
    {
        ColorInput(math::rgb color) : value(color) {}

        math::rgb value;
        utils::SharedPtr<gfx::Texture> texture;
    };

    utils::String name;
    utils::SharedPtr<RenderMethod> renderMethod;
    ColorInput diffuse  = WHITE3;
    ColorInput specular = BLACK3;
    ColorInput emissive = BLACK3;
    float shininess     = 1.0F;
    utils::SharedPtr<gfx::Texture> normalMap;
};

struct SubMesh
{
    utils::String name;
    utils::SharedPtr<gfx::Buffer> vertexBuffer;
    utils::SharedPtr<gfx::Buffer> indexBuffer;
    math::mat4x4 transform = math::mat4x4(1.0F);
    utils::SharedPtr<Material> material;
    utils::Array<SubMesh> childs;
};

struct Mesh
{
    utils::String name;
    utils::Array<SubMesh> subMeshes;
};

class AssetManager
{
public:
    AssetManager()                    = delete;
    AssetManager(const AssetManager&) = delete;
    AssetManager(AssetManager&&)      = delete;
    
    static inline void init(const utils::SharedPtr<gfx::GraphicAPI>& api) { s_sharedInstance = utils::UniquePtr<AssetManager>(new AssetManager(api)); }
    static inline AssetManager& shared() { return *s_sharedInstance; }
    static inline void terminate() { s_sharedInstance.clear(); }

    const utils::SharedPtr<RenderMethod>& renderMethod(const utils::String& name);

    const utils::SharedPtr<gfx::Texture>& texture(const utils::String& filepath);

    const utils::SharedPtr<Material>& material(const utils::String& name);
    const utils::SharedPtr<Material>& material(aiMaterial* aiMaterial, const utils::String& baseDir);

    template<typename VERTEX>
    const utils::Array<Mesh>& mesh(const utils::String& filePath);

    ~AssetManager() = default;

private:
    AssetManager(utils::SharedPtr<gfx::GraphicAPI>);

    static utils::UniquePtr<AssetManager> s_sharedInstance;

    utils::SharedPtr<gfx::GraphicAPI> m_graphicAPI;

    utils::Dictionary<utils::String, utils::Array<Mesh>> m_meshScenes;
    utils::Dictionary<utils::String, utils::SharedPtr<Material>> m_materials;
    utils::Dictionary<utils::String, utils::SharedPtr<gfx::Texture>> m_textures;
    utils::Dictionary<utils::String, utils::SharedPtr<RenderMethod>> m_renderMethods;

public:
    AssetManager& operator = (const AssetManager&) = delete;
    AssetManager& operator = (AssetManager&&)      = delete;
};

#endif // ASSETMANAGER_HPP