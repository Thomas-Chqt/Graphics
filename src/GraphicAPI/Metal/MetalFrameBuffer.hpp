/*
 * ---------------------------------------------------
 * MetalFrameBuffer.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/10 13:55:02
 * ---------------------------------------------------
 */

#ifndef METALFRAMEBUFFER_HPP
# define METALFRAMEBUFFER_HPP

#include "GraphicAPI/Metal/MetalGraphicAPI.hpp"
#include "GraphicAPI/Metal/MetalTexture.hpp"
#include "Graphics/FrameBuffer.hpp"

#ifdef __OBJC__
    #import <Metal/Metal.h>
#else
    template<typename T> using id = void*;
    class MTLTexture;
#endif // OBJCPP

namespace gfx
{
class MetalFrameBuffer : public FrameBuffer
{
public:
    MetalFrameBuffer(const MetalFrameBuffer&) = delete;
    MetalFrameBuffer(MetalFrameBuffer&&)      = delete;

    virtual id<MTLTexture> mtlColorTexture() = 0;

    virtual ~MetalFrameBuffer() override = default;

protected:
    MetalFrameBuffer() = default;

public:
    MetalFrameBuffer& operator = (const MetalFrameBuffer&) = delete;
    MetalFrameBuffer& operator = (MetalFrameBuffer&&)      = delete;
};


class MetalTextureFrameBuffer : public MetalFrameBuffer
{
private:
    friend utils::SharedPtr<FrameBuffer> MetalGraphicAPI::newFrameBuffer(const FrameBuffer::Descriptor&) const;

public:
    MetalTextureFrameBuffer()                               = delete;
    MetalTextureFrameBuffer(const MetalTextureFrameBuffer&) = delete;
    MetalTextureFrameBuffer(MetalTextureFrameBuffer&&)      = delete;

    id<MTLTexture> mtlColorTexture() override;

    inline const MetalTexture& colorTexture() const { return m_colorTexture; }
    
    ~MetalTextureFrameBuffer() override = default;

private:
    MetalTextureFrameBuffer(MetalTexture&& colorTexture);

    MetalTexture m_colorTexture;

public:
    MetalTextureFrameBuffer& operator = (const MetalTextureFrameBuffer&) = delete;
    MetalTextureFrameBuffer& operator = (MetalTextureFrameBuffer&&)      = delete;
};


class MetalDrawableFrameBuffer : public MetalFrameBuffer
{
public:
    MetalDrawableFrameBuffer()                                = delete;
    MetalDrawableFrameBuffer(const MetalDrawableFrameBuffer&) = delete;
    MetalDrawableFrameBuffer(MetalDrawableFrameBuffer&&)      = delete;

    MetalDrawableFrameBuffer(id<CAMetalDrawable>);

    id<MTLTexture> mtlColorTexture() override;

    ~MetalDrawableFrameBuffer() override;

private:
    id<CAMetalDrawable> m_caDrawable;

public:
    MetalDrawableFrameBuffer& operator = (const MetalDrawableFrameBuffer&) = delete;
    MetalDrawableFrameBuffer& operator = (MetalDrawableFrameBuffer&&)      = delete;
};

}

#endif // METALFRAMEBUFFER_HPP