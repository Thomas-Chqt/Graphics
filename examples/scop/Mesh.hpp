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

#if !defined (SCOP_MANDATORY)
#include <glm/glm.hpp>
#else
#include "math/math.hpp"
#ifndef SCOP_MATH_GLM_ALIAS_DEFINED
#define SCOP_MATH_GLM_ALIAS_DEFINED
namespace glm = scop::math;
#endif
#endif

#include <string>
#include <memory>
#include <vector>

namespace scop
{

struct Vertex
{
    glm::vec3 pos;
    glm::vec2 uv;
    glm::vec3 normal;
    glm::vec3 tangent;
};

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
    glm::vec3 bBoxMin;
    glm::vec3 bBoxMax;
    std::vector<SubMesh> subMeshes;
};

}

#endif // MESH_HPP
