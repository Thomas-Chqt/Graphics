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

namespace gfx
{

MetalTexture::MetalTexture(const Texture::Descriptor& desc)
    : m_usages(desc.usages)
    , m_storageMode(desc.storageMode)
{
    assert(desc.width > 0 && desc.height > 0);
    assert(desc.mipLevelCount > 0);
    assert(desc.arrayLayerCount > 0);
    assert(desc.type == TextureType::texture2dArray || desc.arrayLayerCount == 1);
}

MetalTexture::MetalTexture(const MetalDevice& device, const Texture::Descriptor& desc)
    : MetalTexture(desc) { @autoreleasepool
{
    MTLTextureDescriptor* mtlTextureDescriptor = [[MTLTextureDescriptor alloc] init];
    mtlTextureDescriptor.textureType = toMTLTextureType(desc.type);
    mtlTextureDescriptor.pixelFormat = toMTLPixelFormat(desc.pixelFormat);
    mtlTextureDescriptor.width = desc.width;
    mtlTextureDescriptor.height = desc.height;
    mtlTextureDescriptor.mipmapLevelCount = desc.mipLevelCount;
    mtlTextureDescriptor.arrayLength = desc.arrayLayerCount;
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

uint32_t MetalTexture::baseMipLevel() const { @autoreleasepool
{
    assert(m_mtlTexture);
    return static_cast<uint32_t>(m_mtlTexture.parentRelativeLevel);
}}

uint32_t MetalTexture::mipLevelCount() const { @autoreleasepool
{
    assert(m_mtlTexture);
    return static_cast<uint32_t>(m_mtlTexture.mipmapLevelCount);
}}

uint32_t MetalTexture::baseArrayLayer() const { @autoreleasepool
{
    assert(m_mtlTexture);
    return static_cast<uint32_t>(m_mtlTexture.parentRelativeSlice);
}}

uint32_t MetalTexture::arrayLayerCount() const { @autoreleasepool
{
    assert(m_mtlTexture);
    return static_cast<uint32_t>(m_mtlTexture.arrayLength);
}}

PixelFormat MetalTexture::pixelFormat() const { @autoreleasepool
{
    assert(m_mtlTexture);
    return toPixelFormat([m_mtlTexture pixelFormat]);
}}

TextureUsages MetalTexture::usages() const { @autoreleasepool
{
    assert(m_mtlTexture);
    return m_usages;
}}

ResourceStorageMode MetalTexture::storageMode() const { @autoreleasepool
{
    assert(m_mtlTexture);
    return m_storageMode;
}}

}
