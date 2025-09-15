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
#include "Vulkan/VulkanInstance.hpp"
#include "Vulkan/VulkanTexture.hpp"
#include "Vulkan/imgui_impl_vulkan.h"
#include "Vulkan/VulkanEnums.hpp"
#include "Vulkan/VulkanCommandBufferPool.hpp"
#include <algorithm>

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

    auto timelineSemaphoreFeature = vk::PhysicalDeviceTimelineSemaphoreFeatures{}
        .setTimelineSemaphore(vk::True)
        .setPNext(dynamicRenderingFeature);

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
        .setPNext(&timelineSemaphoreFeature)
        .setQueueCreateInfos(queueCreateInfo)
        .setEnabledExtensionCount(static_cast<uint32_t>(enabledExtensions.size()))
        .setPpEnabledExtensionNames(enabledExtensions.data())
        .setPEnabledFeatures(&deviceFeatures);

    m_vkDevice = m_physicalDevice->createDevice(deviceCreateInfo);
    VULKAN_HPP_DEFAULT_DISPATCHER.init(m_vkDevice);

    m_queue = m_vkDevice.getQueue(m_queueFamily.index, 0);

    VmaVulkanFunctions vulkanFunctions = {};
    vulkanFunctions.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
    vulkanFunctions.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;

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
        throw ext::runtime_error("vmaCreateAllocator failed");

    m_timelineSemaphore = m_vkDevice.createSemaphore(vk::SemaphoreCreateInfo{}
        .setPNext(vk::SemaphoreTypeCreateInfo{}
            .setSemaphoreType(vk::SemaphoreType::eTimeline)
            .setInitialValue(0)));
}

ext::unique_ptr<Swapchain> VulkanDevice::newSwapchain(const Swapchain::Descriptor& desc) const
{
    return ext::make_unique<VulkanSwapchain>(this, desc);
}

ext::unique_ptr<ShaderLib> VulkanDevice::newShaderLib(const ext::filesystem::path& path) const
{
    return ext::make_unique<VulkanShaderLib>(*this, path);
}

ext::unique_ptr<GraphicsPipeline> VulkanDevice::newGraphicsPipeline(const GraphicsPipeline::Descriptor& desc)
{
    for (auto& pbl : desc.parameterBlockLayouts)
        (void)descriptorSetLayout(pbl); // add to cache, will create the layout if not present
    return ext::make_unique<VulkanGraphicsPipeline>(this, desc);
}

ext::unique_ptr<Buffer> VulkanDevice::newBuffer(const Buffer::Descriptor& desc) const
{
    return ext::make_unique<VulkanBuffer>(this, desc);
}

ext::unique_ptr<Texture> VulkanDevice::newTexture(const Texture::Descriptor& desc) const
{
    return ext::make_unique<VulkanTexture>(this, desc);
}

ext::unique_ptr<CommandBufferPool> VulkanDevice::newCommandBufferPool() const
{
    return ext::make_unique<VulkanCommandBufferPool>(this, m_queueFamily);
}

ext::unique_ptr<ParameterBlockPool> VulkanDevice::newParameterBlockPool() const
{
    return ext::make_unique<VulkanParameterBlockPool>(this);
}

ext::unique_ptr<Sampler> VulkanDevice::newSampler(const Sampler::Descriptor& desc) const
{
    return ext::make_unique<VulkanSampler>(this, desc);
}

