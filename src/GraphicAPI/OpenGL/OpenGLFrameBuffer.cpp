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
#include "Graphics/Error.hpp"
#include "Graphics/FrameBuffer.hpp"
#include "UtilsCPP/SharedPtr.hpp"

#include <glad/glad.h>

#define GL_CALL(x) { x; GLenum __err__ = glGetError(); if (__err__ != GL_NO_ERROR) throw OpenGLCallError(__err__); }

namespace gfx
{

OpenGLFrameBuffer::OpenGLFrameBuffer(const FrameBuffer::Descriptor& descriptor)
{
    GL_CALL(glGenFramebuffers(1, &m_frameBufferID))
    if (descriptor.colorTexture)
        OpenGLFrameBuffer::setColorTexture(descriptor.colorTexture);
    if (descriptor.depthTexture)
        OpenGLFrameBuffer::setDepthTexture(descriptor.depthTexture);
}

void OpenGLFrameBuffer::setColorTexture(const utils::SharedPtr<Texture>& texture)
{
    m_colorTexture = texture.forceDynamicCast<OpenGLTexture>();
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferID))
    GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorTexture->textureID(), 0))
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0))
}

void OpenGLFrameBuffer::setDepthTexture(const utils::SharedPtr<Texture>& texture)
{
    m_depthTexture = texture.forceDynamicCast<OpenGLTexture>();
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferID))
    GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTexture->textureID(), 0))
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0))
}

OpenGLFrameBuffer::~OpenGLFrameBuffer()
{
    glDeleteFramebuffers(1, &m_frameBufferID);
}

}