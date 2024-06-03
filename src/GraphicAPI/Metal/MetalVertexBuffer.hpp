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
    class MTLBuffer;
    class MTLDevice;
#endif // OBJCPP

#include "GraphicAPI/Metal/MetalGraphicAPI.hpp"
#include "Graphics/VertexBuffer.hpp"
#include "UtilsCPP/Types.hpp"

namespace gfx
{

class MetalVertexBuffer : public VertexBuffer
{
private:
    friend utils::SharedPtr<VertexBuffer> MetalGraphicAPI::newVertexBuffer(void* data, utils::uint64 size, const VertexBuffer::LayoutBase& layout) const;

public:
    MetalVertexBuffer()                         = delete;
    MetalVertexBuffer(const MetalVertexBuffer&) = delete;
    MetalVertexBuffer(MetalVertexBuffer&&)      = delete;

    inline id<MTLBuffer> mtlBuffer() { return m_mtlBuffer; }
    
    ~MetalVertexBuffer() override;

private:
    MetalVertexBuffer(id<MTLDevice> device, void* data, utils::uint64 size);

    id<MTLBuffer> m_mtlBuffer;

public:
    MetalVertexBuffer& operator = (const MetalVertexBuffer&) = delete;
    MetalVertexBuffer& operator = (MetalVertexBuffer&&)      = delete;
};

}


#endif // METALVERTEXBUFFER_HPP