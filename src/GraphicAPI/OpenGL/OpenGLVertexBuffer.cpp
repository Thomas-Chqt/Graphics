/*
 * ---------------------------------------------------
 * OpenGLVertexBuffer.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/20 14:28:04
 * ---------------------------------------------------
 */

#include "GraphicAPI/OpenGL/OpenGLVertexBuffer.hpp"
#include "Math/Vector.hpp"
#include "UtilsCPP/StructLayout.hpp"
#include "UtilsCPP/Types.hpp"
#include <GL/glew.h>

using utils::uint64;
using utils::uint32;

namespace gfx
{

OpenGLVertexBuffer::OpenGLVertexBuffer(const void* data, utils::uint64 count, const utils::StructLayout& layout)
{
    glGenVertexArrays(1, &m_vertexArrayID);
    glGenBuffers(1, &m_vertexBufferID);

    glBindVertexArray(m_vertexArrayID);

    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, count * utils::sizeOf(layout), data, GL_STATIC_DRAW);

    for (uint32 i = 0; i < layout.length(); i++)
    {
        const auto& el = layout[i];
        glEnableVertexAttribArray(i);
        switch (el.typeId)
        {
        case TYPEID_UINT:
            glVertexAttribPointer(i, el.count, GL_UNSIGNED_INT, GL_FALSE, utils::sizeOf(layout), (void*)(uint64)el.offset);
            break;
        case TYPEID_FLOAT:
            glVertexAttribPointer(i, el.count, GL_FLOAT, GL_FALSE, utils::sizeOf(layout), (void*)(uint64)el.offset);
            break;
        case TYPEID_VEC2F:
            glVertexAttribPointer(i, el.count * 2, GL_FLOAT, GL_FALSE, utils::sizeOf(layout), (void*)(uint64)el.offset);
            break;
        case TYPEID_VEC3F:
            glVertexAttribPointer(i, el.count * 3, GL_FLOAT, GL_FALSE, utils::sizeOf(layout), (void*)(uint64)el.offset);
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