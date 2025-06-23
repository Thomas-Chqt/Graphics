/*
 * ---------------------------------------------------
 * VulkanDrawable.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/21 17:07:48
 * ---------------------------------------------------
 */

#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1

#include "Vulkan/VulkanDrawable.hpp"
#include "Vulkan/VulkanDevice.hpp"

#include <vulkan/vulkan.hpp>

namespace gfx
{

VulkanDrawable::VulkanDrawable(const VulkanDevice& device)
    : m_device(&device)
{
    m_imageAvailableSemaphore = device.vkDevice().createSemaphore(vk::SemaphoreCreateInfo{});
    
#if !defined (NDEBUG)
    auto debugUtilsObjectNameInfo = vk::DebugUtilsObjectNameInfoEXT{}
        .setObjectHandle(reinterpret_cast<uint64_t>(static_cast<VkSemaphore>(m_imageAvailableSemaphore)))
        .setObjectType(vk::ObjectType::eSemaphore)
        .setPObjectName("drawable semaphore");
    device.vkDevice().setDebugUtilsObjectNameEXT(debugUtilsObjectNameInfo);
#endif
}

void VulkanDrawable::setSwapchainImage(const ext::shared_ptr<SwapchainImage>& image)
{
    m_swapchainImage = image;
    m_swapchainImage->setImageAvailableSemaphoreRef(&m_imageAvailableSemaphore);
}

VulkanDrawable::~VulkanDrawable()
{
    m_device->vkDevice().destroySemaphore(m_imageAvailableSemaphore);
}

}
