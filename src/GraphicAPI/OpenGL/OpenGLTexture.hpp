/*
 * ---------------------------------------------------
 * OpenGLTexture.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/24 16:57:14
 * ---------------------------------------------------
 */

#ifndef OPENGLTEXTURE_HPP
# define OPENGLTEXTURE_HPP

#include "Graphics/Enums.hpp"
#include "Graphics/Texture.hpp"
#include "UtilsCPP/Types.hpp"
#include <GL/glew.h>

namespace gfx
{

class OpenGLTexture : public Texture
{
public:
    OpenGLTexture()                     = delete;
    OpenGLTexture(const OpenGLTexture&) = delete;
    OpenGLTexture(OpenGLTexture&&)      = delete;

    OpenGLTexture(const Texture::Descriptor&);

    inline utils::uint32 width() override { return m_width; }
    inline utils::uint32 height() override { return m_height; }

    void replaceRegion(utils::uint32 offsetX, utils::uint32 offsetY, utils::uint32 width, utils::uint32 height, const void* bytes) override;
    
    inline GLuint textureID() const { return m_textureID; };

    ~OpenGLTexture() override;

private:
    GLuint m_textureID;
    const utils::uint32 m_width;
    const utils::uint32 m_height;
    const PixelFormat m_pixelFormat;

public:
    OpenGLTexture& operator = (const OpenGLTexture&) = delete;
    OpenGLTexture& operator = (OpenGLTexture&&)      = delete;
};

}

#endif // OPENGLTEXTURE_HPP