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
#include "Vulkan/VulkanShaderLib.hpp"
#include "Vulkan/VulkanGraphicsPipeline.hpp"

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
    #include <utility>
    #include <queue>
    namespace ext = std;
#endif

namespace gfx
{

VulkanDevice::VulkanDevice(const VulkanPhysicalDevice& phyDevice, const VulkanDevice::Descriptor& desc)
    : m_physicalDevice(&phyDevice), m_frameData(desc.deviceDescriptor->maxFrameInFlight), m_currFD(m_frameData.begin())
{
    assert((phyDevice.getQueueFamilies() | ext::views::filter([&desc](auto f){ return f.hasCapabilities(desc.deviceDescriptor->queueCaps); })).empty() == false);

    auto dynamicRenderingFeature = vk::PhysicalDeviceDynamicRenderingFeatures{}
        .setDynamicRendering(vk::True);

    m_queueFamily = (phyDevice.getQueueFamilies() | ext::views::filter([&desc](auto f){ return f.hasCapabilities(desc.deviceDescriptor->queueCaps); })).front();
    float queuePriority = 1.0f;
    auto queueCreateInfo = vk::DeviceQueueCreateInfo{}
        .setQueueFamilyIndex(m_queueFamily.index)
        .setQueueCount(1)
        .setPQueuePriorities(&queuePriority);
    
    ext::vector<const char*> enabledExtensions = desc.deviceExtensions;
    if (phyDevice.getProperties().apiVersion < VK_API_VERSION_1_3)
        enabledExtensions.push_back(VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME);

    vk::PhysicalDeviceFeatures deviceFeatures{};

    auto deviceCreateInfo = vk::DeviceCreateInfo{}
        .setPNext(&dynamicRenderingFeature)
        .setQueueCreateInfos(queueCreateInfo)
        .setEnabledExtensionCount(static_cast<uint32_t>(enabledExtensions.size()))
        .setPpEnabledExtensionNames(enabledExtensions.data())
        .setPEnabledFeatures(&deviceFeatures);

    m_vkDevice = phyDevice.createDevice(deviceCreateInfo);
    m_queue = m_vkDevice.getQueue(m_queueFamily.index, 0);

    auto commandPoolCreateInfo = vk::CommandPoolCreateInfo{}
        .setQueueFamilyIndex(m_queueFamily.index);

    for (auto& fd : m_frameData) {
        fd.commandPool = m_vkDevice.createCommandPool(commandPoolCreateInfo);
        fd.frameCompletedFence = m_vkDevice.createFence(vk::FenceCreateInfo{.flags=vk::FenceCreateFlagBits::eSignaled});
    }
}

ext::unique_ptr<Swapchain> VulkanDevice::newSwapchain(const Swapchain::Descriptor& desc) const
{
    return ext::make_unique<VulkanSwapchain>(this, desc);
}

ext::unique_ptr<ShaderLib> VulkanDevice::newShaderLib(const ext::filesystem::path& path) const
{
    return ext::make_unique<VulkanShaderLib>(*this, path);
}

ext::unique_ptr<GraphicsPipeline> VulkanDevice::newGraphicsPipeline(const GraphicsPipeline::Descriptor& desc) const
{
    return ext::make_unique<VulkanGraphicsPipeline>(this, desc);
}

void VulkanDevice::beginFrame(void) 
{
    if (m_vkDevice.waitForFences(m_currFD->frameCompletedFence, true, ext::numeric_limits<uint64_t>::max()) != vk::Result::eSuccess)
        throw ext::runtime_error("failed to wait for fence");
    
    m_currFD->presentedDrawables.clear();
    m_currFD->submittedCmdBuffers.clear();
    while(m_currFD->usedCommandBuffers.empty() == false)
    {
        m_currFD->usedCommandBuffers.front().reset();
        m_currFD->commandBuffers.push(ext::move(m_currFD->usedCommandBuffers.front()));
        m_currFD->usedCommandBuffers.pop();
    }
    m_vkDevice.resetCommandPool(m_currFD->commandPool);
}

CommandBuffer& VulkanDevice::commandBuffer(void)
{
    bool useDynamicRenderingExt = false;
    if (m_physicalDevice->getProperties().apiVersion < VK_API_VERSION_1_3)
        useDynamicRenderingExt = true;

    if (m_currFD->commandBuffers.empty() == false) {
        m_currFD->usedCommandBuffers.push(ext::move(m_currFD->commandBuffers.front()));
        m_currFD->commandBuffers.pop();
    }
    else {
        auto commandBufferAllocateInfo = vk::CommandBufferAllocateInfo{}
            .setCommandPool(m_currFD->commandPool)
            .setLevel(vk::CommandBufferLevel::ePrimary)
            .setCommandBufferCount(1);
        vk::CommandBuffer vkCommandBuffer = m_vkDevice.allocateCommandBuffers(commandBufferAllocateInfo).front();
        m_currFD->usedCommandBuffers.emplace(ext::move(vkCommandBuffer), m_queueFamily, useDynamicRenderingExt);
    }
    return m_currFD->usedCommandBuffers.back();
}

void VulkanDevice::submitCommandBuffer(CommandBuffer& commandBuffer)
{
    assert(dynamic_cast<VulkanCommandBuffer*>(&commandBuffer));
    m_currFD->submittedCmdBuffers.push_back(dynamic_cast<VulkanCommandBuffer*>(&commandBuffer));
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

    m_currFD->submittedCmdBuffers.back()->vkCommandBuffer().pipelineBarrier(vk::PipelineStageFlagBits::eColorAttachmentOutput,
                                                                    vk::PipelineStageFlagBits::eTransfer,
                                                                    {}, nullptr, nullptr, imageMemoryBarrier);
    m_currFD->presentedDrawables.push_back(drawable);
}

void VulkanDevice::endFrame(void)
{
    for (size_t i = 0; auto& buffer : m_currFD->submittedCmdBuffers)
        buffer->vkCommandBuffer().end();

    ext::vector<vk::Semaphore> waitSemaphores = m_currFD->presentedDrawables
        | ext::views::transform([](auto& d){return d->imageAvailableSemaphore();})
        | ext::ranges::to<ext::vector>();

    ext::vector<vk::PipelineStageFlags> waitStages (m_currFD->presentedDrawables.size(), vk::PipelineStageFlagBits::eColorAttachmentOutput);

    ext::vector<vk::CommandBuffer> vkCommandBuffers = m_currFD->submittedCmdBuffers
        | ext::views::transform([](auto& b){return b->vkCommandBuffer();})
        | ext::ranges::to<ext::vector>();

    ext::vector<vk::Semaphore> signalSemaphores = m_currFD->presentedDrawables
        | ext::views::transform([](auto& d){return d->swapchainImage()->imagePresentableSemaphore();})
        | ext::ranges::to<ext::vector>();

    auto submitInfo = vk::SubmitInfo{}
        .setWaitSemaphores(waitSemaphores)
        .setPWaitDstStageMask(waitStages.data())
        .setCommandBuffers(vkCommandBuffers)
        .setSignalSemaphores(signalSemaphores);
    
    m_vkDevice.resetFences(m_currFD->frameCompletedFence);
    m_queue.submit(submitInfo, m_currFD->frameCompletedFence);
    
    ext::vector<vk::SwapchainKHR> swapchains = m_currFD->presentedDrawables
        | ext::views::transform([](auto& d){return d->swapchain();})
        | ext::ranges::to<ext::vector>();

    ext::vector<uint32_t> imageIndices = m_currFD->presentedDrawables
        | ext::views::transform([](auto& d){return d->imageIndex();})
        | ext::ranges::to<ext::vector>();

    auto presentInfo = vk::PresentInfoKHR{}
        .setWaitSemaphores(signalSemaphores)
        .setSwapchains(swapchains)
        .setImageIndices(imageIndices);

    if (m_queue.presentKHR(&presentInfo) != vk::Result::eSuccess)
        throw std::runtime_error("failed to present swap chain image!");

    m_currFD++;
    if (m_currFD == m_frameData.end())
        m_currFD = m_frameData.begin();
}

void VulkanDevice::waitIdle(void)
{
    m_vkDevice.waitIdle();
}

VulkanDevice::~VulkanDevice()
{
    waitIdle();

    for (auto& fd : m_frameData) {
        fd.presentedDrawables.clear();
        fd.usedCommandBuffers = ext::queue<VulkanCommandBuffer>();
        fd.commandBuffers = ext::queue<VulkanCommandBuffer>();
        m_vkDevice.destroyFence(fd.frameCompletedFence);
        m_vkDevice.destroyCommandPool(fd.commandPool);
    }
    m_vkDevice.destroy();
}

} // namespace gfx
