/*
 * ---------------------------------------------------
 * MetalTexture.mm
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/06 18:20:36
 * ---------------------------------------------------
 */

#include "Graphics/Texture.hpp"
#include "Graphics/Enums.hpp"

#include "Metal/MetalDevice.hpp"
#include "Metal/MetalTexture.hpp"

#import "Metal/MetalEnums.hpp"

#include <cassert>

namespace gfx
{

MetalTexture::MetalTexture(const Texture::Descriptor& desc)
    : m_usages(desc.usages)
    , m_storageMode(desc.storageMode)
{
}

MetalTexture::MetalTexture(const MetalDevice& device, const Texture::Descriptor& desc)
    : m_usages(desc.usages), m_storageMode(desc.storageMode) { @autoreleasepool
{
    MTLTextureDescriptor* mtlTextureDescriptor = [[MTLTextureDescriptor alloc] init];
    mtlTextureDescriptor.textureType = toMTLTextureType(desc.type);
    mtlTextureDescriptor.pixelFormat = toMTLPixelFormat(desc.pixelFormat);
    mtlTextureDescriptor.width = desc.width;
    mtlTextureDescriptor.height = desc.height;
    if (desc.storageMode == ResourceStorageMode::deviceLocal)
        mtlTextureDescriptor.storageMode = MTLStorageModePrivate;
    mtlTextureDescriptor.usage = toMTLTextureUsage(desc.usages);

    m_mtlTexture = [device.mtlDevice() newTextureWithDescriptor:mtlTextureDescriptor];
    if (m_mtlTexture == nil)
        throw std::runtime_error("metal texture creation failed");
}}

TextureType MetalTexture::type() const { @autoreleasepool
{
    assert(m_mtlTexture);
    return toTextureType([m_mtlTexture textureType]);
}}

uint32_t MetalTexture::width() const { @autoreleasepool
{
    assert(m_mtlTexture);
    return static_cast<uint32_t>(m_mtlTexture.width);
}}

uint32_t MetalTexture::height() const { @autoreleasepool
{
    assert(m_mtlTexture);
    return static_cast<uint32_t>(m_mtlTexture.height);
}}

PixelFormat MetalTexture::pixelFormat() const { @autoreleasepool
{
    assert(m_mtlTexture);
    return toPixelFormat([m_mtlTexture pixelFormat]);
}}

}
