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

#include "Graphics/GraphicAPI.hpp"
#include "Graphics/IndexBuffer.hpp"
#include "Graphics/VertexBuffer.hpp"
#include "Math/Matrix.hpp"
#include "UtilsCPP/Array.hpp"
#include "UtilsCPP/String.hpp"
#include "UtilsCPP/SharedPtr.hpp"

struct SubMesh
{
    utils::String name;
    utils::SharedPtr<gfx::VertexBuffer> vertexBuffer;
    utils::SharedPtr<gfx::IndexBuffer> indexBuffer;
    math::mat4x4 transform = math::mat4x4(1.0f);
    utils::Array<SubMesh> childs;
};

struct Mesh
{
    utils::String name;
    utils::Array<SubMesh> subMeshes;
};

utils::Array<Mesh> loadMeshes(gfx::GraphicAPI&, const utils::String& filePath);

#endif // MESH_HPP