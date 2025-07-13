/*
 * ---------------------------------------------------
 * VulkanTexture.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/06 07:27:52
 * ---------------------------------------------------
 */

#include "Vulkan/VulkanTexture.hpp"
#include "Vulkan/VulkanDevice.hpp"
#include "Vulkan/VulkanEnums.hpp"

#include <vulkan/vulkan.hpp>

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <utility>
    namespace ext = std;
#endif

namespace gfx
{

VulkanTexture::VulkanTexture(const VulkanDevice* device, vk::Image&& vkImage, const Texture::Descriptor& desc)
    : m_device(device),
      m_vkImage(ext::move(vkImage)), m_shouldDestroyImg(false),
      m_width(desc.width), m_height(desc.height),
      m_pixelFormat(desc.pixelFormat)
{
    m_subresourceRange = vk::ImageSubresourceRange{}
        .setAspectMask(vk::ImageAspectFlagBits::eColor)
        .setBaseMipLevel(0)
        .setLevelCount(1)
        .setBaseArrayLayer(0)
        .setLayerCount(1);

    auto imageViewCreateInfo = vk::ImageViewCreateInfo{}
        .setImage(m_vkImage)
        .setViewType(vk::ImageViewType::e2D)
        .setFormat(toVkFormat(desc.pixelFormat))
        .setComponents(vk::ComponentMapping{}
            .setR(vk::ComponentSwizzle::eIdentity)
            .setG(vk::ComponentSwizzle::eIdentity)
            .setB(vk::ComponentSwizzle::eIdentity)
            .setA(vk::ComponentSwizzle::eIdentity))
        .setSubresourceRange(m_subresourceRange);

    m_vkImageView = m_device->vkDevice().createImageView(imageViewCreateInfo);
}

VulkanTexture::~VulkanTexture()
{
    m_device->vkDevice().destroyImageView(m_vkImageView);

    if (m_shouldDestroyImg)
        m_device->vkDevice().destroyImage(m_vkImage);
}

} // namespace gfx
