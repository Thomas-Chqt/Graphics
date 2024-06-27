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

OpenGLVertexBuffer::OpenGLVertexBuffer(void* data, utils::uint64 count, utils::uint32 size, const StructLayout& layout)
{
    glGenVertexArrays(1, &m_vertexArrayID);
    glGenBuffers(1, &m_vertexBufferID);

    glBindVertexArray(m_vertexArrayID);

    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, count * size, data, GL_STATIC_DRAW);

    for (uint32 i = 0; i < layout.length(); i++)
    {
        const auto& el = layout[i];
        glEnableVertexAttribArray(i);
        switch (el.type)
        {
        case Type::Uint32:
            glVertexAttribPointer(i, 1, GL_UNSIGNED_INT, GL_FALSE, size, el.offset);
            break;
        case Type::Float:
            glVertexAttribPointer(i, 1, GL_FLOAT, GL_FALSE, size, el.offset);
            break;
        case Type::vec2f:
            glVertexAttribPointer(i, 2, GL_FLOAT, GL_FALSE, size, el.offset);
            break;
        case Type::vec3f:
            glVertexAttribPointer(i, 3, GL_FLOAT, GL_FALSE, size, el.offset);
            break;
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

OpenGLVertexBuffer::~OpenGLVertexBuffer()
{
    glDeleteBuffers(1, &m_vertexBufferID);
    glDeleteVertexArrays(1, &m_vertexArrayID);
}

}