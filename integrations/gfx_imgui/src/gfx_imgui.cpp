/*
 * ---------------------------------------------------
 * gfx_imgui.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2026/05/09
 * ---------------------------------------------------
 */

#include "gfx_imgui/gfx_imgui.hpp"

#include "Graphics/CommandBuffer.hpp"
#include "Graphics/Device.hpp"
#include "Graphics/Texture.hpp"

#include "Vulkan/VulkanCommandBuffer.hpp"
#include "Vulkan/VulkanDevice.hpp"
#include "Vulkan/VulkanEnums.hpp"
#include "Vulkan/VulkanInstance.hpp"
#include "Vulkan/VulkanSampler.hpp"
#include "Vulkan/VulkanTexture.hpp"

#include "imgui_impl_vulkan.h"

namespace gfx::imgui
{

namespace
{
    void removeVulkanTextureId(uint64_t textureId)
    {
        ImGui_ImplVulkan_RemoveTexture(std::bit_cast<VkDescriptorSet>(textureId));
    }
}

void init(const Device& device, const InitInfo& info)
{
    if (const auto* vulkanDevice = dynamic_cast<const VulkanDevice*>(&device))
    {
        std::vector<vk::Format> colorAttachmentFormats;
        colorAttachmentFormats.reserve(info.colorAttachmentPixelFormats.size());
        for (PixelFormat pxf : info.colorAttachmentPixelFormats)
            colorAttachmentFormats.push_back(toVkFormat(pxf));

        auto pipelineRenderingCreateInfo = vk::PipelineRenderingCreateInfo()
            .setColorAttachmentFormats(colorAttachmentFormats);
        if (info.depthAttachmentPixelFormat.has_value())
            pipelineRenderingCreateInfo.setDepthAttachmentFormat(toVkFormat(info.depthAttachmentPixelFormat.value()));

        constexpr auto minAllocSize = static_cast<VkDeviceSize>(1024 * 1024);

        ImGui_ImplVulkan_LoadFunctions(
            VK_API_VERSION_1_2,
            [](const char* function_name, void* user_data) -> PFN_vkVoidFunction {
                auto* instance = static_cast<VkInstance>(user_data);
                return VULKAN_HPP_DEFAULT_DISPATCHER.vkGetInstanceProcAddr(instance, function_name);
            },
            static_cast<void*>(vulkanDevice->instance().vkInstance())
        );

        ImGui_ImplVulkan_InitInfo initInfo = {
            .ApiVersion = vulkanDevice->physicalDevice().getProperties().apiVersion,
            .Instance = vulkanDevice->instance().vkInstance(),
            .PhysicalDevice = vulkanDevice->physicalDevice(),
            .Device = vulkanDevice->vkDevice(),
            .QueueFamily = vulkanDevice->queueFamily().index,
            .Queue = vulkanDevice->queue(),
            .DescriptorPool = VK_NULL_HANDLE,
            .RenderPass = VK_NULL_HANDLE,
            .MinImageCount = info.minImageCount,
            .ImageCount = info.imageCount,
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
        return;
    }

    throw std::runtime_error("unsupported gfx::Device backend for ImGui");
}

void newFrame(const Device& device)
{
    if (dynamic_cast<const VulkanDevice*>(&device) != nullptr)
    {
        ImGui_ImplVulkan_NewFrame();
        return;
    }

    throw std::runtime_error("unsupported gfx::Device backend for ImGui");
}

void renderDrawData(CommandBuffer& commandBuffer, ImDrawData* drawData)
{
    if (auto* vulkanCommandBuffer = dynamic_cast<VulkanCommandBuffer*>(&commandBuffer))
    {
        ImGui_ImplVulkan_RenderDrawData(drawData, vulkanCommandBuffer->vkCommandBuffer());
        return;
    }

    throw std::runtime_error("unsupported gfx::CommandBuffer backend for ImGui");
}

void shutdown(Device& device)
{
    if (auto* vulkanDevice = dynamic_cast<VulkanDevice*>(&device))
    {
        vulkanDevice->waitIdle();
        ImGui_ImplVulkan_Shutdown();
        return;
    }

    throw std::runtime_error("unsupported gfx::Device backend for ImGui");
}

uint64_t initTextureId(Texture& texture)
{
    if (auto* vulkanTexture = dynamic_cast<VulkanTexture*>(&texture))
    {
        std::shared_ptr<Sampler> sampler = vulkanTexture->device().newSampler(Sampler::Descriptor{});
        auto vulkanSampler = std::dynamic_pointer_cast<VulkanSampler>(sampler);
        assert(vulkanSampler);

        const uint64_t textureId = std::bit_cast<uint64_t>(ImGui_ImplVulkan_AddTexture(vulkanSampler->vkSampler(), vulkanTexture->vkImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL));
        vulkanTexture->setImTextureId(textureId, vulkanSampler, removeVulkanTextureId);
        return textureId;
    }

    throw std::runtime_error("unsupported gfx::Texture backend for ImGui");
}

std::optional<uint64_t> textureId(const Texture& texture)
{
    if (const auto* vulkanTexture = dynamic_cast<const VulkanTexture*>(&texture))
        return vulkanTexture->imTextureId();

    return std::nullopt;
}

void removeTextureId(Texture& texture)
{
    if (auto* vulkanTexture = dynamic_cast<VulkanTexture*>(&texture))
    {
        vulkanTexture->removeImTextureId();
        return;
    }
}

} // namespace gfx::imgui
