/*
 * ---------------------------------------------------
 * MetalBuffer.mm
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/07/14 09:07:05
 * ---------------------------------------------------
 */

#include "Graphics/Buffer.hpp"

#include "Metal/MetalBuffer.hpp"
#include "Metal/MetalDevice.hpp"

#import <Metal/Metal.h>

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <stdexcept>
    namespace ext = std;
#endif

namespace gfx
{

MetalBuffer::MetalBuffer(const MetalDevice& device, const Buffer::Descriptor& desc) { @autoreleasepool
{
    if (desc.data == nullptr)
        m_mtlBuffer = [device.mtlDevice() newBufferWithLength:desc.size options:MTLResourceStorageModeShared];    
    else
        m_mtlBuffer = [device.mtlDevice() newBufferWithBytes:desc.data length:desc.size options:MTLResourceStorageModeShared];

    if (m_mtlBuffer == Nil)
        throw ext::runtime_error("mtl buffer creating failed");
}}

}
