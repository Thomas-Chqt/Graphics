/*
 * ---------------------------------------------------
 * SwapchainImage.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/21 17:30:58
 * ---------------------------------------------------
 */

#include "Vulkan/SwapchainImage.hpp"
#include "Vulkan/VulkanDevice.hpp"
#include "Vulkan/VulkanTexture.hpp"

namespace gfx
{

SwapchainImage::SwapchainImage(const VulkanDevice* device, vk::Image&& image, const ext::shared_ptr<vk::SwapchainKHR>& swapchain, const Texture::Descriptor& desc)
    : VulkanTexture(device, ext::move(image), desc), m_swapchain(swapchain)
{
    m_imagePresentableSemaphore = m_device->vkDevice().createSemaphore(vk::SemaphoreCreateInfo{});

#if !defined (NDEBUG)
    auto debugUtilsObjectNameInfo = vk::DebugUtilsObjectNameInfoEXT{}
        .setObjectHandle(ext::bit_cast<uint64_t>(static_cast<VkSemaphore>(m_imagePresentableSemaphore)))
        .setObjectType(vk::ObjectType::eSemaphore)
        .setPObjectName("imagePresentableSemaphore");
    m_device->vkDevice().setDebugUtilsObjectNameEXT(debugUtilsObjectNameInfo);
#endif
}

SwapchainImage::~SwapchainImage()
{
    m_device->vkDevice().destroySemaphore(m_imagePresentableSemaphore);
}

}
