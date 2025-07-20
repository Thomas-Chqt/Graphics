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

namespace gfx
{

class MetalDevice;

class MetalBuffer : public Buffer
{
public:
    MetalBuffer() = delete;
    MetalBuffer(const MetalBuffer&) = delete;
    MetalBuffer(MetalBuffer&&)      = delete;

    MetalBuffer(const MetalDevice&, const Buffer::Descriptor&);

    inline const id<MTLBuffer>& mtlBuffer() const { return m_mtlBuffer; }

    ~MetalBuffer() override = default;

private:
    id<MTLBuffer> m_mtlBuffer;
    
public:
    MetalBuffer& operator = (const MetalBuffer&) = delete;
    MetalBuffer& operator = (MetalBuffer&&)      = delete;
};

}

#endif // METALBUFFER_HPP
