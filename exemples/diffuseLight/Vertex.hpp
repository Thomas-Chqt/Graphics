/*
 * ---------------------------------------------------
 * Vertex.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/16 07:42:18
 * ---------------------------------------------------
 */

#ifndef VERTEX_HPP
# define VERTEX_HPP

#include "Math/Vector.hpp"

#ifndef __METAL_VERSION__
    #include "Graphics/VertexBuffer.hpp"
    #include "UtilsCPP/Array.hpp"
#endif

struct Vertex
{
    math::vec3f pos;
    math::vec2f uv;
    math::vec3f normal;
};

#ifndef __METAL_VERSION__
    template<>
    inline utils::Array<gfx::VertexBuffer::LayoutElement> gfx::VertexBuffer::getLayout<Vertex>()
    {
        return {
            { 3, Type::FLOAT, false, sizeof(Vertex), (void*)offsetof(Vertex, pos)    },
            { 2, Type::FLOAT, false, sizeof(Vertex), (void*)offsetof(Vertex, uv)     },
            { 3, Type::FLOAT, false, sizeof(Vertex), (void*)offsetof(Vertex, normal) }
        };
    }
#endif

#endif // VERTEX_HPP