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
#include "Metal/MetalEnums.hpp"
#include "Metal/MetalTexture.hpp"

#include <memory>

namespace gfx
{

MetalDrawable::MetalDrawable(const Texture::Descriptor& textureDescriptor)
    : m_texture(std::make_shared<MetalTexture>(textureDescriptor))
{
}

std::shared_ptr<Texture> MetalDrawable::texture() const { @autoreleasepool
{
    m_texture->setMtlTexture(m_mtlDrawable.texture);
    return m_texture;
}}

void MetalDrawable::setMtlDrawable(const id<CAMetalDrawable>& drawable)
{
    m_mtlDrawable = drawable;
    m_texture->setMtlTexture(nil);
}

}
