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

#include "GraphicAPI/OpenGL/OpenGLGraphicAPI.hpp"
#include "Graphics/Texture.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "UtilsCPP/Types.hpp"
#include <GL/glew.h>

namespace gfx
{

class OpenGLTexture : public Texture
{
private:
    friend utils::SharedPtr<Texture> OpenGLGraphicAPI::newTexture(utils::uint32 width, utils::uint32 height);

public:
    OpenGLTexture()                     = delete;
    OpenGLTexture(const OpenGLTexture&) = delete;
    OpenGLTexture(OpenGLTexture&&)      = delete;

    inline utils::uint32 width() override { return m_width; }
    inline utils::uint32 height() override { return m_height; }

    void replaceRegion(utils::uint32 offsetX, utils::uint32 offsetY, utils::uint32 width, utils::uint32 height, const void* bytes) override;
    
    inline GLuint textureID() const { return m_textureID; };

    ~OpenGLTexture() override;

private:
    OpenGLTexture(utils::uint32 width, utils::uint32 height);

    GLuint m_textureID;
    utils::uint32 m_width;
    utils::uint32 m_height;

public:
    OpenGLTexture& operator = (const OpenGLTexture&) = delete;
    OpenGLTexture& operator = (OpenGLTexture&&)      = delete;
};

}

#endif // OPENGLTEXTURE_HPP