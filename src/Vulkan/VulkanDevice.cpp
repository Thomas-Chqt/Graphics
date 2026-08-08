/*
 * ---------------------------------------------------
 * VulkanDevice.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/05/28 06:27:45
 * ---------------------------------------------------
 */

#include "Graphics/ParameterBlockPool.hpp"
#include "Graphics/Swapchain.hpp"
#include "Graphics/ShaderLib.hpp"
#include "Graphics/GraphicsPipeline.hpp"
#include "Graphics/CommandBuffer.hpp"
#include "Graphics/Texture.hpp"

#include "Vulkan/VulkanDevice.hpp"
#include "Vulkan/QueueFamily.hpp"
#include "Vulkan/SwapchainImage.hpp"
#include "Vulkan/Sync.hpp"
#include "Vulkan/VulkanBuffer.hpp"
#include "Vulkan/VulkanParameterBlockPool.hpp"
#include "Vulkan/VulkanPhysicalDevice.hpp"
#include "Vulkan/VulkanSwapchain.hpp"
#include "Vulkan/VulkanCommandBuffer.hpp"
#include "Vulkan/VulkanDrawable.hpp"
#include "Vulkan/VulkanShaderLib.hpp"
#include "Vulkan/VulkanGraphicsPipeline.hpp"
#include "Vulkan/VulkanComputePipeline.hpp"
#include "Vulkan/VulkanInstance.hpp"
#include "Vulkan/VulkanTexture.hpp"
#include "Vulkan/VulkanTextureView.hpp"
#include "Vulkan/VulkanPassDescriptor.hpp"
#include "VulkanParameterBlockLayout.hpp"
#include "Vulkan/VulkanEnums.hpp"
#include "Vulkan/VulkanCommandBufferPool.hpp"

