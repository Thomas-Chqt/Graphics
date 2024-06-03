/*
 * ---------------------------------------------------
 * OpenGLIndexBuffer.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/22 12:22:40
 * ---------------------------------------------------
 */

#include "GraphicAPI/OpenGL/OpenGLGraphicAPI.hpp"
#include "Graphics/IndexBuffer.hpp"
#include "UtilsCPP/Array.hpp"
#include "UtilsCPP/Types.hpp"
#include <GL/glew.h>

#ifndef OPENGLINDEXBUFFER_HPP
# define OPENGLINDEXBUFFER_HPP

namespace gfx
{

class OpenGLIndexBuffer : public IndexBuffer
{
private:
    friend utils::SharedPtr<IndexBuffer> OpenGLGraphicAPI::newIndexBuffer(const utils::Array<utils::uint32>&) const;

public:
    OpenGLIndexBuffer()                         = delete;
    OpenGLIndexBuffer(const OpenGLIndexBuffer&) = delete;
    OpenGLIndexBuffer(OpenGLIndexBuffer&&)      = delete;

    inline GLuint indexBufferID() { return m_indexBufferID; }
    inline utils::uint64 indexCount() override { return static_cast<utils::uint64>(m_indexCount); }

    ~OpenGLIndexBuffer() override;

private:
    OpenGLIndexBuffer(const utils::Array<utils::uint32>& indices);

    utils::Array<utils::uint32>::Size m_indexCount;
    GLuint m_indexBufferID;

public:
    OpenGLIndexBuffer& operator = (const OpenGLIndexBuffer&) = delete;
    OpenGLIndexBuffer& operator = (OpenGLIndexBuffer&&)      = delete;      
};

}

#endif // OPENGLINDEXBUFFER_HPP