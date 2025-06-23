/*
 * ---------------------------------------------------
 * VulkanDevice.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/05/28 06:27:45
 * ---------------------------------------------------
 */

#define VULKAN_HPP_NO_CONSTRUCTORS

#include "Graphics/Swapchain.hpp"
#include "Graphics/CommandBuffer.hpp"
#include "Graphics/Drawable.hpp"

#include "Vulkan/VulkanDevice.hpp"
#include "Vulkan/VulkanPhysicalDevice.hpp"
#include "Vulkan/VulkanSwapchain.hpp"
#include "Vulkan/VulkanCommandBuffer.hpp"
#include "Vulkan/VulkanDrawable.hpp"

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
    #include <cstddef>
    #include <cstring>
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

    auto dynamicRenderingFeature = vk::PhysicalDeviceDynamicRenderingFeatures{}
        .setDynamicRendering(vk::True);

    auto deviceCreateInfo = vk::DeviceCreateInfo{}
        .setEnabledExtensionCount(static_cast<uint32_t>(desc.deviceExtensions.size()))
        .setPpEnabledExtensionNames(desc.deviceExtensions.data())
        .setPEnabledFeatures(&deviceFeatures)
        .setQueueCreateInfos(queueCreateInfo);
    if (ext::ranges::find_if(desc.deviceExtensions, [](const char* s) { return ::strcmp(s, VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME) == 0; }) != desc.deviceExtensions.end())
        deviceCreateInfo.setPNext(&dynamicRenderingFeature);

    m_vkDevice = phyDevice.createDevice(deviceCreateInfo);
    m_queue = m_vkDevice.getQueue(m_queueFamily.index, 0);

    vk::CommandPoolCreateInfo commandPoolCreateInfo = {
        .queueFamilyIndex = m_queueFamily.index
    };

    m_commandPool = m_vkDevice.createCommandPool(commandPoolCreateInfo);
    m_frameCompletedFence = m_vkDevice.createFence(vk::FenceCreateInfo{.flags=vk::FenceCreateFlagBits::eSignaled});
}

