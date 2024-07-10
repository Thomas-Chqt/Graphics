/*
 * ---------------------------------------------------
 * MetalBuffer.mm
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/07/09 14:28:35
 * ---------------------------------------------------
 */

#include "GraphicAPI/Metal/MetalBuffer.hpp"
#include "Graphics/Error.hpp"

namespace gfx
{

MetalBuffer::MetalBuffer(const id<MTLDevice>& mtlDevice, const Buffer::Descriptor& descriptor) { @autoreleasepool
{
    if (descriptor.initialData == nullptr)
        m_mtlBuffer = [mtlDevice newBufferWithLength:descriptor.size options:MTLResourceStorageModeShared];    
    else
        m_mtlBuffer = [mtlDevice newBufferWithBytes:descriptor.initialData length:descriptor.size options:MTLResourceStorageModeShared];
    if (!m_mtlBuffer)
        throw MTLBufferCreationError();
}}

utils::uint64 MetalBuffer::size() { @autoreleasepool
{
    return m_mtlBuffer.length;
}}

MetalBuffer::~MetalBuffer() { @autoreleasepool
{
    [m_mtlBuffer release];
}}

}