namespace gfx
{

VulkanDevice::VulkanDevice(const VulkanInstance* instance, const VulkanPhysicalDevice* phyDevice, const VulkanDevice::Descriptor& desc)
    : m_instance(instance), m_physicalDevice(phyDevice)
{
    assert((m_physicalDevice->getQueueFamilies() | std::views::filter([&desc](auto f){ return f.hasCapabilities(desc.deviceDescriptor->queueCaps); })).empty() == false);

    auto synchronization2Feature = vk::PhysicalDeviceSynchronization2Features{}
        .setSynchronization2(vk::True);

    auto dynamicRenderingFeature = vk::PhysicalDeviceDynamicRenderingFeatures{}
        .setDynamicRendering(vk::True)
        .setPNext(synchronization2Feature);

    auto timelineSemaphoreFeature = vk::PhysicalDeviceTimelineSemaphoreFeatures{}
        .setTimelineSemaphore(vk::True)
        .setPNext(dynamicRenderingFeature);

    auto hostQueryResetFeature = vk::PhysicalDeviceHostQueryResetFeatures{}
        .setHostQueryReset(vk::True)
        .setPNext(&timelineSemaphoreFeature);

    auto descriptorIndexingFeatures = vk::PhysicalDeviceDescriptorIndexingFeatures{}
        .setPNext(&hostQueryResetFeature)
        .setShaderSampledImageArrayNonUniformIndexing(vk::True)
        .setDescriptorBindingSampledImageUpdateAfterBind(vk::True)
        .setDescriptorBindingUpdateUnusedWhilePending(vk::True)
        .setDescriptorBindingPartiallyBound(vk::True);

    auto supportedVulkan11Features = vk::PhysicalDeviceVulkan11Features{};
    auto supportedFeatures = vk::PhysicalDeviceFeatures2{}
        .setPNext(&supportedVulkan11Features);
    m_physicalDevice->getFeatures2(&supportedFeatures);
    assert(supportedVulkan11Features.shaderDrawParameters);

    auto vulkan11Features = vk::PhysicalDeviceVulkan11Features{}
        .setPNext(&descriptorIndexingFeatures)
        .setShaderDrawParameters(vk::True);

    m_queueFamily = (m_physicalDevice->getQueueFamilies() | std::views::filter([&desc](auto f){ return f.hasCapabilities(desc.deviceDescriptor->queueCaps); })).front();
    float queuePriority = 1.0f;
    auto queueCreateInfo = vk::DeviceQueueCreateInfo{}
        .setQueueFamilyIndex(m_queueFamily.index)
        .setQueueCount(1)
        .setPQueuePriorities(&queuePriority);

    std::vector<const char*> enabledExtensions = desc.deviceExtensions | std::views::filter([&](const char* ext) {
        if (strcmp(ext, vk::KHRDynamicRenderingExtensionName) == 0 && m_physicalDevice->getProperties().apiVersion >= vk::ApiVersion13)
            return false;
        if (strcmp(ext, vk::KHRSynchronization2ExtensionName) == 0 && m_physicalDevice->getProperties().apiVersion >= vk::ApiVersion13)
            return false;
        return true;
    }) | std::ranges::to<std::vector>();

    vk::PhysicalDeviceFeatures deviceFeatures{};

    auto deviceCreateInfo = vk::DeviceCreateInfo{}
        .setPNext(&vulkan11Features)
        .setQueueCreateInfos(queueCreateInfo)
        .setEnabledExtensionCount(static_cast<uint32_t>(enabledExtensions.size()))
        .setPpEnabledExtensionNames(enabledExtensions.data())
        .setPEnabledFeatures(&deviceFeatures);

    m_vkDevice = m_physicalDevice->createDevice(deviceCreateInfo);
    VULKAN_HPP_DEFAULT_DISPATCHER.init(m_vkDevice);

    m_queue = m_vkDevice.getQueue(m_queueFamily.index, 0);

    VmaVulkanFunctions vulkanFunctions = {};
    vulkanFunctions.vkGetInstanceProcAddr = VULKAN_HPP_DEFAULT_DISPATCHER.vkGetInstanceProcAddr;
    vulkanFunctions.vkGetDeviceProcAddr = VULKAN_HPP_DEFAULT_DISPATCHER.vkGetDeviceProcAddr;

    VmaAllocatorCreateInfo allocatorCreateInfo = {
        .flags = VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT,
        .physicalDevice = static_cast<VkPhysicalDevice>(*m_physicalDevice),
        .device = m_vkDevice,
        .pVulkanFunctions = &vulkanFunctions,
        .instance = m_instance->vkInstance(),
        .vulkanApiVersion = VK_API_VERSION_1_2
    };

    auto res = vmaCreateAllocator(&allocatorCreateInfo, &m_allocator);
    if (res != VK_SUCCESS)
        throw std::runtime_error("vmaCreateAllocator failed");

    m_timelineSemaphore = m_vkDevice.createSemaphore(vk::SemaphoreCreateInfo{}
        .setPNext(vk::SemaphoreTypeCreateInfo{}
            .setSemaphoreType(vk::SemaphoreType::eTimeline)
            .setInitialValue(0)));

    m_barrierCommandPool = m_vkDevice.createCommandPool(vk::CommandPoolCreateInfo{}
        .setQueueFamilyIndex(m_queueFamily.index)
        .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer));

}

std::unique_ptr<Swapchain> VulkanDevice::newSwapchain(const Swapchain::Descriptor& desc) const
{
    return std::make_unique<VulkanSwapchain>(this, desc);
}

std::unique_ptr<ShaderLib> VulkanDevice::newShaderLib(const std::filesystem::path& path) const
{
    return std::make_unique<VulkanShaderLib>(this, path);
}

std::unique_ptr<ParameterBlockLayout> VulkanDevice::newParameterBlockLayout(const ParameterBlockLayout::Descriptor& desc) const
{
    return std::make_unique<VulkanParameterBlockLayout>(this, desc);
}

std::unique_ptr<GraphicsPipeline> VulkanDevice::newGraphicsPipeline(const GraphicsPipeline::Descriptor& desc) const
{
    return std::make_unique<VulkanGraphicsPipeline>(this, desc);
}

std::unique_ptr<ComputePipeline> VulkanDevice::newComputePipeline(const ComputePipeline::Descriptor& desc) const
{
    return std::make_unique<VulkanComputePipeline>(this, desc);
}

std::unique_ptr<Buffer> VulkanDevice::newBuffer(const Buffer::Descriptor& desc) const
{
    return std::make_unique<VulkanBuffer>(this, desc);
}

std::unique_ptr<Texture> VulkanDevice::newTexture(const Texture::Descriptor& desc) const
{
    auto texture = std::make_shared<VulkanTexture>(this, desc);
    assert(texture);

    return std::make_unique<VulkanTextureView>(this, texture, Texture::ViewDescriptor{
        .type = desc.type,
        .baseMipLevel = 0,
        .mipLevelCount = desc.mipLevelCount,
        .baseArrayLayer = 0,
        .arrayLayerCount = desc.arrayLayerCount
    });
}

