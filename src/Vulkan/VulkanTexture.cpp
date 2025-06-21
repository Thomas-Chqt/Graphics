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

#include <Vulkan/vulkan.hpp>

namespace gfx
{

VulkanTexture::VulkanTexture(vk::Image vkImage, const Texture::Descriptor& desc)
    : m_vkImage(vkImage),
      m_width(desc.width), m_height(desc.height),
      m_pixelFormat(desc.pixelFormat) 
{
}

VulkanTexture::~VulkanTexture()
{
    if (m_device)
        m_device->vkDevice().destroyImage(m_vkImage);
}

}
