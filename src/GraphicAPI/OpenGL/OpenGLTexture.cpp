/*
 * ---------------------------------------------------
 * OpenGLTexture.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/24 17:08:05
 * ---------------------------------------------------
 */

#include "GraphicAPI/OpenGL/OpenGLTexture.hpp"
#include "Graphics/VertexBuffer.hpp"

namespace gfx
{

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
    glDeleteTextures(1, &m_textureID);
}

OpenGLTexture::OpenGLTexture(utils::uint32 width, utils::uint32 height, Texture::PixelFormat pxFormat) : m_width(width), m_height(height)
{
    switch (pxFormat)
    {
    case Texture::PixelFormat::RGBA:
        m_pixelFormat = GL_RGBA;
        break;
    case Texture::PixelFormat::ARGB:
        m_pixelFormat = GL_BGRA;
        break;
    }

    glGenTextures(1, &m_textureID);
    glBindTexture(GL_TEXTURE_2D, m_textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, 	  GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)width, (GLsizei)height, 0, m_pixelFormat, GL_UNSIGNED_BYTE, nullptr);

    glBindTexture(GL_TEXTURE_2D, 0);
}

}
