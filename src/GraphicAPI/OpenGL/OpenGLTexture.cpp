/*
 * ---------------------------------------------------
 * OpenGLTexture.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/24 17:08:05
 * ---------------------------------------------------
 */

#include "GraphicAPI/OpenGL/OpenGLTexture.hpp"
#include "Graphics/Enums.hpp"
#include "Graphics/Error.hpp"

#define GL_CALL(x) { x; GLenum __err__ = glGetError(); if (__err__ != GL_NO_ERROR) throw OpenGLCallError(__err__); }

namespace gfx
{

OpenGLTexture::OpenGLTexture(const Texture::Descriptor& desc) : m_width(desc.width), m_height(desc.height), m_pixelFormat(desc.pixelFormat)
{
    GL_CALL(glGenTextures(1, &m_textureID))
    
    GL_CALL(glBindTexture(GL_TEXTURE_2D, m_textureID))

    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)m_width, (GLsizei)m_height, 0, toOpenGLPixelFormat(m_pixelFormat), GL_UNSIGNED_BYTE, nullptr))

    GL_CALL(glBindTexture(GL_TEXTURE_2D, 0))
}

void OpenGLTexture::replaceRegion(utils::uint32 offsetX, utils::uint32 offsetY, utils::uint32 width, utils::uint32 height, const void* bytes)
{
    GL_CALL(glBindTexture(GL_TEXTURE_2D, m_textureID))
    
    GL_CALL(glTexSubImage2D(GL_TEXTURE_2D, 0, (GLint)offsetX, (GLint)offsetY, (GLsizei)width, (GLsizei)height, toOpenGLPixelFormat(m_pixelFormat), GL_UNSIGNED_BYTE, bytes))

    GL_CALL(glBindTexture(GL_TEXTURE_2D, 0))
}

OpenGLTexture::~OpenGLTexture()
{
    glDeleteTextures(1, &m_textureID);
}

}
