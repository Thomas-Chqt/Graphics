/*
 * ---------------------------------------------------
 * VulkanDevice.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/05/28 06:27:45
 * ---------------------------------------------------
 */

#define VULKAN_HPP_NO_CONSTRUCTORS

#include "Graphics/RenderPass.hpp"
#include "Graphics/Swapchain.hpp"
#include "Graphics/CommandBuffer.hpp"

#include "Vulkan/VulkanDevice.hpp"
#include "Vulkan/VulkanPhysicalDevice.hpp"
#include "Vulkan/VulkanRenderPass.hpp"
#include "Vulkan/VulkanSwapchain.hpp"
#include "Vulkan/VulkanCommandBuffer.hpp"

#include <vulkan/vulkan.hpp>

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <vector>
    #include <cstdint>
    #include <cassert>
    #include <memory>
    #include <ranges>
    #include <stdexcept>
    namespace ext = std;
#endif

namespace gfx
{

VulkanDevice::VulkanDevice(const VulkanPhysicalDevice& phyDevice, const VulkanDevice::Descriptor& desc)
    : m_physicalDevice(&phyDevice)
{
    assert((phyDevice.getQueueFamilies() | ext::views::filter([&desc](auto f){ return f.hasCapabilities(desc.deviceDescriptor->queueCaps); })).empty() == false);

    m_queueFamily = (phyDevice.getQueueFamilies() | ext::views::filter([&desc](auto f){ return f.hasCapabilities(desc.deviceDescriptor->queueCaps); })).front();

    float queuePriority = 1.0f;
    vk::DeviceQueueCreateInfo queueCreateInfo = {
        .queueFamilyIndex = m_queueFamily.index,
        .queueCount = 1,                      
        .pQueuePriorities = &queuePriority};

    vk::PhysicalDeviceFeatures deviceFeatures{};

    vk::DeviceCreateInfo deviceCreateInfo = {
        .enabledExtensionCount = static_cast<uint32_t>(desc.deviceExtensions.size()),
        .ppEnabledExtensionNames = desc.deviceExtensions.data(),
        .pEnabledFeatures = &deviceFeatures,
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &queueCreateInfo};

    m_vkDevice = phyDevice.createDevice(deviceCreateInfo);
    m_queue = m_vkDevice.getQueue(m_queueFamily.index, 0);

    vk::CommandPoolCreateInfo commandPoolCreateInfo = {
        .queueFamilyIndex = m_queueFamily.index
    };

    m_commandPool = m_vkDevice.createCommandPool(commandPoolCreateInfo);
    m_commandBuffer = VulkanCommandBuffer(*this, m_commandPool);

    m_imageAvailableSemaphore = m_vkDevice.createSemaphore(vk::SemaphoreCreateInfo{});
    m_renderFinisedSemaphore = m_vkDevice.createSemaphore(vk::SemaphoreCreateInfo{});
    m_renderFinisedFence = m_vkDevice.createFence(vk::FenceCreateInfo{.flags=vk::FenceCreateFlagBits::eSignaled});
}

ext::unique_ptr<RenderPass> VulkanDevice::newRenderPass(const RenderPass::Descriptor& desc) const
{
    return ext::make_unique<VulkanRenderPass>(*this, desc);
}

ext::unique_ptr<Swapchain> VulkanDevice::newSwapchain(const Swapchain::Descriptor& desc) const
{
    return ext::make_unique<VulkanSwapchain>(*this, desc);
}

void VulkanDevice::beginFrame(void) 
{
    if (m_vkDevice.waitForFences(m_renderFinisedFence, true, ext::numeric_limits<uint64_t>::max()) != vk::Result::eSuccess)
        throw ext::runtime_error("failed to wait for fence");
    m_vkDevice.resetFences(m_renderFinisedFence);

    m_commandBuffer.reset();
    m_vkDevice.resetCommandPool(m_commandPool);
}

CommandBuffer& VulkanDevice::commandBuffer(void)
{
    return m_commandBuffer;
}

void VulkanDevice::submitCommandBuffer(const CommandBuffer& _commandBuffer)
{
    const VulkanCommandBuffer& commandBuffer = dynamic_cast<const VulkanCommandBuffer&>(_commandBuffer);
    assert(&commandBuffer == &m_commandBuffer);
}

void VulkanDevice::presentSwapchain(const Swapchain& _swapchain)
{
    const VulkanSwapchain& swapchain = dynamic_cast<const VulkanSwapchain&>(_swapchain);

    vk::ImageMemoryBarrier imageMemoryBarrier = {
        .newLayout = vk::ImageLayout::ePresentSrcKHR,
        .srcQueueFamilyIndex = m_queueFamily.index,
        .dstQueueFamilyIndex = m_queueFamily.index,
        .subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor,
        .subresourceRange.baseMipLevel = 0,
        .subresourceRange.levelCount = 1,
        .subresourceRange.baseArrayLayer = 0,
        .subresourceRange.layerCount = 1,
        .srcAccessMask = vk::AccessFlagBits{},
        .dstAccessMask = vk::AccessFlagBits::eTransferWrite
    };

    m_commandBuffer.addImageMemoryBarrier(swapchain.currentTexture(), imageMemoryBarrier);

    m_presentedSwapchain = &swapchain;
}

void VulkanDevice::endFrame(void)
{
    m_commandBuffer.vkCommandBuffer().end();

    vk::PipelineStageFlags waitStages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
    vk::SubmitInfo submitInfo = {
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &m_imageAvailableSemaphore,
        .pWaitDstStageMask = waitStages,
        .commandBufferCount = 1,
        .pCommandBuffers = &m_commandBuffer.vkCommandBuffer(),
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = &m_renderFinisedSemaphore
    };
    
    m_queue.submit(submitInfo, m_renderFinisedFence);

    vk::PresentInfoKHR presentInfo = {
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &m_renderFinisedSemaphore,
        .swapchainCount = 1,
        .pSwapchains = &m_presentedSwapchain->vkSwapchain(),
        .pImageIndices = &m_presentedSwapchain->currentImageIndex()
    };

    vk::Result result = m_queue.presentKHR(&presentInfo);
    if (result != vk::Result::eSuccess)
        throw std::runtime_error("failed to present swap chain image!");
}

VulkanDevice::~VulkanDevice()
{
    (void)m_vkDevice.waitForFences(m_renderFinisedFence, true, ext::numeric_limits<uint64_t>::max());
    m_vkDevice.resetFences(m_renderFinisedFence);

    m_commandBuffer.reset();
    m_vkDevice.resetCommandPool(m_commandPool);

    m_vkDevice.destroyFence(m_renderFinisedFence);

    m_vkDevice.destroySemaphore(m_renderFinisedSemaphore);
    m_vkDevice.destroySemaphore(m_imageAvailableSemaphore);

    m_vkDevice.destroyCommandPool(m_commandPool);

    m_vkDevice.destroy();
}

} // namespace gfx
