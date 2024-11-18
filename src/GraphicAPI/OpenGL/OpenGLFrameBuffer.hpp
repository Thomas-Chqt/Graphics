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
#include <glad/glad.h>

namespace gfx
{

class OpenGLFrameBuffer : public FrameBuffer
{
public:
    OpenGLFrameBuffer()                         = delete;
    OpenGLFrameBuffer(const OpenGLFrameBuffer&) = delete;
    OpenGLFrameBuffer(OpenGLFrameBuffer&&)      = delete;

    explicit OpenGLFrameBuffer(const FrameBuffer::Descriptor&);

    inline utils::uint32 width() override { return m_colorTexture->width(); }
    inline utils::uint32 height() override { return m_colorTexture->height(); }

    void setColorTexture(const utils::SharedPtr<Texture>&) override;
    inline utils::SharedPtr<Texture> colorTexture() override { return m_colorTexture.staticCast<Texture>(); }

    void setDepthTexture(const utils::SharedPtr<Texture>&) override;
    inline utils::SharedPtr<Texture> depthTexture() override { return m_depthTexture.staticCast<Texture>(); }

    inline utils::SharedPtr<OpenGLTexture> glColorTexture() { return m_colorTexture; }
    inline GLuint frameBufferID() const { return m_frameBufferID; }
    
    ~OpenGLFrameBuffer() override;

private:
    GLuint m_frameBufferID = 0;
    utils::SharedPtr<OpenGLTexture> m_colorTexture;
    utils::SharedPtr<OpenGLTexture> m_depthTexture;

public:
    OpenGLFrameBuffer& operator = (const OpenGLFrameBuffer&) = delete;
    OpenGLFrameBuffer& operator = (OpenGLFrameBuffer&&)      = delete;
};

}

#endif // OPENGLFRAMEBUFFER_HPP