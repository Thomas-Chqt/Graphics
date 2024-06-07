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

#ifndef __METAL_VERSION__
    #include <cstddef>
    #include "Graphics/VertexBuffer.hpp"
#endif // __METAL_VERSION__

#include "Math/Vector.hpp"

struct Vertex
{
    math::vec2f pos;
    math::vec2f uv;
};

#ifndef __METAL_VERSION__
template<>
class gfx::VertexBuffer::Layout<Vertex> : public gfx::VertexBuffer::LayoutBase
{
public:
    #ifdef GFX_OPENGL_ENABLED
        Layout()
        {
            m_elements.append({ 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0 });
            m_elements.append({ 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv) });
        }
        inline const utils::Array<Element>& getElements() const override { return m_elements; };
    #endif
    inline utils::uint64 getSize() const override { return sizeof(Vertex); };

#ifdef GFX_OPENGL_ENABLED
private:
    utils::Array<Element> m_elements;
#endif
};
#endif // __METAL_VERSION__

#endif // VERTEX_HPP