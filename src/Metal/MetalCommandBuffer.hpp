/*
 * ---------------------------------------------------
 * MetalCommandBuffer.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/08 10:33:10
 * ---------------------------------------------------
 */

#ifndef METALCOMMANDBUFFER_HPP
#define METALCOMMANDBUFFER_HPP

#include "Graphics/CommandBuffer.hpp"

#ifdef __OBJC__
    #import <Metal/Metal.h>
#else
    template<typename T>
    using id = T*;
    class MTLCommandQueue;
    class MTLCommandBuffer;
#endif // __OBJC__

namespace gfx
{

class MetalCommandBuffer : public CommandBuffer
{
public:
    MetalCommandBuffer() = delete;
    MetalCommandBuffer(const MetalCommandBuffer&) = delete;
    MetalCommandBuffer(MetalCommandBuffer&&)      = delete;

    MetalCommandBuffer(id<MTLCommandQueue>);
    
    ~MetalCommandBuffer();

private:
    id<MTLCommandBuffer> m_mtlCommandBuffer;

public:
    MetalCommandBuffer& operator = (const MetalCommandBuffer&) = delete;
    MetalCommandBuffer& operator = (MetalCommandBuffer&&)      = delete;
};

}

#endif // METALCOMMANDBUFFER_HPP
