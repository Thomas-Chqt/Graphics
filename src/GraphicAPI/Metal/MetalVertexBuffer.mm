/*
 * ---------------------------------------------------
 * MetalVertexBuffer.mm
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/20 17:15:03
 * ---------------------------------------------------
 */



#include "GraphicAPI/Metal/MetalVertexBuffer.hpp"

namespace gfx
{

MetalVertexBuffer::MetalVertexBuffer(id<MTLDevice> device, void* data, utils::uint64 size)
{
    m_mtlBuffer = [device newBufferWithBytes:data
                                      length:size
                                     options:MTLResourceStorageModeShared];
    assert(m_mtlBuffer);
}

MetalVertexBuffer::~MetalVertexBuffer()
{
    [m_mtlBuffer release];
}

}