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
#include "UtilsCPP/SharedPtr.hpp"
#include "UtilsCPP/Types.hpp"
#include <GL/glew.h>
#include "GraphicAPI/OpenGL/OpenGLGraphicAPI.hpp"

namespace gfx
{

class OpenGLVertexBuffer : public VertexBuffer
{
private:
    friend utils::SharedPtr<VertexBuffer> OpenGLGraphicAPI::newVertexBuffer(void* data, utils::uint64 size, const VertexBuffer::LayoutBase& layout);

public:
    OpenGLVertexBuffer()                          = delete;
    OpenGLVertexBuffer(const OpenGLVertexBuffer&) = delete;
    OpenGLVertexBuffer(OpenGLVertexBuffer&&)      = delete;

    inline GLuint vertexArrayID() { return m_vertexArrayID; }
    
    ~OpenGLVertexBuffer() override;

private:
    OpenGLVertexBuffer(void* data, utils::uint64 size, const LayoutBase& layout);

    GLuint m_vertexArrayID;
    GLuint m_vertexBufferID;


public:
    OpenGLVertexBuffer& operator = (const OpenGLVertexBuffer&) = delete;
    OpenGLVertexBuffer& operator = (OpenGLVertexBuffer&&)      = delete;
};

}

#endif // OPENGLVERTEXBUFFER_HPP