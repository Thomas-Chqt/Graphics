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

#include <Metal/Metal.h>

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <stdexcept>
    namespace ext = std;
#endif

namespace gfx
{

MetalTexture::MetalTexture(const MetalDevice* device, const id<MTLTexture>& mtltexture)
    : m_device(device), m_imageCount(1) { @autoreleasepool
{
    m_frameDatas.front().mtlTexture = [mtltexture retain];
}}

MetalTexture::MetalTexture(const MetalDevice* device, const Texture::Descriptor& desc)
    : m_device(device) { @autoreleasepool
{
    if (desc.usages & TextureUsage::depthStencilAttachment ||
        desc.storageMode == ResourceStorageMode::hostVisible) {
        m_imageCount = 3;
    } else {
        m_imageCount = 1;
    }

    MTLTextureDescriptor* mtlTextureDescriptor = [[[MTLTextureDescriptor alloc] init] autorelease];
    mtlTextureDescriptor.textureType = MTLTextureType2D;
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
