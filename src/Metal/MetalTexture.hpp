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
#include <bit>

#if !defined(__OBJC__)
#error this file can only by used in objective c
#endif

namespace gfx
{

class MetalDevice;

class MetalTexture : public Texture
{
public:
    MetalTexture() = delete;
    MetalTexture(const MetalTexture&) = delete;
    MetalTexture(MetalTexture&&)      = delete;

    MetalTexture(const Texture::Descriptor&);
    MetalTexture(const MetalDevice&, const Texture::Descriptor&);

    TextureType type() const override;
    uint32_t width() const override;
    uint32_t height() const override;
    PixelFormat pixelFormat() const override;
    inline TextureUsages usages() const override { return m_usages; };
    inline ResourceStorageMode storageMode() const override { return m_storageMode; };

#if defined (GFX_IMGUI_ENABLED)
    inline void initImTextureId(const std::shared_ptr<Sampler>&) override {} // no-op
    inline std::optional<uint64_t> imTextureId() const override { return std::bit_cast<uint64_t>((__bridge void*)m_mtlTexture); }
#endif

    inline id<MTLTexture> mtltexture() const { return m_mtlTexture; }
    inline void setMtlTexture(const id<MTLTexture>& t) { m_mtlTexture = t; }

    ~MetalTexture() override = default;

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
