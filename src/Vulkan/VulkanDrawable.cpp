/*
 * ---------------------------------------------------
 * VulkanDrawable.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/21 17:07:48
 * ---------------------------------------------------
 */

#include "Vulkan/VulkanDrawable.hpp"

#include "Vulkan/VulkanDevice.hpp"

namespace gfx
{

VulkanDrawable::VulkanDrawable(const VulkanDevice* device)
    : m_device(device)
{
    m_imageAvailableSemaphore = m_device->vkDevice().createSemaphore(vk::SemaphoreCreateInfo{});

    #if !defined(NDEBUG)
    auto debugUtilsObjectNameInfo = vk::DebugUtilsObjectNameInfoEXT{}
        .setObjectHandle(std::bit_cast<uint64_t>(static_cast<VkSemaphore>(m_imageAvailableSemaphore)))
        .setObjectType(vk::ObjectType::eSemaphore)
        .setPObjectName("imageAvailableSemaphore");
    m_device->vkDevice().setDebugUtilsObjectNameEXT(debugUtilsObjectNameInfo);
    #endif
}

void VulkanDrawable::setSwapchainImage(std::shared_ptr<SwapchainImage> swapchainImage, std::shared_ptr<VulkanTextureView> swapchainImageView, uint32_t imageIndex)
{
    m_swapchainImage = std::move(swapchainImage);
    m_swapchainImageView = std::move(swapchainImageView);
    m_imageIndex = imageIndex;
    m_swapchainImage->setImageAvailableSemaphoreRef(&m_imageAvailableSemaphore);
}

VulkanDrawable::~VulkanDrawable()
{
    m_device->vkDevice().destroySemaphore(m_imageAvailableSemaphore);
}

}
