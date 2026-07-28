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

#if !defined(__OBJC__)
#error this file can only by used in objective c
#endif

namespace gfx
{

class MetalDevice;

class MetalTexture : public Texture
{
public:
    MetalTexture(const MetalTexture&) = delete;
    MetalTexture(MetalTexture&&) = delete;

    MetalTexture(const Texture::Descriptor&);
    MetalTexture(const MetalDevice&, const Texture::Descriptor&);

    TextureType type() const override;

    uint32_t width() const override;
    uint32_t height() const override;

    uint32_t baseMipLevel() const override;
    uint32_t mipLevelCount() const override;

    uint32_t baseArrayLayer() const override;
    uint32_t arrayLayerCount() const override;

    PixelFormat pixelFormat() const override;

    TextureUsages usages() const override;
    ResourceStorageMode storageMode() const override;

    inline id<MTLTexture> mtltexture() const { return m_mtlTexture; }
    inline void setMtlTexture(const id<MTLTexture>& t) { m_mtlTexture = t; }

    ~MetalTexture() override = default;

protected:
    MetalTexture() = default;

    TextureUsages m_usages = TextureUsage::shaderRead;
    ResourceStorageMode m_storageMode = ResourceStorageMode::deviceLocal;

    id<MTLTexture> m_mtlTexture = nullptr;

public:
    MetalTexture& operator = (const MetalTexture&) = delete;
    MetalTexture& operator = (MetalTexture&&) = delete;
};

}

#endif // METALTEXTURE_HPP
