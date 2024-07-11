/*
 * ---------------------------------------------------
 * OpenGLBuffer.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/07/09 16:14:58
 * ---------------------------------------------------
 */

#include "GraphicAPI/OpenGL/OpenGLBuffer.hpp"
#include "Graphics/Error.hpp"

#define GL_CALL(x) { x; GLenum __err__ = glGetError(); if (__err__ != GL_NO_ERROR) throw OpenGLCallError(__err__); }

namespace gfx
{

OpenGLBuffer::OpenGLBuffer(const Buffer::Descriptor& descriptor) : m_size(descriptor.size)
{
    GL_CALL(glGenBuffers(1, &m_bufferID))

    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_bufferID))
    {
        GL_CALL(glBufferData(GL_ARRAY_BUFFER, descriptor.size, descriptor.initialData, GL_DYNAMIC_COPY))
    }
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0))
}

OpenGLBuffer::~OpenGLBuffer()
{
    glDeleteBuffers(1, &m_bufferID);
}

}