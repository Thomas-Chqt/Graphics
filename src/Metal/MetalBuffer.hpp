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

#ifdef __OBJC__
    #import <Metal/Metal.h>
#else
    template<typename T> using id = T*;
    #define nil nullptr
    class MTLDevice;
    class MTLBuffer;
#endif // __OBJC__

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <vector>
    namespace ext = std;
#endif

namespace gfx
{

class MetalDevice;

class MetalBuffer : public Buffer
{
public:
    MetalBuffer() = delete;
    MetalBuffer(const MetalBuffer&) = delete;
    MetalBuffer(MetalBuffer&&)      = delete;

    MetalBuffer(const MetalDevice*, const Buffer::Descriptor&);

    void setContent(void* data, size_t size) override;

    const id<MTLBuffer>& mtlBuffer() const;

    ~MetalBuffer() override;

private:
    const MetalDevice* const m_device;

    struct FrameData
    {
        id<MTLBuffer> mtlBuffer;
    };
    ext::vector<FrameData> m_frameDatas;
    
public:
    MetalBuffer& operator = (const MetalBuffer&) = delete;
    MetalBuffer& operator = (MetalBuffer&&)      = delete;
};

}

#endif // METALBUFFER_HPP