std::unique_ptr<Texture> VulkanDevice::newTextureView(const std::shared_ptr<Texture>& texture, const Texture::ViewDescriptor& desc) const
{
    auto vulkanTextureView = std::dynamic_pointer_cast<VulkanTextureView>(texture);
    assert(vulkanTextureView);
    return std::make_unique<VulkanTextureView>(this, vulkanTextureView, desc);
}

std::unique_ptr<RenderPassDescriptor> VulkanDevice::newRenderPassDescriptor() const
{
    return std::make_unique<VulkanRenderPassDescriptor>();
}

std::unique_ptr<BlitPassDescriptor> VulkanDevice::newBlitPassDescriptor() const
{
    return std::make_unique<VulkanBlitPassDescriptor>();
}

std::unique_ptr<ComputePassDescriptor> VulkanDevice::newComputePassDescriptor() const
{
    return std::make_unique<VulkanComputePassDescriptor>();
}

std::unique_ptr<CommandBufferPool> VulkanDevice::newCommandBufferPool() const
{
    return std::make_unique<VulkanCommandBufferPool>(this, m_queueFamily);
}

std::unique_ptr<ParameterBlockPool> VulkanDevice::newParameterBlockPool(const ParameterBlockPool::Descriptor& descriptor) const
{
    return std::make_unique<VulkanParameterBlockPool>(this, descriptor);
}

std::unique_ptr<Sampler> VulkanDevice::newSampler(const Sampler::Descriptor& desc) const
{
    return std::make_unique<VulkanSampler>(this, desc);
}

void VulkanDevice::submitCommandBuffers(const std::shared_ptr<CommandBuffer>& aCommandBuffer)
{
    submitCommandBuffers(std::vector<std::shared_ptr<CommandBuffer>>{aCommandBuffer});
}

