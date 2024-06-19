/*
 * ---------------------------------------------------
 * MetalTexture.mm
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/24 16:39:54
 * ---------------------------------------------------
 */

#include "GraphicAPI/Metal/MetalTexture.hpp"
#include <Metal/MTLTypes.h>
#include <Metal/Metal.h>

namespace gfx
{

MetalTexture::MetalTexture(MetalTexture&& mv) : m_mtlTexture(mv.m_mtlTexture)
{
    mv.m_mtlTexture = nullptr;
}

MetalTexture::MetalTexture(id<MTLDevice> device, MTLTextureDescriptor* desc) { @autoreleasepool
{
    m_mtlTexture = [device newTextureWithDescriptor:desc];
}}

utils::uint32 MetalTexture::width() { @autoreleasepool
{
    return m_mtlTexture.width;
}}

utils::uint32 MetalTexture::height() { @autoreleasepool
{
    return m_mtlTexture.height;
}}

void MetalTexture::replaceRegion(utils::uint32 offsetX, utils::uint32 offsetY, utils::uint32 width, utils::uint32 height, const void* bytes) { @autoreleasepool
{
    [m_mtlTexture replaceRegion:MTLRegionMake2D(offsetX, offsetY, width, height)
                    mipmapLevel:0
                      withBytes:bytes
                    bytesPerRow:4 * width];
}}

MetalTexture::~MetalTexture() { @autoreleasepool
{
    if (m_mtlTexture)
        [m_mtlTexture release];
}}

MetalTexture& MetalTexture::operator = (MetalTexture&& mv)
{
    if (this != &mv)
    {
        m_mtlTexture = mv.m_mtlTexture;
        mv.m_mtlTexture = nullptr;
    }
    return *this;
}

}