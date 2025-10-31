/*
 * ---------------------------------------------------
 * Vertex.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/10/31 11:26:45
 * ---------------------------------------------------
 */

#ifndef VERTEX_HPP
#define VERTEX_HPP

#include <glm/glm.hpp>

namespace scop
{

struct Vertex
{
    glm::vec3 pos;
    glm::vec2 uv;
    glm::vec3 normal;
    glm::vec3 tangent;
};

}

#endif // VERTEX_HPP
