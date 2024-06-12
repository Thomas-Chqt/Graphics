/*
 * ---------------------------------------------------
 * OpenGLFrameBuffer.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/12 10:38:24
 * ---------------------------------------------------
 */

#ifndef OPENGLFRAMEBUFFER_HPP
# define OPENGLFRAMEBUFFER_HPP

#include "GraphicAPI/OpenGL/OpenGLTexture.hpp"
#include "Graphics/FrameBuffer.hpp"
#include "GL/glew.h"

namespace gfx
{

class OpenGLFrameBuffer : public FrameBuffer
{
public:
    OpenGLFrameBuffer(const OpenGLFrameBuffer&) = delete;
    OpenGLFrameBuffer(OpenGLFrameBuffer&&)      = delete;

    virtual void useForRendering() = 0;
    
    virtual ~OpenGLFrameBuffer() = default;

protected:
    OpenGLFrameBuffer() = default;

public:
    OpenGLFrameBuffer& operator = (const OpenGLFrameBuffer&) = delete;
    OpenGLFrameBuffer& operator = (OpenGLFrameBuffer&&)      = delete;
};

class OpenGLScreenFrameBuffer : public OpenGLFrameBuffer
{
public:
    OpenGLScreenFrameBuffer()                               = default;
    OpenGLScreenFrameBuffer(const OpenGLScreenFrameBuffer&) = delete;
    OpenGLScreenFrameBuffer(OpenGLScreenFrameBuffer&&)      = delete;

    void useForRendering() override;

    ~OpenGLScreenFrameBuffer() override = default;

public:
    OpenGLScreenFrameBuffer& operator = (const OpenGLScreenFrameBuffer&) = delete;
    OpenGLScreenFrameBuffer& operator = (OpenGLScreenFrameBuffer&&)      = delete;

};

class OpenGLTextureFrameBuffer : public OpenGLFrameBuffer
{
public:
    OpenGLTextureFrameBuffer()                                = delete;
    OpenGLTextureFrameBuffer(const OpenGLTextureFrameBuffer&) = delete;
    OpenGLTextureFrameBuffer(OpenGLTextureFrameBuffer&&)      = delete;

    OpenGLTextureFrameBuffer(OpenGLTexture&& colorTexture);

    void useForRendering() override;

    inline const OpenGLTexture& colorTexture() { return m_colorTexture; }

    ~OpenGLTextureFrameBuffer() override = default;

private:
    OpenGLTexture m_colorTexture;
    GLuint m_frameBufferID;

public:
    OpenGLTextureFrameBuffer& operator = (const OpenGLTextureFrameBuffer&) = delete;
    OpenGLTextureFrameBuffer& operator = (OpenGLTextureFrameBuffer&&)      = delete;

};

}

#endif // OPENGLFRAMEBUFFER_HPP