void VulkanDevice::submitCommandBuffers(const std::vector<std::shared_ptr<CommandBuffer>>& aCommandBuffers)
{
    std::scoped_lock lock(m_submitMtx);

    std::vector<vk::CommandBuffer> vkCommandBuffers;

    std::vector<vk::Semaphore> waitSemaphores;
    std::vector<uint64_t> waitSemaphoreValues;
    std::vector<vk::PipelineStageFlags> waitDstStageMasks;

    std::vector<vk::Semaphore> signalSemaphores;
    std::vector<uint64_t> signalSemaphoreValues;

    std::vector<vk::Semaphore> presentWaitSemaphores;
    std::vector<vk::SwapchainKHR> presentedSwapchains;
    std::vector<uint32_t> presentedImageIndices;

    for (std::shared_ptr<VulkanCommandBuffer> commandBuffer : aCommandBuffers | std::views::transform([](auto& c) { return std::dynamic_pointer_cast<VulkanCommandBuffer>(c); }))
    {
        assert(commandBuffer);

        std::vector<vk::ImageMemoryBarrier2> imageMemoryBarriers;
        std::vector<vk::BufferMemoryBarrier2> bufferMemoryBarriers;

        for (auto& [image, syncReq] : commandBuffer->imageSyncRequests())
        {
            assert(image);
            // if the buffer use a swapchain image, add its imageAvailableSemaphore to the list of wait semaphores
            if (auto swapchainImg = dynamic_pointer_cast<SwapchainImage>(image)) {
                // no ideal to do a linear seach but we need to keep the order for the association with the value
                if (std::ranges::find(waitSemaphores, swapchainImg->imageAvailableSemaphore()) == waitSemaphores.end()) {
                    waitSemaphores.push_back(swapchainImg->imageAvailableSemaphore());
                    waitSemaphoreValues.push_back(0); // not a timeline semaphore, value doesnt matter
                    waitDstStageMasks.emplace_back(vk::PipelineStageFlagBits::eAllCommands);
                }
            }

            // define if a barrier is required
            auto memoryBarrier = syncImage(image->syncState(), syncReq);
            if (memoryBarrier.has_value()) {
                memoryBarrier->setImage(image->vkImage());
                const auto subresourceRange = vk::ImageSubresourceRange{}
                    .setAspectMask(toVkImageAspectFlags(image->pixelFormat()))
                    .setBaseMipLevel(0)
                    .setLevelCount(image->mipLevelCount())
                    .setBaseArrayLayer(0)
                    .setLayerCount(image->type() == TextureType::textureCube ? 6u : image->arrayLayerCount());
                memoryBarrier->setSubresourceRange(subresourceRange);
                imageMemoryBarriers.push_back(memoryBarrier.value());
            }

            // TODO : check if a semaphore is required
            // (image->syncState().queueIdx != syncReq.queueIdx)

            // the new sync state is the state a the end of the command buffer
            image->syncState() = commandBuffer->imageSyncStates().at(image);
        }

        for (auto& [buffer, syncReq] : commandBuffer->bufferSyncRequests())
        {
            // define if a barrier is required
            auto barrier = syncBuffer(buffer->syncState(), syncReq);
            if (barrier.has_value()) {
                barrier->setBuffer(buffer->vkBuffer());
                barrier->setOffset(0);
                barrier->setSize(vk::WholeSize);
                bufferMemoryBarriers.push_back(barrier.value());
            }

            // TODO : check if a semaphore is required

            // the new sync state is the state a the end of the command buffer
            buffer->syncState() = commandBuffer->bufferFinalSyncStates().at(buffer);
        }

        for (auto& drawable : commandBuffer->presentedDrawables())
        {
            ImageSyncRequest syncReq = {
                .layout = vk::ImageLayout::ePresentSrcKHR,
                .preserveContent = true};

            if (auto memoryBarrier = syncImage(drawable->swapchainImage()->syncState(), syncReq))
            {
                memoryBarrier->setImage(drawable->swapchainImage()->vkImage());
                memoryBarrier->setSubresourceRange(drawable->vulkanTextureView()->subresourceRange());

                // barrier need to be added at the en of the command buffer, before presenting
                commandBuffer->vkCommandBuffer().pipelineBarrier2(vk::DependencyInfo{}
                    .setDependencyFlags(vk::DependencyFlags{}) // TODO
                    .setImageMemoryBarriers(*memoryBarrier));
            }

            signalSemaphores.push_back(drawable->imagePresentableSemaphore());
            signalSemaphoreValues.push_back(0); // not a timeline semaphore, value doesnt matter
            presentWaitSemaphores.push_back(drawable->imagePresentableSemaphore());
            presentedSwapchains.push_back(drawable->swapchain());
            presentedImageIndices.push_back(drawable->imageIndex());
        }

        if (imageMemoryBarriers.empty() == false || bufferMemoryBarriers.empty() == false)
        {
            auto dependencyInfo = vk::DependencyInfo{}
                .setDependencyFlags(vk::DependencyFlags{});

            if (imageMemoryBarriers.empty() == false)
                dependencyInfo.setImageMemoryBarriers(imageMemoryBarriers);
            if (bufferMemoryBarriers.empty() == false)
                dependencyInfo.setBufferMemoryBarriers(bufferMemoryBarriers);

            std::shared_ptr<VulkanCommandBuffer> barrierCmdBuffer = getBarrierCommandBuffer();
            barrierCmdBuffer->vkCommandBuffer().pipelineBarrier2(dependencyInfo);
            barrierCmdBuffer->end();
            // barrierCmdBuffer is added before the user command buffer
            barrierCmdBuffer->setSignaledTimeValue(m_nextSignaledTimeValue);
            vkCommandBuffers.push_back(barrierCmdBuffer->vkCommandBuffer());
            m_submittedCommandBuffers.push_back(barrierCmdBuffer);
        }

        commandBuffer->end();
        // all the command buffer get submitted in the same call, so they have the same value.
        // wating on one command buffer will wait all the command buffer of the submit groupe
        commandBuffer->setSignaledTimeValue(m_nextSignaledTimeValue);
        vkCommandBuffers.push_back(commandBuffer->vkCommandBuffer());
        m_submittedCommandBuffers.push_back(commandBuffer);
    }

    // currently useless because it dont make sense to call this function with an empty vector
    // but it allow to possibly discard command buffers in the future
    if (vkCommandBuffers.empty() == false)
    {
        signalSemaphores.push_back(m_timelineSemaphore);
        signalSemaphoreValues.push_back(m_nextSignaledTimeValue);
        m_nextSignaledTimeValue++;

        auto timelineSemaphoreSubmitInfo = vk::TimelineSemaphoreSubmitInfo{}
            .setWaitSemaphoreValues(waitSemaphoreValues)
            .setSignalSemaphoreValues(signalSemaphoreValues);

        auto submitInfo = vk::SubmitInfo{}
            .setPNext(timelineSemaphoreSubmitInfo)
            .setWaitSemaphores(waitSemaphores)
            .setWaitDstStageMask(waitDstStageMasks)
            .setCommandBuffers(vkCommandBuffers)
            .setSignalSemaphores(signalSemaphores);

        m_queue.submit(submitInfo);
    }

    // for offscreen rendering or compute only
    if (presentedSwapchains.empty() == false)
    {
        auto presentInfo = vk::PresentInfoKHR{}
            .setWaitSemaphores(presentWaitSemaphores)
            .setSwapchains(presentedSwapchains)
            .setImageIndices(presentedImageIndices);

        if (m_queue.presentKHR(&presentInfo) != vk::Result::eSuccess)
            throw std::runtime_error("failed to present swap chain image!");
    }
}

