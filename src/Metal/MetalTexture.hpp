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
#include "common.hpp"

#ifdef __OBJC__
    #import <Metal/Metal.h>
#else
    template<typename T> using id = T*;
    class MTLTexture;
#endif // __OBJC__

namespace gfx
{

class MetalDevice;

class MetalTexture : public Texture
{
public:
    MetalTexture() = delete;
    MetalTexture(const MetalTexture&) = delete;
    MetalTexture(MetalTexture&&)      = delete;
    
    MetalTexture(const MetalDevice*, const id<MTLTexture>&);
    MetalTexture(const MetalDevice*, const Texture::Descriptor&);

    uint32_t width() const override;
    uint32_t height() const override;
    PixelFormat pixelFormat() const override;

    inline const id<MTLTexture>& mtltexture() const { return currentFrameData().mtlTexture; }

    ~MetalTexture() override;

private:
    struct FrameData
    {
        id<MTLTexture> mtlTexture = nullptr;
    };

    FrameData& currentFrameData();
    const FrameData& currentFrameData() const;

    const MetalDevice* m_device;
    PerFrameInFlight<FrameData> m_frameDatas;
    uint8_t m_imageCount;
    
public:
    MetalTexture& operator = (const MetalTexture&) = delete;
    MetalTexture& operator = (MetalTexture&&)      = delete;
};

}

#endif // METALTEXTURE_HPP
