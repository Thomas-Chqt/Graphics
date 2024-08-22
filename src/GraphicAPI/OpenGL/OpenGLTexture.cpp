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
#include "Graphics/Texture.hpp"
#include "UtilsCPP/RuntimeError.hpp"

#define GL_CALL(x) { x; GLenum __err__ = glGetError(); if (__err__ != GL_NO_ERROR) throw OpenGLCallError(__err__); }


namespace gfx
{

GLenum toOpenglTextureType(Texture::Type type)
{
    switch (type)
    {
    case Texture::Type::texture2d:
        return GL_TEXTURE_2D;
    case Texture::Type::textureCube:
        return GL_TEXTURE_CUBE_MAP;
    }
}

OpenGLTexture::OpenGLTexture(const Texture::Descriptor& desc)
    : m_textureType(toOpenglTextureType(desc.type)),
      m_width((GLsizei)desc.width),
      m_height((GLsizei)desc.height),
      m_internalPixelFormat(toOpenGLInternalPixelFormat(desc.pixelFormat)),
      m_pixelFormat(toOpenGLPixelFormat(desc.pixelFormat))
{
    GL_CALL(glGenTextures(1, &m_textureID))
    
    GL_CALL(glBindTexture(m_textureType, m_textureID))

    if (m_textureType == GL_TEXTURE_2D)
        GL_CALL(glTexImage2D(m_textureType, 0, m_internalPixelFormat, m_width, m_height, 0, m_pixelFormat, GL_UNSIGNED_BYTE, nullptr))
    else
    {
        for (int i = 0; i < 6; i++)
            GL_CALL(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, 0, m_internalPixelFormat, m_width, m_height, 0, m_pixelFormat, GL_UNSIGNED_BYTE, nullptr))
    }

    GL_CALL(glTexParameteri(m_textureType, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL_CALL(glTexParameteri(m_textureType, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GL_CALL(glTexParameteri(m_textureType, GL_TEXTURE_WRAP_S, 	  GL_CLAMP_TO_EDGE));
    GL_CALL(glTexParameteri(m_textureType, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE));
    if (m_textureType == GL_TEXTURE_CUBE_MAP)
        GL_CALL(glTexParameteri(m_textureType, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));

    GL_CALL(glBindTexture(m_textureType, 0))
}

void OpenGLTexture::replaceRegion(const Region& region, const void* bytes)
{
    if (m_textureType == GL_TEXTURE_CUBE_MAP)
        throw utils::RuntimeError("Need to provide a slice");

    GL_CALL(glBindTexture(m_textureType, m_textureID))
    
    GL_CALL(glTexSubImage2D(
        m_textureType, 0,
        (GLint)region.offsetX, (GLint)region.offsetY,
        (GLsizei)region.width, (GLsizei)region.height,
        m_pixelFormat, GL_UNSIGNED_BYTE, bytes
    ))

    GL_CALL(glBindTexture(m_textureType, 0))
}

void OpenGLTexture::replaceRegion(const Region& region, utils::uint32 slice, const void* bytes)
{
    if (m_textureType != GL_TEXTURE_CUBE_MAP)
        throw utils::RuntimeError("No slice for other than texture cube");

    GL_CALL(glBindTexture(m_textureType, m_textureID))
    
    GL_CALL(glTexSubImage2D(
        GL_TEXTURE_CUBE_MAP_POSITIVE_X + slice, 0,
        (GLint)region.offsetX, (GLint)region.offsetY,
        (GLsizei)region.width, (GLsizei)region.height,
        m_pixelFormat, GL_UNSIGNED_BYTE, bytes
    ))

    GL_CALL(glBindTexture(m_textureType, 0))

}


OpenGLTexture::~OpenGLTexture()
{
    glDeleteTextures(1, &m_textureID);
}

}
