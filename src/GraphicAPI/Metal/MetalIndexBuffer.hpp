/*
 * ---------------------------------------------------
 * MetalIndexBuffer.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/22 12:41:16
 * ---------------------------------------------------
 */

#ifndef METALINDEXBUFFER_HPP
# define METALINDEXBUFFER_HPP

#include "GraphicAPI/Metal/MetalGraphicAPI.hpp"
#include "Graphics/IndexBuffer.hpp"
#include "UtilsCPP/SharedPtr.hpp"

#ifdef __OBJC__
    #import <Metal/Metal.h>
#else
    class MTLBuffer;
    class MTLDevice;
#endif // OBJCPP


namespace gfx
{

class MetalIndexBuffer : public IndexBuffer
{
private:
    friend utils::SharedPtr<IndexBuffer> MetalGraphicAPI::newIndexBuffer(const utils::Array<utils::uint32>&);

public:
    MetalIndexBuffer()                         = delete;
    MetalIndexBuffer(const MetalIndexBuffer&) = delete;
    MetalIndexBuffer(MetalIndexBuffer&&)      = delete;

    inline id<MTLBuffer> mtlBuffer() { return m_mtlBuffer; }
    inline utils::uint64 indexCount() override { return static_cast<utils::uint64>(m_indexCount); }
    
    ~MetalIndexBuffer() override;

private:
    MetalIndexBuffer(id<MTLDevice> device, const utils::Array<utils::uint32>& indices);

    utils::Array<utils::uint32>::Size m_indexCount;
    id<MTLBuffer> m_mtlBuffer;

public:
    MetalIndexBuffer& operator = (const MetalIndexBuffer&) = delete;
    MetalIndexBuffer& operator = (MetalIndexBuffer&&)      = delete;
};

}

#endif // METALINDEXBUFFER_HPP