/*
 * ---------------------------------------------------
 * OpenGLIndexBuffer.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/22 12:22:47
 * ---------------------------------------------------
 */

#include "GraphicAPI/OpenGL/OpenGLIndexBuffer.hpp"
#include "UtilsCPP/Array.hpp"
#include "UtilsCPP/Types.hpp"

using utils::uint32;
using utils::Array;

namespace gfx
{

OpenGLIndexBuffer::OpenGLIndexBuffer(const Array<uint32>& indices)
{
    glGenBuffers(1, &m_indexBufferID);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32) * indices.length(), (const uint32*)indices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    m_indexCount = indices.length();
}

OpenGLIndexBuffer::~OpenGLIndexBuffer()
{
    glDeleteBuffers(1, &m_indexBufferID);
} 

}