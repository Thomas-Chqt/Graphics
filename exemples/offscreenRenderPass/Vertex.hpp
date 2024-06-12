/*
 * ---------------------------------------------------
 * Vertex.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/07 16:56:07
 * ---------------------------------------------------
 */

#ifndef VERTEX_HPP
# define VERTEX_HPP

#include "Math/Vector.hpp"

struct Vertex
{
    math::vec2f pos;
    math::vec2f uv;
};

#endif // VERTEX_HPP