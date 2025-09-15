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

#include "Metal/MetalBuffer.hpp"

namespace gfx
{

class MetalDevice;

class MetalTexture : public Texture
{
public:
    MetalTexture() = delete;
    MetalTexture(const MetalTexture&) = delete;
    MetalTexture(MetalTexture&&)      = delete;
    
    MetalTexture(const id<MTLTexture>&, const Texture::Descriptor&);
    MetalTexture(const MetalDevice&, const Texture::Descriptor&);

    uint32_t width() const override;
    uint32_t height() const override;
    PixelFormat pixelFormat() const override;
    inline TextureUsages usages() const override { return m_usages; };
    inline ResourceStorageMode storageMode() const override { return m_storageMode; };

    inline const id<MTLTexture>& mtltexture() const { return m_mtlTexture; }

    ~MetalTexture() override;

private:
    TextureUsages m_usages;
    ResourceStorageMode m_storageMode;

    id<MTLTexture> m_mtlTexture = nullptr;
    
public:
    MetalTexture& operator = (const MetalTexture&) = delete;
    MetalTexture& operator = (MetalTexture&&)      = delete;
};

}

#endif // METALTEXTURE_HPP
