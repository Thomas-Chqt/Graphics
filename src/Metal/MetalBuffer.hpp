/*
 * ---------------------------------------------------
 * MetalBuffer.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/07/14 08:11:10
 * ---------------------------------------------------
 */

#ifndef METALBUFFER_HPP
#define METALBUFFER_HPP

#include "Graphics/Buffer.hpp"

#include "common.hpp"

#ifdef __OBJC__
    #import <Metal/Metal.h>
#else
    template<typename T> using id = T*;
    #define nil nullptr
    class MTLBuffer;
#endif // __OBJC__

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    namespace ext = std; // NOLINT
#endif

namespace gfx
{

class MetalDevice;

class MetalBuffer : public Buffer
{
public:
    MetalBuffer() = default;
    MetalBuffer(const MetalBuffer&) = delete;
    MetalBuffer(MetalBuffer&&) = default;

    MetalBuffer(const MetalDevice*, const Buffer::Descriptor&);

    size_t size() const override;

    void setContent(const void* data, size_t size) override;

    const id<MTLBuffer>& mtlBuffer() const;

    ~MetalBuffer() override = default;

protected:
    void* contentVoid() override;

private:
    struct FrameData
    {
        id<MTLBuffer> mtlBuffer = nil;

        FrameData() = default;
        FrameData(const FrameData&) = delete;
        FrameData(FrameData&&) noexcept ;
        ~FrameData();
        FrameData& operator = (const FrameData&) = delete;
        FrameData& operator = (FrameData&&) noexcept ;
    };

    FrameData& currentFrameData();
    const FrameData& currentFrameData() const;

    const MetalDevice* m_device = nullptr;
    BufferUsages m_usages;

    PerFrameInFlight<FrameData> m_frameDatas;
    
public:
    MetalBuffer& operator = (const MetalBuffer&) = delete;
    MetalBuffer& operator = (MetalBuffer&&) = default;

    inline operator bool () const { return m_device != nullptr; }
};

}

#endif // METALBUFFER_HPP
