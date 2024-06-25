/*
 * ---------------------------------------------------
 * Vertex.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/21 13:57:25
 * ---------------------------------------------------
 */

#ifndef VERTEX_HPP
# define VERTEX_HPP

#include "Math/Vector.hpp"

#ifndef __METAL_VERSION__
    #include "Graphics/StructLayout.hpp"
#endif

struct Vertex
{
    math::vec3f pos;
    math::vec2f uv;
    math::vec3f normal;
};

#ifndef __METAL_VERSION__
template<>
inline gfx::StructLayout gfx::getLayout<Vertex>()
{
    return {
        { "pos",    Type::vec3f, (void*)offsetof(Vertex, pos)    },
        { "uv",     Type::vec2f, (void*)offsetof(Vertex, uv)     },
        { "normal", Type::vec3f, (void*)offsetof(Vertex, normal) }
    };
}

#endif

#endif // VERTEX_HPP