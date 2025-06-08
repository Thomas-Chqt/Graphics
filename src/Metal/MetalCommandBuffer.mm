/*
 * ---------------------------------------------------
 * MetalCommandBuffer.mm
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/08 10:37:29
 * ---------------------------------------------------
 */

#include "Metal/MetalCommandBuffer.hpp"

#import <Metal/Metal.h>

namespace gfx
{

MetalCommandBuffer::MetalCommandBuffer(id<MTLCommandQueue> queue)
{
    m_mtlCommandBuffer = [[queue commandBuffer] retain];
}

MetalCommandBuffer::~MetalCommandBuffer()
{
    [m_mtlCommandBuffer release];
}

}
