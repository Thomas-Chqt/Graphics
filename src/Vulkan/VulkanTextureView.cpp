#include "Vulkan/VulkanTextureView.hpp"

#include "Graphics/Texture.hpp"

#include "Vulkan/VulkanEnums.hpp"
#include "Vulkan/VulkanDevice.hpp"
#include "Vulkan/VulkanTexture.hpp"

#include <algorithm>

namespace gfx
{

VulkanTextureView::VulkanTextureView(const VulkanDevice* device, const std::shared_ptr<VulkanTexture>& rootTexture, const Texture::ViewDescriptor& desc)
    : m_device(device)
    , m_rootTexture(rootTexture)
    , m_type(desc.type)
    , m_baseMipLevel(desc.baseMipLevel)
    , m_mipLevelCount(desc.mipLevelCount)
    , m_baseArrayLayer(desc.baseArrayLayer)
    , m_arrayLayerCount(desc.arrayLayerCount)
{
    assert(m_rootTexture);
    assert(m_mipLevelCount > 0);
    assert(m_arrayLayerCount > 0);

    const uint32_t rootLayerCount = m_rootTexture->type() == TextureType::textureCube ? 6u : m_rootTexture->arrayLayerCount();
    const uint32_t viewLayerCount = m_type == TextureType::textureCube ? 6u : m_arrayLayerCount;

    assert(m_baseMipLevel + m_mipLevelCount <= m_rootTexture->mipLevelCount());
    assert(m_baseArrayLayer + viewLayerCount <= rootLayerCount);

    assert(desc.type == TextureType::texture2dArray || m_arrayLayerCount == 1);
    assert(desc.type != TextureType::texture2dArray || (m_rootTexture->type() == TextureType::texture2dArray || m_rootTexture->type() == TextureType::textureCube));
    assert(desc.type != TextureType::textureCube    || (m_rootTexture->type() == TextureType::textureCube && m_baseArrayLayer == 0));

    m_subresourceRange = vk::ImageSubresourceRange{}
        .setAspectMask(toVkImageAspectFlags(m_rootTexture->pixelFormat()))
        .setBaseMipLevel(m_baseMipLevel)
        .setLevelCount(m_mipLevelCount)
        .setBaseArrayLayer(m_baseArrayLayer)
        .setLayerCount(viewLayerCount);

    auto createInfo = vk::ImageViewCreateInfo{}
        .setImage(m_rootTexture->vkImage())
        .setViewType(toVkImageViewType(m_type))
        .setFormat(toVkFormat(m_rootTexture->pixelFormat()))
        .setComponents(vk::ComponentMapping{}
            .setR(vk::ComponentSwizzle::eIdentity)
            .setG(vk::ComponentSwizzle::eIdentity)
            .setB(vk::ComponentSwizzle::eIdentity)
            .setA(vk::ComponentSwizzle::eIdentity))
        .setSubresourceRange(m_subresourceRange);

    m_vkImageView = m_device->vkDevice().createImageView(createInfo);
}

VulkanTextureView::VulkanTextureView(const VulkanDevice* device, const std::shared_ptr<VulkanTextureView>& parent, const Texture::ViewDescriptor& desc)
    : VulkanTextureView(device, parent->rootTexture(), Texture::ViewDescriptor{
        .type = desc.type,
        .baseMipLevel = parent->baseMipLevel() + desc.baseMipLevel,
        .mipLevelCount = desc.mipLevelCount,
        .baseArrayLayer = parent->baseArrayLayer() + desc.baseArrayLayer,
        .arrayLayerCount = desc.arrayLayerCount
    })
{
}

TextureType VulkanTextureView::type() const
{
    return m_type;
}

uint32_t VulkanTextureView::width() const
{
    return std::max(m_rootTexture->width() >> m_baseMipLevel, 1u);
}

uint32_t VulkanTextureView::height() const
{
    return std::max(m_rootTexture->height() >> m_baseMipLevel, 1u);
}

uint32_t VulkanTextureView::baseMipLevel() const
{
    return m_baseMipLevel;
}

uint32_t VulkanTextureView::mipLevelCount() const
{
    return m_mipLevelCount;
}

uint32_t VulkanTextureView::baseArrayLayer() const
{
    return m_baseArrayLayer;
}

uint32_t VulkanTextureView::arrayLayerCount() const
{
    return m_arrayLayerCount;
}

PixelFormat VulkanTextureView::pixelFormat() const
{
    return m_rootTexture->pixelFormat();
}

TextureUsages VulkanTextureView::usages() const
{
    return m_rootTexture->usages();
}

ResourceStorageMode VulkanTextureView::storageMode() const
{
    return m_rootTexture->storageMode();
}

std::shared_ptr<VulkanTexture> VulkanTextureView::rootTexture()
{
    return m_rootTexture;
}

const vk::ImageView& VulkanTextureView::vkImageView() const
{
    return m_vkImageView;
}

const vk::ImageSubresourceRange& VulkanTextureView::subresourceRange() const
{
    return m_subresourceRange;
}

void VulkanTextureView::setImTextureId(uint64_t textureId, const std::shared_ptr<VulkanSampler>& sampler, ImTextureIdCleanup cleanup)
{
    removeImTextureId();
    m_imTextureId = textureId;
    m_imTextureIdSampler = sampler;
    m_imTextureIdCleanup = cleanup;
}

std::optional<uint64_t> VulkanTextureView::imTextureId() const
{
    return m_imTextureId;
}

void VulkanTextureView::removeImTextureId()
{
    if (m_imTextureId.has_value() && m_imTextureIdCleanup != nullptr)
        m_imTextureIdCleanup(*m_imTextureId);
    m_imTextureId.reset();
    m_imTextureIdSampler = nullptr;
    m_imTextureIdCleanup = nullptr;
}

VulkanTextureView::~VulkanTextureView()
{
    m_device->vkDevice().destroyImageView(m_vkImageView);
}

} // namespace gfx
