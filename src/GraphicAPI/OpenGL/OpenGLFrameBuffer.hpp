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
#include "Graphics/Texture.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include <GL/glew.h>

namespace gfx
{

class OpenGLFrameBuffer : public FrameBuffer
{
public:
    OpenGLFrameBuffer()                         = delete;
    OpenGLFrameBuffer(const OpenGLFrameBuffer&) = delete;
    OpenGLFrameBuffer(OpenGLFrameBuffer&&)      = delete;

    OpenGLFrameBuffer(const utils::SharedPtr<Texture>& colorTexture = utils::SharedPtr<Texture>());

    void setColorTexture(const utils::SharedPtr<Texture>&) override;
    inline utils::SharedPtr<Texture> colorTexture() override { return m_colorTexture.staticCast<Texture>(); }

    inline utils::SharedPtr<OpenGLTexture> glColorTexture() { return m_colorTexture; }
    inline GLuint frameBufferID() { return m_frameBufferID; } 
    
    ~OpenGLFrameBuffer() override;

private:
    GLuint m_frameBufferID;
    utils::SharedPtr<OpenGLTexture> m_colorTexture;

public:
    OpenGLFrameBuffer& operator = (const OpenGLFrameBuffer&) = delete;
    OpenGLFrameBuffer& operator = (OpenGLFrameBuffer&&)      = delete;
};

}

#endif // OPENGLFRAMEBUFFER_HPP