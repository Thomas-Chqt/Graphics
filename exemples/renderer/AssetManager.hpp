/*
 * ---------------------------------------------------
 * AssetManager.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/07/01 09:28:44
 * ---------------------------------------------------
 */

#ifndef ASSETMANAGER_HPP
# define ASSETMANAGER_HPP

#include "Graphics/GraphicAPI.hpp"
#include "Graphics/IndexBuffer.hpp"
#include "Graphics/Texture.hpp"
#include "Graphics/VertexBuffer.hpp"
#include "Math/Matrix.hpp"
#include "Math/Vector.hpp"
#include "RenderMethod.hpp"
#include "UtilsCPP/Array.hpp"
#include "UtilsCPP/Dictionary.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "UtilsCPP/String.hpp"
#include "UtilsCPP/UniquePtr.hpp"
#include "assimp/material.h"

struct Material
{
    utils::String name;

    utils::SharedPtr<IRenderMethod> renderMethod;

    math::rgb ambientColor = WHITE3;
    utils::SharedPtr<gfx::Texture> ambientTexture;

    math::rgb diffuseColor = WHITE3;
    utils::SharedPtr<gfx::Texture> diffuseTexture;
    
    math::rgb specularColor;
    utils::SharedPtr<gfx::Texture> specularTexture;
    
    math::rgb emissiveColor;
    utils::SharedPtr<gfx::Texture> emissiveTexture;
    
    float shininess = 1.0f;
    utils::SharedPtr<gfx::Texture> shininessTexture;
};

struct SubMesh
{
    utils::String name;
    utils::SharedPtr<gfx::VertexBuffer> vertexBuffer;
    utils::SharedPtr<gfx::IndexBuffer> indexBuffer;
    utils::SharedPtr<Material> material;
    math::mat4x4 transform = math::mat4x4(1.0f);
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
    static inline void init(const utils::SharedPtr<gfx::GraphicAPI>& api) { s_instance = utils::UniquePtr<AssetManager>(new AssetManager(api)); }
    static inline AssetManager& shared() { return *s_instance; }
    static inline void terminate() { s_instance.clear(); }

    utils::SharedPtr<gfx::Texture> texture(const utils::String& filePath);
    utils::SharedPtr<Material> material(const utils::String& name);
    utils::SharedPtr<Material> material(aiMaterial*, const utils::String& baseDir);
    utils::Array<Mesh> scene(const utils::String& filePath);

    ~AssetManager() = default;

private:
    AssetManager(const utils::SharedPtr<gfx::GraphicAPI>&);
    
    static utils::UniquePtr<AssetManager> s_instance;

    utils::SharedPtr<gfx::GraphicAPI> m_api;
    utils::Dictionary<utils::String, utils::SharedPtr<gfx::Texture>> m_textures;
    utils::Array<utils::SharedPtr<Material>> m_materials;
    utils::Dictionary<utils::String, utils::Array<Mesh>> m_scenes;
};

#endif // ASSETMANAGER_HPP