#if defined (GFX_IMGUI_ENABLED)
void VulkanDevice::imguiInit(ext::vector<PixelFormat> colorAttachmentPxFormats, ext::optional<PixelFormat> depthAttachmentPxFormat) const
{
    ext::vector<vk::Format> colorAttachmentFormats;
    colorAttachmentFormats.reserve(colorAttachmentPxFormats.size());
    for (PixelFormat pxf : colorAttachmentPxFormats)
    colorAttachmentFormats.push_back(toVkFormat(pxf));

    auto pipelineRenderingCreateInfo = vk::PipelineRenderingCreateInfo()
        .setColorAttachmentFormats(colorAttachmentFormats);
    if (depthAttachmentPxFormat.has_value())
        pipelineRenderingCreateInfo.setDepthAttachmentFormat(toVkFormat(depthAttachmentPxFormat.value()));

    constexpr auto minAllocSize = static_cast<VkDeviceSize>(1024*1024);

    ImGui_ImplVulkan_InitInfo initInfo = {
        .ApiVersion = m_physicalDevice->getProperties().apiVersion,
        .Instance = m_instance->vkInstance(),
        .PhysicalDevice = static_cast<const VkPhysicalDevice>(*m_physicalDevice),
        .Device = m_vkDevice,
        .QueueFamily = m_queueFamily.index,
        .Queue = m_queue,
        .DescriptorPool = VK_NULL_HANDLE,
        .RenderPass = VK_NULL_HANDLE,
        .MinImageCount = 3,
        .ImageCount = 3,
        .MSAASamples = VK_SAMPLE_COUNT_1_BIT,
        .PipelineCache = VK_NULL_HANDLE,
        .Subpass = 1,
        .DescriptorPoolSize = IMGUI_IMPL_VULKAN_MINIMUM_IMAGE_SAMPLER_POOL_SIZE,
        .UseDynamicRendering = true,
        .PipelineRenderingCreateInfo = pipelineRenderingCreateInfo,
        .Allocator = nullptr,
        .CheckVkResultFn = nullptr,
        .MinAllocationSize = minAllocSize
    };

    ImGui_ImplVulkan_Init(&initInfo);
}

void VulkanDevice::imguiNewFrame() const
{
    ImGui_ImplVulkan_NewFrame();
}

void VulkanDevice::imguiShutdown()
{
    waitIdle();
    ImGui_ImplVulkan_Shutdown();
}
#endif

void VulkanDevice::submitCommandBuffers(ext::unique_ptr<CommandBuffer>&& aCommandBuffer)
{
    ext::vector<ext::unique_ptr<CommandBuffer>> vec(1);
    vec.at(0) = ext::move(aCommandBuffer);
    submitCommandBuffers(ext::move(vec));
}

