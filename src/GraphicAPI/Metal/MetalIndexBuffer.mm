/*
 * ---------------------------------------------------
 * MetalIndexBuffer.mm
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/22 12:44:32
 * ---------------------------------------------------
 */

#include "GraphicAPI/Metal/MetalIndexBuffer.hpp"
#include "UtilsCPP/Array.hpp"
#include "UtilsCPP/Types.hpp"

using utils::uint32;
using utils::Array;

namespace gfx
{

MetalIndexBuffer::~MetalIndexBuffer(){ @autoreleasepool 
{
    [m_mtlBuffer release];
}}

MetalIndexBuffer::MetalIndexBuffer(id<MTLDevice> device, const Array<uint32>& indices) { @autoreleasepool 
{
    m_indexCount = indices.length();
    m_mtlBuffer = [device newBufferWithBytes:(const uint32*)indices
                                      length:sizeof(uint32) * indices.length()
                                     options:MTLResourceStorageModeShared];
    assert(m_mtlBuffer);
}}

}