/*
 * ---------------------------------------------------
 * ObjParser.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/12/05 16:39:16
 * ---------------------------------------------------
 */

#ifndef OBJPARSER_HPP
#define OBJPARSER_HPP

#include "math/math.hpp"
#ifndef SCOP_MATH_GLM_ALIAS_DEFINED
#define SCOP_MATH_GLM_ALIAS_DEFINED
namespace glm = scop::math;
#endif

#include <vector>
#include <filesystem>
#include <cstdint>
#include <utility>
#include <optional>

namespace scop
{

struct ObjMesh
{
    std::vector<glm::vec3> positions;
    std::vector<uint32_t> indices;
    std::optional<std::vector<glm::vec3>> normals;
};

ObjMesh parseObjFile(const std::filesystem::path& path);

}

#endif // OBJPARSER_HPP
