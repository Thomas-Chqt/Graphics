/*
 * ---------------------------------------------------
 * OpenGLTexture.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/24 17:08:05
 * ---------------------------------------------------
 */

#include "GraphicAPI/OpenGL/OpenGLTexture.hpp"

namespace gfx
{

OpenGLTexture::OpenGLTexture(const Texture::Descriptor& desc) : m_width(desc.width), m_height(desc.height)
{
    switch (desc.pixelFormat)
    {
    case PixelFormat::RGBA:
        m_pixelFormat = GL_RGBA;
        break;
    case PixelFormat::ARGB:
        m_pixelFormat = GL_BGRA;
        break;
    }

    glGenTextures(1, &m_textureID);
    glBindTexture(GL_TEXTURE_2D, m_textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, 	  GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)m_width, (GLsizei)m_height, 0, m_pixelFormat, GL_UNSIGNED_BYTE, nullptr);

    glBindTexture(GL_TEXTURE_2D, 0);
}

OpenGLTexture::OpenGLTexture(OpenGLTexture&& mv)
    : m_textureID(mv.m_textureID),
      m_width(mv.m_width), m_height(mv.m_height),
      m_pixelFormat(mv.m_pixelFormat)
{
    mv.m_textureID = 0;
}

void OpenGLTexture::replaceRegion(utils::uint32 offsetX, utils::uint32 offsetY, utils::uint32 width, utils::uint32 height, const void* bytes)
{
    glBindTexture(GL_TEXTURE_2D, m_textureID);

    glTexSubImage2D(GL_TEXTURE_2D, 0,
        (GLint)offsetX, (GLint)offsetY,
        (GLsizei)width, (GLsizei)height,
        m_pixelFormat, GL_UNSIGNED_BYTE, bytes);

    glBindTexture(GL_TEXTURE_2D, 0);
}

OpenGLTexture::~OpenGLTexture()
{
    if (m_textureID)
        glDeleteTextures(1, &m_textureID);
}

}
