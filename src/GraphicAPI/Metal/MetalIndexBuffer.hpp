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

#include "Graphics/IndexBuffer.hpp"
#include "UtilsCPP/Array.hpp"
#include "UtilsCPP/Types.hpp"

#ifdef __OBJC__
    #import <Metal/Metal.h>
#else
    template<typename T> using id = void*;

    class MTLBuffer;
    class MTLDevice;
#endif // OBJCPP


namespace gfx
{

class MetalIndexBuffer : public IndexBuffer
{
public:
    MetalIndexBuffer()                        = delete;
    MetalIndexBuffer(const MetalIndexBuffer&) = delete;
    MetalIndexBuffer(MetalIndexBuffer&&)      = delete;

    MetalIndexBuffer(id<MTLDevice> device, const utils::Array<utils::uint32>& indices);

    inline id<MTLBuffer> mtlBuffer() { return m_mtlBuffer; }
    inline utils::uint64 indexCount() override { return static_cast<utils::uint64>(m_indexCount); }
    
    ~MetalIndexBuffer() override;

private:
    utils::Array<utils::uint32>::Size m_indexCount;
    id<MTLBuffer> m_mtlBuffer;

public:
    MetalIndexBuffer& operator = (const MetalIndexBuffer&) = delete;
    MetalIndexBuffer& operator = (MetalIndexBuffer&&)      = delete;
};

}

#endif // METALINDEXBUFFER_HPP