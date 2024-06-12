/*
 * ---------------------------------------------------
 * OpenGLFrameBuffer.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/12 11:07:55
 * ---------------------------------------------------
 */

#include "GraphicAPI/OpenGL/OpenGLFrameBuffer.hpp"
#include "UtilsCPP/RuntimeError.hpp"

#include <GL/glew.h>
#include <utility>

namespace gfx
{

void OpenGLScreenFrameBuffer::useForRendering()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, 800 * 2, 600 * 2);

}

OpenGLTextureFrameBuffer::OpenGLTextureFrameBuffer(OpenGLTexture&& colorTexture) : m_colorTexture(std::move(colorTexture))
{
    glGenFramebuffers(1, &m_frameBufferID);
    glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferID);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorTexture.textureID(), 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLTextureFrameBuffer::useForRendering()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferID);
    glViewport(0, 0, m_colorTexture.width(), m_colorTexture.height());
    
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        throw utils::RuntimeError("glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE");
}

}