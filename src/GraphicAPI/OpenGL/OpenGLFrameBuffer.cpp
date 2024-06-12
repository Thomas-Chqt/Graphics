/*
 * ---------------------------------------------------
 * OpenGLFrameBuffer.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/12 11:07:55
 * ---------------------------------------------------
 */

#include "GraphicAPI/OpenGL/OpenGLFrameBuffer.hpp"
#include "UtilsCPP/Types.hpp"

#include <GL/glew.h>
#include <utility>

namespace gfx
{

OpenGLScreenFrameBuffer::OpenGLScreenFrameBuffer(const utils::SharedPtr<OpenGLWindow>& window) : m_window(window)
{
}

void OpenGLScreenFrameBuffer::useForRendering()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    utils::uint32 width;
    utils::uint32 height;
    m_window->getFrameBufferSize(&width, &height);
    glViewport(0, 0, width, height);
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
}

}