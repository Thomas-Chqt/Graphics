/*
 * ---------------------------------------------------
 * VulkanTexture.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/06 07:27:52
 * ---------------------------------------------------
 */

#define VULKAN_HPP_NO_CONSTRUCTORS

#include "Vulkan/VulkanTexture.hpp"
#include "Vulkan/VulkanDevice.hpp"
#include "Vulkan/VulkanEnums.hpp"

#include <Vulkan/vulkan.hpp>

namespace gfx
{

VulkanTexture::VulkanTexture(const VulkanDevice& device, vk::Image vkImage, const Texture::Descriptor& desc)
    : m_device(&device),
      m_vkImage(vkImage), m_shouldDestroyImg(false),
      m_width(desc.width), m_height(desc.height),
      m_pixelFormat(desc.pixelFormat)
{
    auto imageViewCreateInfo = vk::ImageViewCreateInfo{}
        .setImage(m_vkImage)
        .setViewType(vk::ImageViewType::e2D)
        .setFormat(toVkFormat(desc.pixelFormat))
        .setComponents({
            .r = vk::ComponentSwizzle::eIdentity,
            .g = vk::ComponentSwizzle::eIdentity,
            .b = vk::ComponentSwizzle::eIdentity,
            .a = vk::ComponentSwizzle::eIdentity
        })
        .setSubresourceRange({
            .aspectMask = vk::ImageAspectFlagBits::eColor,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1
        });

    m_vkImageView = m_device->vkDevice().createImageView(imageViewCreateInfo);
}

VulkanTexture::~VulkanTexture()
{
    m_device->vkDevice().destroyImageView(m_vkImageView);

    if (m_shouldDestroyImg)
        m_device->vkDevice().destroyImage(m_vkImage);
}

} // namespace gfx
