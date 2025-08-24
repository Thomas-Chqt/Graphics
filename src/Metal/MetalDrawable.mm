/*
 * ---------------------------------------------------
 * MetalDrawable.mm
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/23 13:03:22
 * ---------------------------------------------------
 */

#include "Graphics/Enums.hpp"
#include "Graphics/Texture.hpp"

#include "Metal/MetalDrawable.hpp"
#include "Metal/MetalEnums.h"
#include "Metal/MetalTexture.hpp"

#import <QuartzCore/CAMetalLayer.h>

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <memory>
    #include <cstdint>
    namespace ext = std;
#endif

namespace gfx
{

MetalDrawable::MetalDrawable(const MetalDevice* device, id<CAMetalDrawable> mtlDrawable)
    : m_device(device) { @autoreleasepool
{
    m_mtlDrawable = [mtlDrawable retain];
}}

ext::shared_ptr<Texture> MetalDrawable::texture() const { @autoreleasepool
{
    id<MTLTexture> mtlTexture = m_mtlDrawable.texture;
    Texture::Descriptor desc = {
        .type = TextureType::texture2d,
        .width = static_cast<uint32_t>(mtlTexture.width),
        .height = static_cast<uint32_t>(mtlTexture.height),
        .pixelFormat = toPixelFormat(mtlTexture.pixelFormat),
        .usages = TextureUsage::colorAttachment,
        .storageMode = ResourceStorageMode::deviceLocal
    };
    return ext::make_shared<MetalTexture>(m_device, m_mtlDrawable.texture, desc);
}}

MetalDrawable::~MetalDrawable()
{
    [m_mtlDrawable release];
}

}
