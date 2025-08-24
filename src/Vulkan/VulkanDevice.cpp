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
#include "Vulkan/vk_mem_alloc.hpp" // IWYU pragma: keep
#include "Vulkan/VulkanCommandBufferPool.hpp"

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
    #include <set>
    #include <array>
    #include <deque>
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

    auto fenceCreateInfo = vk::FenceCreateInfo{}
        .setFlags(vk::FenceCreateFlagBits::eSignaled);

    for (auto& frameData : m_frameDatas) {
        frameData.commandBufferPool = VulkanCommandBufferPool(this, m_queueFamily);
        frameData.pbPool = VulkanParameterBlockPool(this);
        frameData.semaphorePool = SemaphorePool(this);
        frameData.frameCompletedFence = m_vkDevice.createFence(fenceCreateInfo);
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

ext::unique_ptr<GraphicsPipeline> VulkanDevice::newGraphicsPipeline(const GraphicsPipeline::Descriptor& desc)
{
    for (auto& pbl : desc.parameterBlockLayouts)
        (void)descriptorSetLayout(pbl); // will create the layout if not present
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

#if defined (GFX_IMGUI_ENABLED)
void VulkanDevice::imguiInit(uint32_t imageCount,
                             ext::vector<PixelFormat> colorAttachmentPxFormats,
                             ext::optional<PixelFormat> depthAttachmentPxFormat) const
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
        .MinImageCount = imageCount,
        .ImageCount = imageCount,
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
#endif

#if defined (GFX_IMGUI_ENABLED)
void VulkanDevice::imguiNewFrame() const
{
    ImGui_ImplVulkan_NewFrame();
}
#endif

void VulkanDevice::beginFrame()
{
    if (m_vkDevice.waitForFences(m_currFrameData->frameCompletedFence, true, ext::numeric_limits<uint64_t>::max()) != vk::Result::eSuccess)
        throw ext::runtime_error("failed to wait for fence");

    m_currFrameData->presentedDrawables.clear();

    auto it = ext::ranges::find(m_currFrameData->submittedCmdBuffers, m_currFrameData->fencedCmdBuffer);
    if (it != m_currFrameData->submittedCmdBuffers.end())
        m_currFrameData->submittedCmdBuffers.erase(m_currFrameData->submittedCmdBuffers.begin(), ext::next(it));
    m_currFrameData->fencedCmdBuffer = nullptr;

    m_currFrameData->semaphorePool.reset();
    m_currFrameData->pbPool.reset();
    m_currFrameData->commandBufferPool.reset();
}

ParameterBlock& VulkanDevice::parameterBlock(const ParameterBlock::Layout& pbLayout)
{
    (void)descriptorSetLayout(pbLayout); // create if not cached, so no need to pass non const device
    return m_currFrameData->pbPool.get(pbLayout);
}

CommandBuffer& VulkanDevice::commandBuffer()
{
    return m_currFrameData->commandBufferPool.get();
}

void VulkanDevice::submitCommandBuffer(CommandBuffer& aCommandBuffer) // NOLINT(misc-no-recursion)
{
    // submittedCmdBuffers resolve the dependency between the submitted command buffers and the others
    // it insert memoryBarriers and add semaphores if necessary

    auto& cmdBuffer = dynamic_cast<VulkanCommandBuffer&>(aCommandBuffer);
    cmdBuffer.end();

    ext::vector<vk::BufferMemoryBarrier2> bufferMemoryBarriers;
    ext::vector<vk::ImageMemoryBarrier2> imageMemoryBarriers;

    for (auto& [buffer, syncReq] : cmdBuffer.bufferSyncRequests())
    {
        // TODO : check if a semaphore is required
        
        // define if a barrier is required
        auto barrier = syncBuffer(buffer->syncState(), syncReq);
        if (barrier.has_value()) {
            barrier->setBuffer(buffer->vkBuffer());
            barrier->setOffset(0);
            barrier->setSize(vk::WholeSize);
            bufferMemoryBarriers.push_back(barrier.value());
        }
        // the new sync state is the state a the end of the command buffer
        buffer->syncState() = cmdBuffer.bufferFinalSyncStates().at(buffer);
    }

    for (auto& [image, syncReq] : cmdBuffer.imageSyncRequests())
    {
        // TODO : check if a semaphore is required
        // (image->syncState().queueIdx != syncReq.queueIdx)
        
        // if the buffer use a swapchain image, add its imageAvailableSemaphore
        // to the list of wait semaphores
        if (auto swapchainImg = dynamic_pointer_cast<SwapchainImage>(image))
            cmdBuffer.waitSemaphores().insert(&swapchainImg->imageAvailableSemaphore());

        // define if a barrier is required
        auto memoryBarrier = syncImage(image->syncState(), syncReq);
        if (memoryBarrier.has_value()) {
            memoryBarrier->setImage(image->vkImage());
            memoryBarrier->setSubresourceRange(image->subresourceRange());
            imageMemoryBarriers.push_back(memoryBarrier.value());
        }
        // the new sync state is the state a the end of the command buffer
        image->syncState() = cmdBuffer.imageFinalSyncStates().at(image);
    }

    if (bufferMemoryBarriers.empty() == false || imageMemoryBarriers.empty() == false)
    {
        auto dependencyInfo = vk::DependencyInfo{}
            .setDependencyFlags(vk::DependencyFlags{});

        if (bufferMemoryBarriers.empty() == false)
            dependencyInfo.setBufferMemoryBarriers(bufferMemoryBarriers);
        if (imageMemoryBarriers.empty() == false)
            dependencyInfo.setImageMemoryBarriers(imageMemoryBarriers);

        // if a barrier is needed, we need to create a command buffer for it
        auto& barrierCmdBuffer = dynamic_cast<VulkanCommandBuffer&>(commandBuffer());
        barrierCmdBuffer.vkCommandBuffer().pipelineBarrier2(dependencyInfo);
        // transfert the command buffer wait semaphore to the barrier command buffer
        // so they are submitted in the same call
        barrierCmdBuffer.waitSemaphores().insert_range(cmdBuffer.waitSemaphores());
        cmdBuffer.waitSemaphores().clear();
        submitCommandBuffer(barrierCmdBuffer);
    }

    // if the command has wait semaphores we need to submit it in a separate call
    // so an other semaphore is require to sync with the previous command buffer.
    // this may not be required if there is not dependency between the command buffers
    // but is fine for now
    // *will happend on the barrierCmdBuffer if there is one (because the wait semaphores are transfered)*
    if (cmdBuffer.waitSemaphores().empty() == false && m_currFrameData->submittedCmdBuffers.empty() == false)
        m_currFrameData->submittedCmdBuffers.back()->signalSemaphore() = m_currFrameData->semaphorePool.get();
    // if the last command buffer has a signalSemaphore we need to add it to the wait semaphores
    // *will not happend if there is a barrierCmdBuffer (because barrierCmdBuffer dont have signal semaphores)*
    if (m_currFrameData->submittedCmdBuffers.empty() == false && m_currFrameData->submittedCmdBuffers.back()->signalSemaphore())
        cmdBuffer.waitSemaphores().insert(m_currFrameData->submittedCmdBuffers.back()->signalSemaphore());
    
    m_currFrameData->submittedCmdBuffers.push_back(&cmdBuffer);
}

void VulkanDevice::presentDrawable(const ext::shared_ptr<Drawable>& aDrawable)
{
    auto drawable = ext::dynamic_pointer_cast<VulkanDrawable>(aDrawable);
    auto swapchainImg = drawable->swapchainImage();

    ImageSyncRequest syncReq = {
        .layout = vk::ImageLayout::ePresentSrcKHR,
        .preserveContent = true};
    
    // check if a barrier is required for the swapchain image
    if (auto memoryBarrier = syncImage(swapchainImg->syncState(), syncReq))
    {
        memoryBarrier->setImage(swapchainImg->vkImage());
        memoryBarrier->setSubresourceRange(swapchainImg->subresourceRange());

        auto& barrierCmdBuffer = dynamic_cast<VulkanCommandBuffer&>(commandBuffer());
        barrierCmdBuffer.vkCommandBuffer().pipelineBarrier2(vk::DependencyInfo{}
            .setDependencyFlags(vk::DependencyFlags{}) // TODO
            .setImageMemoryBarriers(*memoryBarrier));
        submitCommandBuffer(barrierCmdBuffer);
    }

    m_currFrameData->submittedCmdBuffers.back()->signalSemaphore() = &drawable->imagePresentableSemaphore();
    m_currFrameData->presentedDrawables.push_back(drawable);
}

void VulkanDevice::endFrame()
{
    {
        // split the submittedCmdBuffers on each signal semaphores
        // all the signal/wait semaphores should be in the write place so only spliting is fine
        ext::vector<vk::SubmitInfo> submitInfos;
        ext::deque<ext::vector<vk::Semaphore>> waitSemaphores;
        ext::deque<ext::vector<vk::PipelineStageFlags>> waitStages;
        ext::deque<ext::vector<vk::CommandBuffer>> vkCommandBuffers;

        waitSemaphores.emplace_back();
        vkCommandBuffers.emplace_back();
        for (auto* commandBuffer : m_currFrameData->submittedCmdBuffers)
        {
            waitSemaphores.back().append_range(commandBuffer->waitSemaphores() | ext::views::transform([](const auto* s) -> vk::Semaphore { return *s; }));
            vkCommandBuffers.back().push_back(commandBuffer->vkCommandBuffer());

            if (commandBuffer->signalSemaphore() != nullptr)
            {
                submitInfos.push_back(vk::SubmitInfo{}
                    .setWaitSemaphores(waitSemaphores.back())
                    .setPWaitDstStageMask(waitStages.emplace_back(vkCommandBuffers.size(), vk::PipelineStageFlagBits::eAllCommands).data())
                    .setCommandBuffers(vkCommandBuffers.back())
                    .setSignalSemaphores(*commandBuffer->signalSemaphore()));
                waitSemaphores.emplace_back();
                vkCommandBuffers.emplace_back();
            }
        }
        m_currFrameData->fencedCmdBuffer = m_currFrameData->submittedCmdBuffers.back();
        m_vkDevice.resetFences(m_currFrameData->frameCompletedFence);
        m_queue.submit(submitInfos, m_currFrameData->frameCompletedFence);
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
    
    m_currFrameData->semaphorePool.swapPools();
    m_currFrameData->pbPool.swapPools();
    m_currFrameData->commandBufferPool.swapPools();
    m_currFrameData++;
    if (m_currFrameData == m_frameDatas.end())
        m_currFrameData = m_frameDatas.begin();
}

void VulkanDevice::waitIdle() const
{
    m_vkDevice.waitIdle();
}

#if defined(GFX_IMGUI_ENABLED)
void VulkanDevice::imguiShutdown() const
{
    waitIdle();
    ImGui_ImplVulkan_Shutdown();
}
#endif

const vk::DescriptorSetLayout& VulkanDevice::descriptorSetLayout(const ParameterBlock::Layout& pbLayout)
{
    auto it = m_descriptorSetLayouts.find(pbLayout);
    if (it == m_descriptorSetLayouts.end())
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
        auto [newIt, res] = m_descriptorSetLayouts.insert(ext::make_pair(pbLayout, dsLayout));
        assert(res);
        it = newIt;
    }
    return it->second;
}

VulkanDevice::~VulkanDevice()
{
    waitIdle();
    for (auto& frameData : m_frameDatas) {
        frameData.presentedDrawables.clear();
        frameData.submittedCmdBuffers.clear();
        m_vkDevice.destroyFence(frameData.frameCompletedFence);
        frameData.semaphorePool.clear();
        frameData.pbPool.clear();
        frameData.commandBufferPool.clear();
    }
    for (auto& [_, descriptorSetLayout] : m_descriptorSetLayouts)
        m_vkDevice.destroyDescriptorSetLayout(descriptorSetLayout);
    vmaDestroyAllocator(m_allocator);
    m_vkDevice.destroy();
}

} // namespace gfx
