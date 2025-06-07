/*
 * ---------------------------------------------------
 * VulkanTexture.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/06 07:27:52
 * ---------------------------------------------------
 */

#include "Vulkan/VulkanTexture.hpp"

#include <Vulkan/vulkan.hpp>

namespace gfx
{

VulkanTexture::VulkanTexture(vk::Image vkImage, const Texture::Descriptor& desc)
    : m_shouldDestroyImage(false), m_vkImage(vkImage), m_pixelFormat(desc.pixelFormat) 
{
}

VulkanTexture::~VulkanTexture()
{
    if (m_shouldDestroyImage)
        m_device->vkDevice().destroyImage(m_vkImage);
}

}
