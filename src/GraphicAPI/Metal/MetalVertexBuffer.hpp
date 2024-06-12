/*
 * ---------------------------------------------------
 * MetalVertexBuffer.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/20 17:09:12
 * ---------------------------------------------------
 */

#ifndef METALVERTEXBUFFER_HPP
# define METALVERTEXBUFFER_HPP

#ifdef __OBJC__
    #import <Metal/Metal.h>
#else
    template<typename T> using id = void*;

    class MTLBuffer;
    class MTLDevice;
#endif // OBJCPP

#include "Graphics/VertexBuffer.hpp"
#include "UtilsCPP/Types.hpp"

namespace gfx
{

class MetalVertexBuffer : public VertexBuffer
{
public:
    MetalVertexBuffer()                         = delete;
    MetalVertexBuffer(const MetalVertexBuffer&) = delete;
    MetalVertexBuffer(MetalVertexBuffer&&)      = delete;

    MetalVertexBuffer(id<MTLDevice> device, void* data, utils::uint64 size);

    inline id<MTLBuffer> mtlBuffer() { return m_mtlBuffer; }
    
    ~MetalVertexBuffer() override;

private:
    id<MTLBuffer> m_mtlBuffer;

public:
    MetalVertexBuffer& operator = (const MetalVertexBuffer&) = delete;
    MetalVertexBuffer& operator = (MetalVertexBuffer&&)      = delete;
};

}


#endif // METALVERTEXBUFFER_HPP