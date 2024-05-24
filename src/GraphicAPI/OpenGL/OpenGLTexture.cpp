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

void OpenGLTexture::replaceRegion(utils::uint32 offsetX, utils::uint32 offsetY, utils::uint32 width, utils::uint32 height, const void* bytes)
{
    glBindTexture(GL_TEXTURE_2D, m_textureID);

    glTexSubImage2D(GL_TEXTURE_2D, 0,
        (GLint)offsetX, (GLint)offsetY,
        (GLsizei)width, (GLsizei)height,
        GL_RGBA, GL_UNSIGNED_BYTE, bytes);

    glBindTexture(GL_TEXTURE_2D, 0);
}

OpenGLTexture::~OpenGLTexture()
{
    glDeleteTextures(1, &m_textureID);
}

OpenGLTexture::OpenGLTexture(utils::uint32 width, utils::uint32 height) : m_width(width), m_height(height)
{
    glGenTextures(1, &m_textureID);
    glBindTexture(GL_TEXTURE_2D, m_textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)width, (GLsizei)height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    glBindTexture(GL_TEXTURE_2D, 0);
}

}