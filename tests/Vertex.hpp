/*
 * ---------------------------------------------------
 * Vertex.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/21 17:59:57
 * ---------------------------------------------------
 */

#ifndef VERTEX_HPP
# define VERTEX_HPP

#ifndef __METAL_VERSION__
    #include "Graphics/VertexBuffer.hpp"
#endif // __METAL_VERSION__

namespace gfx_test
{
    struct Vertex
    {
        float x;
        float y;
    };
}

#ifndef __METAL_VERSION__
namespace gfx
{
    template<>
    class VertexBuffer::Layout<gfx_test::Vertex> : public VertexBuffer::LayoutBase
    {
    public:
        #ifdef USING_OPENGL
        Layout()
        {
            m_elements.append({
                .size = 2,
                .type = GL_FLOAT,
                .normalized = GL_FALSE,
                .stride = sizeof(gfx_test::Vertex),
                .pointer = (void*)0
            });
        }
        inline const utils::Array<Element>& getElements() const override { return m_elements; };
        #endif
        inline utils::uint64 getSize() const override { return sizeof(gfx_test::Vertex); };

    #ifdef USING_OPENGL
    private:
        utils::Array<Element> m_elements;
    #endif
    };
}
#endif // __METAL_VERSION__

#endif // VERTEX_HPP