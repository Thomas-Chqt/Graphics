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

#import "Metal/MetalEnums.h"

namespace gfx
{

MetalTexture::MetalTexture(const MetalDevice* device, const id<MTLTexture>& mtltexture, const Texture::Descriptor& desc)
    : m_device(device),
      m_usages(desc.usages),
      m_storageMode(desc.storageMode),
      m_imageCount(1) { @autoreleasepool
{
    m_frameDatas.front().mtlTexture = [mtltexture retain];
}}

MetalTexture::MetalTexture(const MetalDevice* device, const Texture::Descriptor& desc)
    : m_device(device),
      m_usages(desc.usages),
      m_storageMode(desc.storageMode) { @autoreleasepool
{
    if (desc.usages & TextureUsage::depthStencilAttachment ||
        desc.storageMode == ResourceStorageMode::hostVisible) {
        m_imageCount = m_frameDatas.size();
    } else {
        m_imageCount = 1;
    }

    MTLTextureDescriptor* mtlTextureDescriptor = [[[MTLTextureDescriptor alloc] init] autorelease];
    mtlTextureDescriptor.textureType = toMTLTextureType(desc.type);
    mtlTextureDescriptor.pixelFormat = toMTLPixelFormat(desc.pixelFormat);
    mtlTextureDescriptor.width = desc.width;
    mtlTextureDescriptor.height = desc.height;
    if (desc.storageMode == ResourceStorageMode::deviceLocal)
        mtlTextureDescriptor.storageMode = MTLStorageModePrivate;
    mtlTextureDescriptor.usage = toMTLTextureUsage(desc.usages);

    for (uint8_t i = 0; i < m_imageCount; i++)
    {
        FrameData& frameData = m_frameDatas.at(i);
        frameData.mtlTexture = [m_device->mtlDevice() newTextureWithDescriptor:mtlTextureDescriptor];
        if (frameData.mtlTexture == nil)
            throw ext::runtime_error("metal texture creation failed");
    }
}}

uint32_t MetalTexture::width() const { @autoreleasepool
{
    return static_cast<uint32_t>(mtltexture().width);
}}

uint32_t MetalTexture::height() const { @autoreleasepool
{
    return static_cast<uint32_t>(mtltexture().height);
}}

PixelFormat MetalTexture::pixelFormat() const { @autoreleasepool
{
    return toPixelFormat([mtltexture() pixelFormat]);   
}}

void MetalTexture::replaceContent(void* data) { @autoreleasepool
{
    for (uint8_t i = 0; i < m_imageCount; i++)
    {
        FrameData& frameData = m_frameDatas.at(i);
        [frameData.mtlTexture replaceRegion:MTLRegionMake2D(0, 0, width(), height())
                                mipmapLevel:0
                                      slice:0
                                  withBytes:data
                                bytesPerRow:sizeof(uint32_t) * width()
                              bytesPerImage:0];
    }
}}

MetalTexture::~MetalTexture() { @autoreleasepool
{
    for (uint8_t i = 0; i < m_imageCount; i++)
    {
        FrameData& frameData = m_frameDatas.at(i);
        [frameData.mtlTexture release];
    }
}}

MetalTexture::FrameData& MetalTexture::currentFrameData()
{
    return m_frameDatas.at(m_device->currentFrameIdx() % m_imageCount);
}

const MetalTexture::FrameData& MetalTexture::currentFrameData() const
{
    return m_frameDatas.at(m_device->currentFrameIdx() % m_imageCount);
}

}