ext::unique_ptr<Swapchain> VulkanDevice::newSwapchain(const Swapchain::Descriptor& desc) const
{
    ext::vector<ext::shared_ptr<VulkanDrawable>> drawables(MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        drawables[i] = std::make_shared<VulkanDrawable>(*this);
    return ext::make_unique<VulkanSwapchain>(*this, std::move(drawables), desc);
}

void VulkanDevice::beginFrame(void) 
{
    if (m_vkDevice.waitForFences(m_frameCompletedFence, true, ext::numeric_limits<uint64_t>::max()) != vk::Result::eSuccess)
        throw ext::runtime_error("failed to wait for fence");
    m_vkDevice.resetFences(m_frameCompletedFence);
    
    for (const auto& cmdBuf : m_submittedCommandBuffers)
    {
        if (m_nextVkCommandBuffer > 0)
            m_vkCommandBuffers[--m_nextVkCommandBuffer] = ext::move(cmdBuf->vkCommandBuffer());
        else
            m_vkCommandBuffers.push_back(ext::move(cmdBuf->vkCommandBuffer()));
    }
    m_submittedCommandBuffers.clear();
    m_vkDevice.resetCommandPool(m_commandPool);
}

ext::unique_ptr<CommandBuffer> VulkanDevice::commandBuffer(void)
{
    if (m_nextVkCommandBuffer < m_vkCommandBuffers.size())
        return ext::make_unique<VulkanCommandBuffer>(ext::move(m_vkCommandBuffers[m_nextVkCommandBuffer++]), m_queueFamily);
    else {
        auto commandBufferAllocateInfo = vk::CommandBufferAllocateInfo{}
            .setCommandPool(m_commandPool)
            .setLevel(vk::CommandBufferLevel::ePrimary)
            .setCommandBufferCount(1);
        vk::CommandBuffer vkCommandBuffer = m_vkDevice.allocateCommandBuffers(commandBufferAllocateInfo).front();
        return ext::make_unique<VulkanCommandBuffer>(ext::move(vkCommandBuffer), m_queueFamily);
    }
}

void VulkanDevice::submitCommandBuffer(ext::unique_ptr<CommandBuffer>&& commandBuffer)
{
    assert(dynamic_cast<VulkanCommandBuffer*>(commandBuffer.get()));
    m_submittedCommandBuffers.push_back(ext::unique_ptr<VulkanCommandBuffer>(dynamic_cast<VulkanCommandBuffer*>(commandBuffer.release())));
}

void VulkanDevice::presentDrawable(const ext::shared_ptr<Drawable>& _drawable)
{
    auto drawable = ext::dynamic_pointer_cast<VulkanDrawable>(_drawable);
    auto imageMemoryBarrier = vk::ImageMemoryBarrier{}
        .setSrcAccessMask(vk::AccessFlagBits{})
        .setDstAccessMask(vk::AccessFlagBits::eTransferWrite)
        .setOldLayout(vk::ImageLayout::eColorAttachmentOptimal)
        .setNewLayout(vk::ImageLayout::ePresentSrcKHR)
        .setSrcQueueFamilyIndex(m_queueFamily.index)
        .setDstQueueFamilyIndex(m_queueFamily.index)
        .setImage(drawable->swapchainImage()->vkImage())
        .setSubresourceRange({
            .aspectMask = vk::ImageAspectFlagBits::eColor,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1,
        });

    m_submittedCommandBuffers.back()->vkCommandBuffer().pipelineBarrier(vk::PipelineStageFlagBits::eColorAttachmentOutput,
                                                                        vk::PipelineStageFlagBits::eTransfer,
                                                                        {}, nullptr, nullptr, imageMemoryBarrier);

    m_presentedDrawables.push_back(ext::dynamic_pointer_cast<VulkanDrawable>(drawable));
}

void VulkanDevice::endFrame(void)
{
    for (size_t i = 0; auto& buffer : m_submittedCommandBuffers)
        buffer->vkCommandBuffer().end();

    ext::vector<vk::Semaphore> waitSemaphores = m_presentedDrawables
        | ext::views::transform([](auto& d){return d->imageAvailableSemaphore();})
        | ext::ranges::to<ext::vector>();

    ext::vector<vk::PipelineStageFlags> waitStages (m_presentedDrawables.size(), vk::PipelineStageFlagBits::eColorAttachmentOutput);

    ext::vector<vk::CommandBuffer> vkCommandBuffers = m_submittedCommandBuffers
        | ext::views::transform([](auto& b){return b->vkCommandBuffer();})
        | ext::ranges::to<ext::vector>();

    ext::vector<vk::Semaphore> signalSemaphores = m_presentedDrawables
        | ext::views::transform([](auto& d){return d->swapchainImage()->imagePresentableSemaphore();})
        | ext::ranges::to<ext::vector>();

    auto submitInfo = vk::SubmitInfo{}
        .setWaitSemaphores(waitSemaphores)
        .setPWaitDstStageMask(waitStages.data())
        .setCommandBuffers(vkCommandBuffers)
        .setSignalSemaphores(signalSemaphores);
    
    m_queue.submit(submitInfo, m_frameCompletedFence);
    
    ext::vector<vk::SwapchainKHR> swapchains = m_presentedDrawables
        | ext::views::transform([](auto& d){return d->swapchainImage()->swapchain().vkSwapchain();})
        | ext::ranges::to<ext::vector>();

    ext::vector<uint32_t> imageIndices = m_presentedDrawables
        | ext::views::transform([](auto& d){return d->swapchainImage()->imageIndex();})
        | ext::ranges::to<ext::vector>();

    auto presentInfo = vk::PresentInfoKHR{}
        .setWaitSemaphores(signalSemaphores)
        .setSwapchains(swapchains)
        .setImageIndices(imageIndices);

    vk::Result result = m_queue.presentKHR(&presentInfo);

    m_presentedDrawables.clear();

    if (result != vk::Result::eSuccess)
        throw std::runtime_error("failed to present swap chain image!");
}

void VulkanDevice::waitIdle(void)
{
    m_vkDevice.waitIdle();
}

VulkanDevice::~VulkanDevice()
{
    m_submittedCommandBuffers.clear();

    m_vkDevice.destroyFence(m_frameCompletedFence);
    m_vkDevice.destroyCommandPool(m_commandPool);
    m_vkDevice.destroy();
}

} // namespace gfx
