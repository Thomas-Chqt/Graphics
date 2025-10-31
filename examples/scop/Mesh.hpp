/*
 * ---------------------------------------------------
 * Mesh.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/10/31 11:08:14
 * ---------------------------------------------------
 */

#ifndef MESH_HPP
#define MESH_HPP

#include "Material.hpp"

#include <Graphics/Buffer.hpp>

#include <glm/glm.hpp>

#include <string>
#include <memory>
#include <vector>

namespace scop
{

struct SubMesh
{
    std::string name;
    glm::mat4x4 transform;
    std::shared_ptr<gfx::Buffer> vertexBuffer;
    std::shared_ptr<gfx::Buffer> indexBuffer;
    std::shared_ptr<Material> material;
    std::vector<SubMesh> subMeshes;
};

struct Mesh
{
    std::string name;
    std::vector<SubMesh> subMeshes;
};

}

#endif // MESH_HPP