void VulkanDevice::submitCommandBuffers(ext::vector<ext::unique_ptr<CommandBuffer>> aCommandBuffers)
{
    ext::vector<vk::CommandBuffer> vkCommandBuffers;

    ext::vector<vk::Semaphore> waitSemaphores;
    ext::vector<uint64_t> waitSemaphoreValues;
    ext::vector<vk::PipelineStageFlags> waitDstStageMasks;

    ext::vector<vk::Semaphore> signalSemaphores;
    ext::vector<uint64_t> signalSemaphoreValues;

    ext::vector<vk::Semaphore> presentWaitSemaphores;
    ext::vector<vk::SwapchainKHR> presentedSwapchains;
    ext::vector<uint32_t> presentedImageIndices;

    for (ext::unique_ptr<VulkanCommandBuffer> commandBuffer : aCommandBuffers | ext::views::transform([](auto& c) { return ext::unique_ptr<VulkanCommandBuffer>(dynamic_cast<VulkanCommandBuffer*>(c.release())); }))
    {
        assert(commandBuffer);

        ext::vector<vk::ImageMemoryBarrier2> imageMemoryBarriers;
        ext::vector<vk::BufferMemoryBarrier2> bufferMemoryBarriers;

        for (auto& [image, syncReq] : commandBuffer->imageSyncRequests())
        {
            // if the buffer use a swapchain image, add its imageAvailableSemaphore
            // to the list of wait semaphores
            if (auto swapchainImg = dynamic_pointer_cast<SwapchainImage>(image)) {
                // no ideal to do a linear seach but we need to keep the order for the association with the valueq
                if (ext::ranges::find(waitSemaphores, swapchainImg->imageAvailableSemaphore()) == waitSemaphores.end()) {
                    waitSemaphores.push_back(swapchainImg->imageAvailableSemaphore());
                    waitSemaphoreValues.push_back(0); // not a timeline semaphore, value doesnt matter
                    waitDstStageMasks.emplace_back(vk::PipelineStageFlagBits::eAllCommands);
                }
            }

            // define if a barrier is required
            auto memoryBarrier = syncImage(image->syncState(), syncReq);
            if (memoryBarrier.has_value()) {
                memoryBarrier->setImage(image->vkImage());
                memoryBarrier->setSubresourceRange(image->subresourceRange());
                imageMemoryBarriers.push_back(memoryBarrier.value());
            }

            // TODO : check if a semaphore is required
            // (image->syncState().queueIdx != syncReq.queueIdx)

            // the new sync state is the state a the end of the command buffer
            image->syncState() = commandBuffer->imageFinalSyncStates().at(image);
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
                memoryBarrier->setSubresourceRange(drawable->swapchainImage()->subresourceRange());

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

            // if a barrier is needed, we need to create a command buffer for it
            // using the same pool a the submitted command buffer so they have the same lifetime
            assert(commandBuffer->poolVulkan());
            ext::unique_ptr<VulkanCommandBuffer> barrierCmdBuffer = commandBuffer->poolVulkan()->getVulkan();
            barrierCmdBuffer->vkCommandBuffer().pipelineBarrier2(dependencyInfo);
            barrierCmdBuffer->end();
            // barrierCmdBuffer is added before the user command buffer
            barrierCmdBuffer->setSignaledTimeValue(m_nextSignaledTimeValue);
            vkCommandBuffers.push_back(barrierCmdBuffer->vkCommandBuffer());
            m_submittedCommandBuffers.push_back(ext::move(barrierCmdBuffer));
        }

        commandBuffer->end();
        // all the command buffer get submitted in the same call, so they have the same value.
        // wating on one command buffer will wait all the command buffer of the submit groupe
        commandBuffer->setSignaledTimeValue(m_nextSignaledTimeValue);
        vkCommandBuffers.push_back(commandBuffer->vkCommandBuffer());
        m_submittedCommandBuffers.push_back(ext::move(commandBuffer));
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

void VulkanDevice::waitCommandBuffer(const CommandBuffer* aCommandBuffer)
{
    auto it = ext::ranges::find_if(m_submittedCommandBuffers, [&](auto& c){ return c.get() == aCommandBuffer; });
    if (it != m_submittedCommandBuffers.end())
    {
        auto semaphoreWaitInfo = vk::SemaphoreWaitInfo{}
            .setSemaphores(m_timelineSemaphore)
            .setValues((*it)->signaledTimeValue());
        if (m_vkDevice.waitSemaphores(semaphoreWaitInfo, ext::numeric_limits<uint64_t>::max()) != vk::Result::eSuccess)
            throw ext::runtime_error("failed to wait timeline semaphore");
        ++it;
        for(auto curr = m_submittedCommandBuffers.begin(); curr != it; ++curr) {
            if ((*curr)->pool())
                (*curr)->pool()->release(ext::move(*curr));
        }
        m_submittedCommandBuffers.erase(m_submittedCommandBuffers.begin(), it);
    }
}

void VulkanDevice::waitIdle()
{
    m_vkDevice.waitIdle();
    auto it = m_submittedCommandBuffers.end();
    for(auto curr = m_submittedCommandBuffers.begin(); curr != it; ++curr) {
        if ((*curr)->pool())
            (*curr)->pool()->release(ext::move(*curr));
    }
    m_submittedCommandBuffers.erase(m_submittedCommandBuffers.begin(), it);
}

const vk::DescriptorSetLayout& VulkanDevice::descriptorSetLayout(const ParameterBlock::Layout& pbLayout)
{
    auto it = m_descriptorSetLayoutCache.find(pbLayout);
    if (it == m_descriptorSetLayoutCache.end())
    {
        ext::vector<vk::DescriptorSetLayoutBinding> vkBindings;
        for (uint32_t i = 0; const auto& binding : pbLayout.bindings) {
            vkBindings.push_back(vk::DescriptorSetLayoutBinding{}
                .setBinding(i)
                .setDescriptorType(toVkDescriptorType(binding.type))
                .setDescriptorCount(1)
                .setStageFlags(toVkShaderStageFlags(binding.usages)));
            i++;
        }
        auto descriptorSetLayoutCreateInfo = vk::DescriptorSetLayoutCreateInfo{}
            .setBindings(vkBindings);
        vk::DescriptorSetLayout dsLayout = m_vkDevice.createDescriptorSetLayout(descriptorSetLayoutCreateInfo);
        auto [newIt, res] = m_descriptorSetLayoutCache.insert(ext::make_pair(pbLayout, dsLayout));
        assert(res);
        it = newIt;
    }
    return it->second;
}

VulkanDevice::~VulkanDevice()
{
    waitIdle();
    for (auto& [_, descriptorSetLayout] : m_descriptorSetLayoutCache)
        m_vkDevice.destroyDescriptorSetLayout(descriptorSetLayout);
    m_vkDevice.destroySemaphore(m_timelineSemaphore);
    vmaDestroyAllocator(m_allocator);
    m_vkDevice.destroy();
}

} // namespace gfx
