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

#include "GraphicAPI/Metal/MetalTexture.hpp"
#include "Graphics/FrameBuffer.hpp"

#ifdef __OBJC__
    #import <Metal/Metal.h>
    #include <QuartzCore/QuartzCore.h>
#else
    template<typename T> using id = void*;
    class MTLTexture;
    class CAMetalLayer;
    class CAMetalDrawable;
#endif // OBJCPP

namespace gfx
{
class MetalFrameBuffer : public FrameBuffer
{
public:
    MetalFrameBuffer(const MetalFrameBuffer&) = delete;
    MetalFrameBuffer(MetalFrameBuffer&&)      = delete;

    virtual id<MTLTexture> mtlRenderTexture() = 0;

    virtual ~MetalFrameBuffer() override = default;

protected:
    MetalFrameBuffer() = default;

public:
    MetalFrameBuffer& operator = (const MetalFrameBuffer&) = delete;
    MetalFrameBuffer& operator = (MetalFrameBuffer&&)      = delete;
};

class MetalLayerFrameBuffer : public MetalFrameBuffer
{
public:
    MetalLayerFrameBuffer()                             = delete;
    MetalLayerFrameBuffer(const MetalLayerFrameBuffer&) = delete;
    MetalLayerFrameBuffer(MetalLayerFrameBuffer&&)      = delete;

    MetalLayerFrameBuffer(CAMetalLayer*);

    id<MTLTexture> mtlRenderTexture() override;

    inline id<CAMetalDrawable> currentDrawable() { return m_currentDrawable; }
    void releaseCurrentDrawable();

    ~MetalLayerFrameBuffer() override;

private:
    CAMetalLayer* m_metalLayer = nullptr;
    id<CAMetalDrawable> m_currentDrawable = nullptr;

public:
    MetalLayerFrameBuffer& operator = (const MetalLayerFrameBuffer&) = delete;
    MetalLayerFrameBuffer& operator = (MetalLayerFrameBuffer&&)      = delete;
};

class MetalTextureFrameBuffer : public MetalFrameBuffer
{
public:
    MetalTextureFrameBuffer()                               = delete;
    MetalTextureFrameBuffer(const MetalTextureFrameBuffer&) = delete;
    MetalTextureFrameBuffer(MetalTextureFrameBuffer&&)      = delete;

    MetalTextureFrameBuffer(MetalTexture&& colorTexture);

    id<MTLTexture> mtlRenderTexture() override;

    inline const MetalTexture& colorTexture() const { return m_colorTexture; }
    
    ~MetalTextureFrameBuffer() override = default;

private:
    MetalTexture m_colorTexture;

public:
    MetalTextureFrameBuffer& operator = (const MetalTextureFrameBuffer&) = delete;
    MetalTextureFrameBuffer& operator = (MetalTextureFrameBuffer&&)      = delete;
};



}

#endif // METALFRAMEBUFFER_HPP