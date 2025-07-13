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

#include <vulkan/vulkan.hpp>

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <memory>
    #include <cstdint>
    namespace ext = std;
#endif

namespace gfx
{

VulkanDrawable::VulkanDrawable(const VulkanDevice* device)
    : m_device(device)
{
    m_imageAvailableSemaphore = m_device->vkDevice().createSemaphore(vk::SemaphoreCreateInfo{});
    
#if !defined (NDEBUG)
    auto debugUtilsObjectNameInfo = vk::DebugUtilsObjectNameInfoEXT{}
        .setObjectHandle(reinterpret_cast<uint64_t>(static_cast<VkSemaphore>(m_imageAvailableSemaphore)))
        .setObjectType(vk::ObjectType::eSemaphore)
        .setPObjectName("drawable semaphore");
    m_device->vkDevice().setDebugUtilsObjectNameEXT(debugUtilsObjectNameInfo);
#endif
}

void VulkanDrawable::setSwapchainImage(const ext::shared_ptr<SwapchainImage>& swapchainImage, uint32_t imageIndex)
{
    m_swapchainImage = swapchainImage;
    m_imageIndex = imageIndex;
    m_swapchainImage->setImageAvailableSemaphoreRef(&m_imageAvailableSemaphore);
}

VulkanDrawable::~VulkanDrawable()
{
    m_device->vkDevice().destroySemaphore(m_imageAvailableSemaphore);
}

}
