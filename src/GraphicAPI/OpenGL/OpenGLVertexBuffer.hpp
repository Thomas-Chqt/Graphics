/*
 * ---------------------------------------------------
 * OpenGLVertexBuffer.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/20 14:27:51
 * ---------------------------------------------------
 */

#ifndef OPENGLVERTEXBUFFER_HPP
# define OPENGLVERTEXBUFFER_HPP

#include "Graphics/VertexBuffer.hpp"
#include "UtilsCPP/Types.hpp"
#include <GL/glew.h>

namespace gfx
{

class OpenGLVertexBuffer : public VertexBuffer
{
public:
    OpenGLVertexBuffer()                          = delete;
    OpenGLVertexBuffer(const OpenGLVertexBuffer&) = delete;
    OpenGLVertexBuffer(OpenGLVertexBuffer&&)      = delete;

    OpenGLVertexBuffer(void* data, utils::uint64 size, const utils::Array<VertexBuffer::LayoutElement>& layout);

    inline GLuint vertexArrayID() { return m_vertexArrayID; }
    
    ~OpenGLVertexBuffer() override;

private:
    GLuint m_vertexArrayID;
    GLuint m_vertexBufferID;

public:
    OpenGLVertexBuffer& operator = (const OpenGLVertexBuffer&) = delete;
    OpenGLVertexBuffer& operator = (OpenGLVertexBuffer&&)      = delete;
};

}

#endif // OPENGLVERTEXBUFFER_HPP