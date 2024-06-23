/*
 * ---------------------------------------------------
 * Vertex.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/15 16:26:19
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
};

#ifndef __METAL_VERSION__
    template<>
    gfx::StructLayout gfx::getLayout<Vertex>()
    {
        return {
            { "pos", Type::vec3f, (void*)0 },
            { "uv", Type::vec2f, (void*)offsetof(Vertex, uv) },
        };
    }
#endif

#endif // VERTEX_HPP