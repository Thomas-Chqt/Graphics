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
#include "Graphics/Texture.hpp"
#include "UtilsCPP/Types.hpp"
#include <Metal/MTLTypes.h>
#include <Metal/Metal.h>

namespace gfx
{

MTLTextureType toMTLTextureType(Texture::Type type)
{
    switch (type)
    {
    case Texture::Type::texture2d:
        return MTLTextureType2D;
    case Texture::Type::textureCube:
        return MTLTextureTypeCube;
    }
}

MTLTextureUsage toMTLTextureUsage(Texture::Usage usage)
{
    switch (usage)
    {
    case Texture::Usage::ShaderRead:
        return MTLTextureUsageShaderRead;
    case Texture::Usage::RenderTarget:
        return MTLTextureUsageRenderTarget;
    case Texture::Usage::ShaderReadAndRenderTarget:
        return MTLTextureUsageShaderRead | MTLTextureUsageRenderTarget;
    }
}

MetalTexture::MetalTexture(MetalTexture&& mv) noexcept : m_mtlTexture(mv.m_mtlTexture)
{
    mv.m_mtlTexture = nullptr;
}

MetalTexture::MetalTexture(id<MTLDevice> device, const Texture::Descriptor& desc) { @autoreleasepool
{
    MTLTextureDescriptor* mtlTextureDescriptor = [[[MTLTextureDescriptor alloc] init] autorelease];
    mtlTextureDescriptor.textureType = toMTLTextureType(desc.type);
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
    case StorageMode::Managed:
        mtlTextureDescriptor.storageMode = MTLStorageModeManaged;
        break;
    }
    mtlTextureDescriptor.usage = toMTLTextureUsage(desc.usage);

    m_mtlTexture = [device newTextureWithDescriptor:mtlTextureDescriptor];
    if (m_mtlTexture == nil)
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

void MetalTexture::replaceRegion(const Region& region, utils::uint32 slice, const void* bytes) { @autoreleasepool
{
    [m_mtlTexture replaceRegion:MTLRegionMake2D(region.offsetX, region.offsetY, region.width, region.height)
                    mipmapLevel:0
                          slice:slice
                      withBytes:bytes
                    bytesPerRow:sizeof(utils::uint32) * region.width
                  bytesPerImage:0];
}}

MetalTexture::~MetalTexture() { @autoreleasepool
{
    if (m_mtlTexture != nullptr)
        [m_mtlTexture release];
}}

MetalTexture& MetalTexture::operator = (MetalTexture&& mv) noexcept
{
    if (this != &mv)
    {
        m_mtlTexture = mv.m_mtlTexture;
        mv.m_mtlTexture = nullptr;
    }
    return *this;
}

}
