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

namespace gfx
{

MetalDrawable::MetalDrawable(id<CAMetalDrawable> mtlDrawable) { @autoreleasepool
{
    m_mtlDrawable = [mtlDrawable retain];
}}

std::shared_ptr<Texture> MetalDrawable::texture() const { @autoreleasepool
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
    return std::make_shared<MetalTexture>(m_mtlDrawable.texture, desc);
}}

MetalDrawable::~MetalDrawable()
{
    [m_mtlDrawable release];
}

}
