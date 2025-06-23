/*
 * ---------------------------------------------------
 * MetalTexture.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/06 17:47:29
 * ---------------------------------------------------
 */

#ifndef METALTEXTURE_HPP
#define METALTEXTURE_HPP

#include "Graphics/Enums.hpp"
#include "Graphics/Texture.hpp"

#ifdef __OBJC__
    #import <Metal/Metal.h>
#else
    template<typename T> using id = T*;
    class MTLTexture;
#endif // __OBJC__

namespace gfx
{

class MetalTexture : public Texture
{
public:
    MetalTexture() = delete;
    MetalTexture(const MetalTexture&) = delete;
    MetalTexture(MetalTexture&&)      = delete;
    
    MetalTexture(const id<MTLTexture>&);

    uint32_t width() const override;
    uint32_t height() const override;
    PixelFormat pixelFormat(void) const override;

    const id<MTLTexture>& mtltexture(void) const { return m_mtlTexture; }

    ~MetalTexture();

private:
    id<MTLTexture> m_mtlTexture = nullptr;
    
public:
    MetalTexture& operator = (const MetalTexture&) = delete;
    MetalTexture& operator = (MetalTexture&&)      = delete;
};

}

#endif // METALTEXTURE_HPP
