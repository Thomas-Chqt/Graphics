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
    #include <cstddef>
    #include "Graphics/VertexBuffer.hpp"
#endif // __METAL_VERSION__

#include "../dependencies/Math/include/Math/Vector.hpp"

namespace gfx_test
{

struct Vertex_vertexBuffer
{
    float x;
    float y;
};

struct Vertex_triangle
{
    float x;
    float y;
};

struct Vertex_indexedShape
{
    float x;
    float y;
};

struct Vertex_APISwitch
{
    float x;
    float y;
};

struct Vertex_fragmentUniform
{
    float x;
    float y;
};

struct Vertex_flatColorCube
{
    float x;
    float y;
    float z;
};

struct Vertex_texturedSquare
{
    math::vec2f pos;
    math::vec2f uv;
};

struct Vertex_noClearBuffer
{
    math::vec2f pos;
};

}

#ifndef __METAL_VERSION__
namespace gfx
{

template<>
class VertexBuffer::Layout<gfx_test::Vertex_vertexBuffer> : public VertexBuffer::LayoutBase
{
public:
    #ifdef USING_OPENGL
    Layout()
    {
        m_elements.append({
            2,
            GL_FLOAT,
            GL_FALSE,
            sizeof(gfx_test::Vertex_vertexBuffer),
            (void*)0
        });
    }
    inline const utils::Array<Element>& getElements() const override { return m_elements; };
    #endif
    inline utils::uint64 getSize() const override { return sizeof(gfx_test::Vertex_vertexBuffer); };

#ifdef USING_OPENGL
private:
    utils::Array<Element> m_elements;
#endif
};

template<>
class VertexBuffer::Layout<gfx_test::Vertex_triangle> : public VertexBuffer::LayoutBase
{
public:
    #ifdef USING_OPENGL
    Layout()
    {
        m_elements.append({
            2,
            GL_FLOAT,
            GL_FALSE,
            sizeof(gfx_test::Vertex_triangle),
            (void*)0
        });
    }
    inline const utils::Array<Element>& getElements() const override { return m_elements; };
    #endif
    inline utils::uint64 getSize() const override { return sizeof(gfx_test::Vertex_triangle); };

#ifdef USING_OPENGL
private:
    utils::Array<Element> m_elements;
#endif
};

template<>
class VertexBuffer::Layout<gfx_test::Vertex_indexedShape> : public VertexBuffer::LayoutBase
{
public:
    #ifdef USING_OPENGL
    Layout()
    {
        m_elements.append({
            2,
            GL_FLOAT,
            GL_FALSE,
            sizeof(gfx_test::Vertex_indexedShape),
            (void*)0
        });
    }
    inline const utils::Array<Element>& getElements() const override { return m_elements; };
    #endif
    inline utils::uint64 getSize() const override { return sizeof(gfx_test::Vertex_indexedShape); };

#ifdef USING_OPENGL
private:
    utils::Array<Element> m_elements;
#endif
};

template<>
class VertexBuffer::Layout<gfx_test::Vertex_APISwitch> : public VertexBuffer::LayoutBase
{
public:
    #ifdef USING_OPENGL
    Layout()
    {
        m_elements.append({
            2,
            GL_FLOAT,
            GL_FALSE,
            sizeof(gfx_test::Vertex_APISwitch),
            (void*)0
        });
    }
    inline const utils::Array<Element>& getElements() const override { return m_elements; };
    #endif
    inline utils::uint64 getSize() const override { return sizeof(gfx_test::Vertex_APISwitch); };

#ifdef USING_OPENGL
private:
    utils::Array<Element> m_elements;
#endif
};

template<>
class VertexBuffer::Layout<gfx_test::Vertex_fragmentUniform> : public VertexBuffer::LayoutBase
{
public:
    #ifdef USING_OPENGL
    Layout()
    {
        m_elements.append({
            2,
            GL_FLOAT,
            GL_FALSE,
            sizeof(gfx_test::Vertex_fragmentUniform),
            (void*)0
        });
    }
    inline const utils::Array<Element>& getElements() const override { return m_elements; };
    #endif
    inline utils::uint64 getSize() const override { return sizeof(gfx_test::Vertex_fragmentUniform); };

#ifdef USING_OPENGL
private:
    utils::Array<Element> m_elements;
#endif
};

template<>
class VertexBuffer::Layout<gfx_test::Vertex_flatColorCube> : public VertexBuffer::LayoutBase
{
public:
    #ifdef USING_OPENGL
    Layout()
    {
        m_elements.append({
            3,
            GL_FLOAT,
            GL_FALSE,
            sizeof(gfx_test::Vertex_flatColorCube),
            (void*)0
        });
    }
    inline const utils::Array<Element>& getElements() const override { return m_elements; };
    #endif
    inline utils::uint64 getSize() const override { return sizeof(gfx_test::Vertex_flatColorCube); };

#ifdef USING_OPENGL
private:
    utils::Array<Element> m_elements;
#endif
};

template<>
class VertexBuffer::Layout<gfx_test::Vertex_texturedSquare> : public VertexBuffer::LayoutBase
{
public:
    #ifdef USING_OPENGL
    Layout()
    {
        m_elements.append({
            2,
            GL_FLOAT,
            GL_FALSE,
            sizeof(gfx_test::Vertex_texturedSquare),
            (void*)0
        });

        m_elements.append({
            2,
            GL_FLOAT,
            GL_FALSE,
            sizeof(gfx_test::Vertex_texturedSquare),
            (void*)offsetof(gfx_test::Vertex_texturedSquare, uv)
        });
    }
    inline const utils::Array<Element>& getElements() const override { return m_elements; };
    #endif
    inline utils::uint64 getSize() const override { return sizeof(gfx_test::Vertex_texturedSquare); };

#ifdef USING_OPENGL
private:
    utils::Array<Element> m_elements;
#endif
};

template<>
class VertexBuffer::Layout<gfx_test::Vertex_noClearBuffer> : public VertexBuffer::LayoutBase
{
public:
    #ifdef USING_OPENGL
    Layout()
    {
        m_elements.append({
            2,
            GL_FLOAT,
            GL_FALSE,
            sizeof(gfx_test::Vertex_noClearBuffer),
            (void*)0
        });
    }
    inline const utils::Array<Element>& getElements() const override { return m_elements; };
    #endif
    inline utils::uint64 getSize() const override { return sizeof(gfx_test::Vertex_noClearBuffer); };

#ifdef USING_OPENGL
private:
    utils::Array<Element> m_elements;
#endif
};


}
#endif // __METAL_VERSION__

#endif // VERTEX_HPP