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

#include "Graphics/Texture.hpp"
#include "UtilsCPP/Types.hpp"
#include <glad/glad.h>

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

    void replaceRegion(const Region&, const void* bytes) override;
    void replaceRegion(const Region&, utils::uint32 slice, const void* bytes) override;
    
    inline GLuint textureID() const { return m_textureID; };
    inline GLenum textureType() const { return m_textureType; }

    #ifdef GFX_BUILD_IMGUI
    inline void* imguiTextureId() const override { return (void*)(utils::uint64)textureID(); }
    inline ImVec2 imguiUV0() const override { return ImVec2(0, 1); }
    inline ImVec2 imguiUV1() const override { return ImVec2(1, 0); }
    #endif

    ~OpenGLTexture() override;

private:
    GLuint m_textureID = 0;
    const GLenum m_textureType = 0;
    const GLsizei m_width = 0;
    const GLsizei m_height = 0;
    const GLenum m_internalPixelFormat = 0;
    const GLenum m_pixelFormat = 0;

public:
    OpenGLTexture& operator = (const OpenGLTexture&) = delete;
    OpenGLTexture& operator = (OpenGLTexture&&)      = delete;
};

}

#endif // OPENGLTEXTURE_HPP
