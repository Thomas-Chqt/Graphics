/*
 * ---------------------------------------------------
 * Mesh.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/07/06 17:50:10
 * ---------------------------------------------------
 */

#ifndef MESH_HPP
# define MESH_HPP

#include "Graphics/Buffer.hpp"
#include "Graphics/GraphicAPI.hpp"
#include "Math/Matrix.hpp"
#include "UtilsCPP/Array.hpp"
#include "UtilsCPP/String.hpp"
#include "UtilsCPP/SharedPtr.hpp"

struct Vertex
{
    math::vec3f pos;
    math::vec2f uv;
    math::vec3f normal;
    math::vec3f tangent;
    math::vec3f bitangent;
};

struct SubMesh
{
    utils::String name;
    utils::SharedPtr<gfx::Buffer> vertexBuffer;
    utils::SharedPtr<gfx::Buffer> indexBuffer;
    math::mat4x4 transform = math::mat4x4(1.0F);
    utils::Array<SubMesh> childs;
};

struct Mesh
{
    utils::String name;
    utils::Array<SubMesh> subMeshes;
};

utils::Array<Mesh> loadMeshes(gfx::GraphicAPI&, const utils::String& filePath);

#endif // MESH_HPP
