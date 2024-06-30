/*
 * ---------------------------------------------------
 * Mesh.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/30 13:34:21
 * ---------------------------------------------------
 */

#ifndef MESH_HPP
# define MESH_HPP

#include "Graphics/IndexBuffer.hpp"
#include "Graphics/VertexBuffer.hpp"
#include "MaterialLibrary.hpp"
#include "Math/Matrix.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "UtilsCPP/String.hpp"

struct Mesh
{
    utils::String name;

    math::mat4x4 modelMatrix;

    utils::SharedPtr<gfx::VertexBuffer> vertexBuffer;
    utils::SharedPtr<gfx::IndexBuffer> indexBuffer;
    utils::SharedPtr<Material> material;

    utils::Array<Mesh> childs;
};

#endif // MESH_HPP