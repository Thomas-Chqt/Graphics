/*
 * ---------------------------------------------------
 * MetalTexture.mm
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/24 16:39:54
 * ---------------------------------------------------
 */

#include "GraphicAPI/Metal/MetalTexture.hpp"
#include "Graphics/Enums.hpp"
#include "Graphics/Error.hpp"
#include <Metal/MTLTypes.h>
#include <Metal/Metal.h>

namespace gfx
{

MetalTexture::MetalTexture(MetalTexture&& mv) : m_mtlTexture(mv.m_mtlTexture)
{
    mv.m_mtlTexture = nullptr;
}

MetalTexture::MetalTexture(id<MTLDevice> device, const Texture::Descriptor& desc) { @autoreleasepool
{
    MTLTextureDescriptor* mtlTextureDescriptor = [[[MTLTextureDescriptor alloc] init] autorelease];
    mtlTextureDescriptor.textureType = MTLTextureType2D;
    mtlTextureDescriptor.pixelFormat = (MTLPixelFormat)toMetalPixelFormat(desc.pixelFormat);
    mtlTextureDescriptor.width = desc.width;
    mtlTextureDescriptor.height = desc.height;
    switch (desc.storageMode)
    {
    case StorageMode::Private:
        mtlTextureDescriptor.storageMode = MTLStorageModePrivate;
        break;
    case StorageMode::Shared:
        mtlTextureDescriptor.storageMode = MTLStorageModeShared;
        break;
    }
    switch (desc.usage)
    {
    case TextureUsage::ShaderRead:
        mtlTextureDescriptor.usage = MTLTextureUsageShaderRead;
        break;
    case TextureUsage::RenderTarget:
        mtlTextureDescriptor.usage = MTLTextureUsageRenderTarget;
        break;
    }

    m_mtlTexture = [device newTextureWithDescriptor:mtlTextureDescriptor];
    if (!m_mtlTexture)
        throw MTLTextureCreationError();
}}

utils::uint32 MetalTexture::width() { @autoreleasepool
{
    return (utils::uint32)m_mtlTexture.width;
}}

utils::uint32 MetalTexture::height() { @autoreleasepool
{
    return (utils::uint32)m_mtlTexture.height;
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
    if (m_mtlTexture != nullptr)
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
