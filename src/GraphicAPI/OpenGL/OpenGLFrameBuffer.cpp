/*
 * ---------------------------------------------------
 * OpenGLFrameBuffer.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/12 11:07:55
 * ---------------------------------------------------
 */

#include "GraphicAPI/OpenGL/OpenGLFrameBuffer.hpp"
#include "GraphicAPI/OpenGL/OpenGLTexture.hpp"
#include "UtilsCPP/RuntimeError.hpp"
#include "UtilsCPP/SharedPtr.hpp"

#include <GL/glew.h>

namespace gfx
{

OpenGLFrameBuffer::OpenGLFrameBuffer(const utils::SharedPtr<Texture>& colorTexture)
{
    glGenFramebuffers(1, &m_frameBufferID);
    if (colorTexture)
        OpenGLFrameBuffer::setColorTexture(colorTexture);
}

void OpenGLFrameBuffer::setColorTexture(const utils::SharedPtr<Texture>& texture)
{
    if (utils::SharedPtr<OpenGLTexture> glTexture = texture.dynamicCast<OpenGLTexture>())
    {
        m_colorTexture = glTexture;
        glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferID);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorTexture->textureID(), 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    else
        throw utils::RuntimeError("Texture is not OpenGLTexture");
}

OpenGLFrameBuffer::~OpenGLFrameBuffer()
{
    glDeleteFramebuffers(1, &m_frameBufferID);
}

}