void VulkanDevice::waitCommandBuffer(const CommandBuffer& aCommandBuffer)
{
    std::scoped_lock lock(m_submitMtx);

    auto waitedIt = std::ranges::find_if(m_submittedCommandBuffers, [&](auto& c){ return c.get() == &aCommandBuffer; });
    if (waitedIt != m_submittedCommandBuffers.end())
    {
        auto semaphoreWaitInfo = vk::SemaphoreWaitInfo{}
            .setSemaphores(m_timelineSemaphore)
            .setValues((*waitedIt)->signaledTimeValue());
        if (m_vkDevice.waitSemaphores(semaphoreWaitInfo, std::numeric_limits<uint64_t>::max()) != vk::Result::eSuccess)
            throw std::runtime_error("failed to wait timeline semaphore");
        for (auto it = m_submittedCommandBuffers.begin(); it != waitedIt; ++it) {
            if (m_usedBarrierCmdBuffers.contains(*it)) {
                auto node = m_usedBarrierCmdBuffers.extract(*it);
                assert(node);
                node.value()->reuse();
                m_availableBarrierCmdBuffers.push_back(std::move(node.value()));
            }
        }
        m_submittedCommandBuffers.erase(m_submittedCommandBuffers.begin(), std::next(waitedIt));
    }
}

void VulkanDevice::waitIdle()
{
    std::scoped_lock lock(m_submitMtx);

    m_vkDevice.waitIdle();
    auto it = m_submittedCommandBuffers.end();
    m_submittedCommandBuffers.erase(m_submittedCommandBuffers.begin(), it);
    for (auto& commandBuffer : m_usedBarrierCmdBuffers) {
        commandBuffer->reuse();
        m_availableBarrierCmdBuffers.push_back(commandBuffer);
    }
    m_availableBarrierCmdBuffers.clear();
}

PFN_vkGetInstanceProcAddr VulkanDevice::vkGetInstanceProcAddr() const
{
    return VULKAN_HPP_DEFAULT_DISPATCHER.vkGetInstanceProcAddr;
}

PFN_vkGetDeviceProcAddr VulkanDevice::vkGetDeviceProcAddr() const
{
    return VULKAN_HPP_DEFAULT_DISPATCHER.vkGetDeviceProcAddr;
}

VulkanDevice::~VulkanDevice()
{
    waitIdle();
    m_vkDevice.destroyCommandPool(m_barrierCommandPool);
    m_vkDevice.destroySemaphore(m_timelineSemaphore);
    vmaDestroyAllocator(m_allocator);
    m_vkDevice.destroy();
}

std::shared_ptr<VulkanCommandBuffer> VulkanDevice::getBarrierCommandBuffer()
{
    std::shared_ptr<VulkanCommandBuffer> commandBuffer;
    if (m_availableBarrierCmdBuffers.empty() == false) {
        commandBuffer = std::move(m_availableBarrierCmdBuffers.front());
        m_availableBarrierCmdBuffers.pop_front();
    }
    else {
        // command buffer is implicitly reset when begin is called.
        // https://docs.vulkan.org/refpages/latest/refpages/source/VkCommandPoolCreateFlagBits.html#_description
        commandBuffer = std::make_shared<VulkanCommandBuffer>(this, m_barrierCommandPool);
    }
    m_usedBarrierCmdBuffers.insert(commandBuffer);
    commandBuffer->begin();
    return commandBuffer;
}

} // namespace gfx
