/*
 * ---------------------------------------------------
 * VulkanDevice.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/05/28 06:27:45
 * ---------------------------------------------------
 */

#include "Graphics/Swapchain.hpp"
#include "Graphics/ShaderLib.hpp"
#include "Graphics/GraphicsPipeline.hpp"
#include "Graphics/CommandBuffer.hpp"
#include "Graphics/Drawable.hpp"

#include "Vulkan/VulkanDevice.hpp"
#include "Vulkan/Sync.hpp"
#include "Vulkan/VulkanBuffer.hpp"
#include "Vulkan/VulkanPhysicalDevice.hpp"
#include "Vulkan/VulkanSwapchain.hpp"
#include "Vulkan/VulkanCommandBuffer.hpp"
#include "Vulkan/VulkanDrawable.hpp"
#include "Vulkan/VulkanShaderLib.hpp"
#include "Vulkan/VulkanGraphicsPipeline.hpp"
#include "Vulkan/VulkanInstance.hpp"

#include "Vulkan/vk_mem_alloc.hpp" // IWYU pragma: keep
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
    #include <set>
    #include <algorithm>
    namespace ext = std;
#endif

namespace gfx
{

VulkanDevice::VulkanDevice(const VulkanInstance* instance, const VulkanPhysicalDevice* phyDevice, const VulkanDevice::Descriptor& desc)
    : m_instance(instance), m_physicalDevice(phyDevice)
{
    assert((m_physicalDevice->getQueueFamilies() | ext::views::filter([&desc](auto f){ return f.hasCapabilities(desc.deviceDescriptor->queueCaps); })).empty() == false);
    
    auto synchronization2Feature = vk::PhysicalDeviceSynchronization2Features{}
        .setSynchronization2(vk::True);

    auto dynamicRenderingFeature = vk::PhysicalDeviceDynamicRenderingFeatures{}
        .setDynamicRendering(vk::True)
        .setPNext(synchronization2Feature);

    m_queueFamily = (m_physicalDevice->getQueueFamilies() | ext::views::filter([&desc](auto f){ return f.hasCapabilities(desc.deviceDescriptor->queueCaps); })).front();
    float queuePriority = 1.0f;
    auto queueCreateInfo = vk::DeviceQueueCreateInfo{}
        .setQueueFamilyIndex(m_queueFamily.index)
        .setQueueCount(1)
        .setPQueuePriorities(&queuePriority);
    
    ext::vector<const char*> enabledExtensions = desc.deviceExtensions | ext::views::filter([&](const char* ext) {
        if (strcmp(ext, VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME) == 0 && m_physicalDevice->getProperties().apiVersion >= vk::ApiVersion13)
            return false;
        if (strcmp(ext, VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME) == 0 && m_physicalDevice->getProperties().apiVersion >= vk::ApiVersion13)
            return false;
        return true;
    }) | ext::ranges::to<ext::vector>();

    vk::PhysicalDeviceFeatures deviceFeatures{};

    auto deviceCreateInfo = vk::DeviceCreateInfo{}
        .setPNext(&dynamicRenderingFeature)
        .setQueueCreateInfos(queueCreateInfo)
        .setEnabledExtensionCount(static_cast<uint32_t>(enabledExtensions.size()))
        .setPpEnabledExtensionNames(enabledExtensions.data())
        .setPEnabledFeatures(&deviceFeatures);

    m_vkDevice = m_physicalDevice->createDevice(deviceCreateInfo);
    m_queue = m_vkDevice.getQueue(m_queueFamily.index, 0);

    auto commandPoolCreateInfo = vk::CommandPoolCreateInfo{}
        .setQueueFamilyIndex(m_queueFamily.index);

    m_frameDatas.resize(desc.deviceDescriptor->maxFrameInFlight);
    for (auto& frameData : m_frameDatas) {
        frameData.commandPool = m_vkDevice.createCommandPool(commandPoolCreateInfo);
        frameData.frameCompletedFence = m_vkDevice.createFence(vk::FenceCreateInfo{.flags=vk::FenceCreateFlagBits::eSignaled});
    }
    m_currFrameData = m_frameDatas.begin();

    VULKAN_HPP_DEFAULT_DISPATCHER.init(m_vkDevice);

    VmaVulkanFunctions vulkanFunctions = {};
    vulkanFunctions.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
    vulkanFunctions.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;
     
    VmaAllocatorCreateInfo allocatorCreateInfo = {
        .flags = VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT,
        .vulkanApiVersion = VK_API_VERSION_1_2,
        .physicalDevice = static_cast<VkPhysicalDevice>(*m_physicalDevice),
        .device = m_vkDevice,
        .instance = m_instance->vkInstance(),
        .pVulkanFunctions = &vulkanFunctions
    };
     
    auto res = vmaCreateAllocator(&allocatorCreateInfo, &m_allocator);
    if (res != VK_SUCCESS)
        throw ext::runtime_error("vmaCreateAllocator failed");
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

ext::unique_ptr<Buffer> VulkanDevice::newBuffer(const Buffer::Descriptor& desc) const
{
    return ext::make_unique<VulkanBuffer>(this, desc);
}

void VulkanDevice::beginFrame(void) 
{
    if (m_vkDevice.waitForFences(m_currFrameData->frameCompletedFence, true, ext::numeric_limits<uint64_t>::max()) != vk::Result::eSuccess)
        throw ext::runtime_error("failed to wait for fence");
    
    m_currFrameData->presentedDrawables.clear();
    m_currFrameData->submittedCmdBuffers.clear();
    while(m_currFrameData->usedCommandBuffers.empty() == false)
    {
        m_currFrameData->usedCommandBuffers.front().clear();
        m_currFrameData->commandBuffers.push(ext::move(m_currFrameData->usedCommandBuffers.front()));
        m_currFrameData->usedCommandBuffers.pop();
    }
    m_vkDevice.resetCommandPool(m_currFrameData->commandPool);
}

CommandBuffer& VulkanDevice::commandBuffer(void)
{
    if (m_currFrameData->commandBuffers.empty() == false) {
        m_currFrameData->usedCommandBuffers.push(ext::move(m_currFrameData->commandBuffers.front()));
        m_currFrameData->commandBuffers.pop();
    }
    else {
        auto commandBufferAllocateInfo = vk::CommandBufferAllocateInfo{}
            .setCommandPool(m_currFrameData->commandPool)
            .setLevel(vk::CommandBufferLevel::ePrimary)
            .setCommandBufferCount(1);
        vk::CommandBuffer vkCommandBuffer = m_vkDevice.allocateCommandBuffers(commandBufferAllocateInfo).front();
        m_currFrameData->usedCommandBuffers.emplace(ext::move(vkCommandBuffer), m_queueFamily);
    }
    m_currFrameData->usedCommandBuffers.back().begin();
    return m_currFrameData->usedCommandBuffers.back();
}

void VulkanDevice::submitCommandBuffer(CommandBuffer& _commandBuffer)
{
    auto& cmdBuffer = dynamic_cast<VulkanCommandBuffer&>(_commandBuffer);
    cmdBuffer.end();

#if !defined (NDEBUG)
    auto debugUtilsObjectNameInfo = vk::DebugUtilsObjectNameInfoEXT{}
        .setObjectHandle(reinterpret_cast<uint64_t>(static_cast<VkCommandBuffer>(cmdBuffer.vkCommandBuffer())))
        .setObjectType(vk::ObjectType::eCommandBuffer)
        .setPObjectName("user command buffer");
    m_vkDevice.setDebugUtilsObjectNameEXT(debugUtilsObjectNameInfo);
#endif

    ext::vector<vk::ImageMemoryBarrier2> memoryBarriers;
    for (auto& [image, syncReq] : cmdBuffer.imageSyncRequests())
    {
        auto memoryBarrier = syncImage(image->syncState(), syncReq);
        if (memoryBarrier.has_value()) {
            memoryBarrier->setImage(image->vkImage());
            memoryBarrier->setSubresourceRange(image->subresourceRange());
            memoryBarriers.push_back(memoryBarrier.value());
        }
        image->syncState() = cmdBuffer.imageFinalSyncStates().at(image);
    }
    if (memoryBarriers.empty() == false) {
        auto& barrierCmdBuffer = dynamic_cast<VulkanCommandBuffer&>(commandBuffer());
#if !defined (NDEBUG)
        auto debugUtilsObjectNameInfo = vk::DebugUtilsObjectNameInfoEXT{}
            .setObjectHandle(reinterpret_cast<uint64_t>(static_cast<VkCommandBuffer>(barrierCmdBuffer.vkCommandBuffer())))
            .setObjectType(vk::ObjectType::eCommandBuffer)
            .setPObjectName("barrier command buffer");
        m_vkDevice.setDebugUtilsObjectNameEXT(debugUtilsObjectNameInfo);
#endif
        barrierCmdBuffer.vkCommandBuffer().pipelineBarrier2(vk::DependencyInfo{}
            .setDependencyFlags(vk::DependencyFlags{}) // TODO
            .setImageMemoryBarriers(memoryBarriers));
        barrierCmdBuffer.addWaitSemaphores(cmdBuffer.waitSemaphores());
        barrierCmdBuffer.end();
        m_currFrameData->submittedCmdBuffers.push_back(&barrierCmdBuffer);
    }

    m_currFrameData->submittedCmdBuffers.push_back(&cmdBuffer);
}

void VulkanDevice::presentDrawable(const ext::shared_ptr<Drawable>& _drawable)
{
    auto drawable = ext::dynamic_pointer_cast<VulkanDrawable>(_drawable);

    ImageSyncRequest syncReq = {
        .layout = vk::ImageLayout::ePresentSrcKHR,
        .preserveContent = true};

    if (auto memoryBarrier = syncImage(drawable->vulkanTexture()->syncState(), syncReq))
    {
        memoryBarrier->setImage(drawable->vulkanTexture()->vkImage());
        memoryBarrier->setSubresourceRange(drawable->vulkanTexture()->subresourceRange());

        auto& barrierCmdBuffer = dynamic_cast<VulkanCommandBuffer&>(commandBuffer());
#if !defined (NDEBUG)
        auto debugUtilsObjectNameInfo = vk::DebugUtilsObjectNameInfoEXT{}
            .setObjectHandle(reinterpret_cast<uint64_t>(static_cast<VkCommandBuffer>(barrierCmdBuffer.vkCommandBuffer())))
            .setObjectType(vk::ObjectType::eCommandBuffer)
            .setPObjectName("barrier command buffer");
        m_vkDevice.setDebugUtilsObjectNameEXT(debugUtilsObjectNameInfo);
#endif
        barrierCmdBuffer.vkCommandBuffer().pipelineBarrier2(vk::DependencyInfo{}
            .setDependencyFlags(vk::DependencyFlags{}) // TODO
            .setImageMemoryBarriers(*memoryBarrier));
        barrierCmdBuffer.setSignalSemaphore(&drawable->imagePresentableSemaphore());
        barrierCmdBuffer.end();
        m_currFrameData->submittedCmdBuffers.push_back(&barrierCmdBuffer);
    }
    else {
        m_currFrameData->submittedCmdBuffers.back()->setSignalSemaphore(&drawable->imagePresentableSemaphore());
    }

    m_currFrameData->presentedDrawables.push_back(drawable);
}

void VulkanDevice::endFrame(void)
{
    {
        ext::vector<vk::CommandBuffer> vkCommandBuffers;
        ext::set<vk::Semaphore> waitSemaphores;
        vk::Semaphore signalSemaphore;

        auto append = [&](VulkanCommandBuffer& commandBuffer){
            vkCommandBuffers.push_back(commandBuffer.vkCommandBuffer());

            if (commandBuffer.waitSemaphores().empty() == false)
                waitSemaphores.insert_range(commandBuffer.waitSemaphores() | ext::views::transform([](const auto* s){ return *s; }));

            if (commandBuffer.signalSemaphore())
                signalSemaphore = *commandBuffer.signalSemaphore();
        };

        auto submit = [&](){
            if (vkCommandBuffers.empty())
                return;

            ext::vector<vk::Semaphore> waitSemaphoresVec = waitSemaphores | ext::ranges::to<ext::vector>();
            ext::vector<vk::PipelineStageFlags> waitStages = ext::vector<vk::PipelineStageFlags>(vkCommandBuffers.size(), vk::PipelineStageFlagBits::eAllCommands);
            auto submitInfo = vk::SubmitInfo{}
                .setWaitSemaphores(waitSemaphoresVec)
                .setPWaitDstStageMask(waitStages.data())
                .setCommandBuffers(vkCommandBuffers)
                .setSignalSemaphores(signalSemaphore);

            if (vkCommandBuffers.back() == m_currFrameData->submittedCmdBuffers.back()->vkCommandBuffer()) {
                m_vkDevice.resetFences(m_currFrameData->frameCompletedFence);
                m_queue.submit(submitInfo, m_currFrameData->frameCompletedFence);
            } else {
                m_queue.submit(submitInfo);
            }

            signalSemaphore = vk::Semaphore{};
            waitSemaphores.clear();
            vkCommandBuffers.clear();
        };

        for (auto* commandBuffer : m_currFrameData->submittedCmdBuffers)
        {
            if (commandBuffer->signalSemaphore() != nullptr)
            {
                append(*commandBuffer);
                submit();
                continue;
            }
            if (waitSemaphores.empty() == false && ext::ranges::any_of(commandBuffer->waitSemaphores(), [&](const auto* s) {return waitSemaphores.contains(*s) == false;}))
                submit();
            append(*commandBuffer);
        }
        submit();
    }
    {
        ext::vector<vk::Semaphore> waitSemaphores = m_currFrameData->presentedDrawables
            | ext::views::transform([](auto& d){return d->imagePresentableSemaphore();})
            | ext::ranges::to<ext::vector>();
        
        ext::vector<vk::SwapchainKHR> swapchains = m_currFrameData->presentedDrawables
            | ext::views::transform([](auto& d){return d->swapchain();})
            | ext::ranges::to<ext::vector>();

        ext::vector<uint32_t> imageIndices = m_currFrameData->presentedDrawables
            | ext::views::transform([](auto& d){return d->imageIndex();})
            | ext::ranges::to<ext::vector>();

        auto presentInfo = vk::PresentInfoKHR{}
            .setWaitSemaphores(waitSemaphores)
            .setSwapchains(swapchains)
            .setImageIndices(imageIndices);

        if (m_queue.presentKHR(&presentInfo) != vk::Result::eSuccess)
            throw std::runtime_error("failed to present swap chain image!");
    }

    m_currFrameData++;
    if (m_currFrameData == m_frameDatas.end())
        m_currFrameData = m_frameDatas.begin();
}

void VulkanDevice::waitIdle(void)
{
    m_vkDevice.waitIdle();
}

VulkanDevice::~VulkanDevice()
{
    waitIdle();

    for (auto& frameData : m_frameDatas) {
        frameData.presentedDrawables.clear();
        frameData.usedCommandBuffers = ext::queue<VulkanCommandBuffer>();
        frameData.commandBuffers = ext::queue<VulkanCommandBuffer>();
        m_vkDevice.destroyFence(frameData.frameCompletedFence);
        m_vkDevice.destroyCommandPool(frameData.commandPool);
    }

    vmaDestroyAllocator(m_allocator);
    m_vkDevice.destroy();
}

} // namespace gfx
