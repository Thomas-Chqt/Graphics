/*
 * ---------------------------------------------------
 * OpenGLBuffer.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/07/09 15:41:05
 * ---------------------------------------------------
 */

#ifndef OPENGLBUFFER_HPP
# define OPENGLBUFFER_HPP

#include "Graphics/Buffer.hpp"
#include "UtilsCPP/Types.hpp"
#include <GL/glew.h>

namespace gfx
{

class OpenGLBuffer : public Buffer
{
public:
    OpenGLBuffer()                    = delete;
    OpenGLBuffer(const OpenGLBuffer&) = delete;
    OpenGLBuffer(OpenGLBuffer&&)      = delete;
    
    OpenGLBuffer(const Buffer::Descriptor&);

    inline utils::uint64 size() override { return m_size; }

    inline GLuint bufferID() { return m_bufferID; }

    ~OpenGLBuffer() override;

private:
    GLuint m_bufferID;
    const utils::uint64 m_size;

public:
    OpenGLBuffer& operator = (const OpenGLBuffer&) = delete;
    OpenGLBuffer& operator = (OpenGLBuffer&&)      = delete;
};

}

#endif // OPENGLBUFFER_HPP