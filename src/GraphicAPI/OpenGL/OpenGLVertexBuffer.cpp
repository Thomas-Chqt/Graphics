/*
 * ---------------------------------------------------
 * OpenGLVertexBuffer.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/20 14:28:04
 * ---------------------------------------------------
 */

#include "GraphicAPI/OpenGL/OpenGLVertexBuffer.hpp"
#include "UtilsCPP/Types.hpp"
#include <GL/glew.h>

using utils::uint64;
using utils::uint32;

namespace gfx
{

OpenGLVertexBuffer::~OpenGLVertexBuffer()
{
    glDeleteBuffers(1, &m_vertexBufferID);
    glDeleteVertexArrays(1, &m_vertexArrayID);
}

OpenGLVertexBuffer::OpenGLVertexBuffer(void* data, uint64 size, const LayoutBase& layout)
{
    glGenVertexArrays(1, &m_vertexArrayID);
    glGenBuffers(1, &m_vertexBufferID);

    glBindVertexArray(m_vertexArrayID);

    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);

    for (uint32 i = 0; i < layout.getElements().length(); i++)
    {
        const LayoutBase::Element& el = layout.getElements()[i];
        glEnableVertexAttribArray(i);
        glVertexAttribPointer(i, el.size, el.type, el.normalized, el.stride, el.pointer);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

}