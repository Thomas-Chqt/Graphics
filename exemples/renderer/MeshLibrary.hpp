/*
 * ---------------------------------------------------
 * MeshLibrary.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/24 15:07:00
 * ---------------------------------------------------
 */

#ifndef LIBRARY_HPP
# define LIBRARY_HPP

#include "Graphics/GraphicAPI.hpp"
#include "Graphics/IndexBuffer.hpp"
#include "Graphics/VertexBuffer.hpp"
#include "MaterialLibrary.hpp"
#include "UtilsCPP/Array.hpp"
#include "UtilsCPP/Dictionary.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "UtilsCPP/String.hpp"
#include "UtilsCPP/UniquePtr.hpp"

struct SubMesh
{
    utils::SharedPtr<gfx::VertexBuffer> vertexBuffer;
    utils::SharedPtr<gfx::IndexBuffer> indexBuffer;
    utils::SharedPtr<Material> material;
};

struct Mesh
{
    utils::String name;
    utils::Array<SubMesh> subMeshes;
};

class MeshLibrary
{
public:
    MeshLibrary(const MeshLibrary&) = delete;
    MeshLibrary(MeshLibrary&&)      = delete;

    static inline void init(const utils::SharedPtr<gfx::GraphicAPI>& api) { s_instance = utils::UniquePtr<MeshLibrary>(new MeshLibrary(api)); }
    static inline MeshLibrary& shared() { return *s_instance; }
    static inline void terminate() { s_instance.clear(); }

    Mesh meshFromFile(const utils::String& filePath);

    ~MeshLibrary() = default;

private:
    MeshLibrary(const utils::SharedPtr<gfx::GraphicAPI>& api);

    static utils::UniquePtr<MeshLibrary> s_instance;

    utils::SharedPtr<gfx::GraphicAPI> m_api;
    utils::Dictionary<utils::String, Mesh> m_meshes;

public:
    MeshLibrary& operator = (const MeshLibrary&) = delete;
    MeshLibrary& operator = (MeshLibrary&&)      = delete;

};

#endif // LIBRARY_HPP