#include "Metal/MetalTextureView.hpp"

#include "Metal/MetalEnums.hpp"
#include "Metal/MetalTexture.hpp"

namespace gfx
{

MetalTextureView::MetalTextureView(const std::shared_ptr<MetalTexture>& parent, const Texture::ViewDescriptor& desc) { @autoreleasepool
{
    assert(parent);
    assert(desc.mipLevelCount > 0);
    assert(desc.arrayLayerCount > 0);

    const uint32_t parentSliceCount = parent->type() == TextureType::textureCube ? 6u : parent->arrayLayerCount();
    const uint32_t viewSliceCount = desc.type == TextureType::textureCube ? 6u : desc.arrayLayerCount;

    assert(desc.baseMipLevel + desc.mipLevelCount <= parent->mipLevelCount());
    assert(desc.baseArrayLayer + viewSliceCount <= parentSliceCount);

    assert(desc.type == TextureType::texture2dArray || desc.arrayLayerCount == 1);
    assert(desc.type != TextureType::texture2dArray || (parent->type() == TextureType::texture2dArray || parent->type() == TextureType::textureCube));
    assert(desc.type != TextureType::textureCube || (parent->type() == TextureType::textureCube && desc.baseArrayLayer + 6 <= 6));

    if (auto parentView = std::dynamic_pointer_cast<MetalTextureView>(parent))
        m_rootTexture = parentView->rootTexture();
    else
        m_rootTexture = parent;

    m_mtlTexture = [m_rootTexture->mtltexture() newTextureViewWithPixelFormat:m_rootTexture->mtltexture().pixelFormat
                                                                  textureType:toMTLTextureType(desc.type)
                                                                       levels:NSMakeRange(parent->baseMipLevel() + desc.baseMipLevel, desc.mipLevelCount)
                                                                       slices:NSMakeRange(parent->baseArrayLayer() + desc.baseArrayLayer, viewSliceCount)];
    if (m_mtlTexture == nil)
        throw std::runtime_error("metal texture view creation failed");
}}

TextureUsages MetalTextureView::usages() const
{
    return m_rootTexture->usages();
}

ResourceStorageMode MetalTextureView::storageMode() const
{
    return m_rootTexture->storageMode();
}

} // namespace gfx
