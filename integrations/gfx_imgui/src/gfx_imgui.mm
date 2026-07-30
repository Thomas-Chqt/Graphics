/*
 * ---------------------------------------------------
 * gfx_imgui.mm
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2026/05/09
 * ---------------------------------------------------
 */

#include "gfx_imgui/gfx_imgui.hpp"

#include "Graphics/CommandBuffer.hpp"
#include "Graphics/Device.hpp"
#include "Graphics/Texture.hpp"

#include "Metal/MetalCommandBuffer.hpp"
#include "Metal/MetalDevice.hpp"
#include "Metal/MetalEnums.hpp"
#include "Metal/MetalTexture.hpp"

#if defined(GFX_BUILD_VULKAN)
    #include "Vulkan/VulkanCommandBuffer.hpp"
    #include "Vulkan/VulkanDevice.hpp"
    #include "Vulkan/VulkanEnums.hpp"
    #include "Vulkan/VulkanInstance.hpp"
    #include "Vulkan/VulkanSampler.hpp"
    #include "Vulkan/VulkanTexture.hpp"
    #include "Vulkan/VulkanTextureView.hpp"
#endif

#include "imgui_impl_metal.h"

#if defined(GFX_BUILD_VULKAN)
    #include "imgui_impl_vulkan.h"
#endif

namespace gfx::imgui
{

namespace
{

#if defined(GFX_BUILD_VULKAN)
void removeVulkanTextureId(uint64_t textureId)
{
    ImGui_ImplVulkan_RemoveTexture(std::bit_cast<VkDescriptorSet>(textureId));
}
#endif

}

void init(const Device& device, const InitInfo& info)
{
    assert(info.colorAttachmentPixelFormats.empty() == false);
    if (const auto* metalDevice = dynamic_cast<const MetalDevice*>(&device))
    {
        @autoreleasepool
        {
            ImGui_ImplMetal_Init(
                metalDevice->mtlDevice(),
                1,
                toMTLPixelFormat(info.colorAttachmentPixelFormats.front()),
                info.depthAttachmentPixelFormat.has_value() ? toMTLPixelFormat(*info.depthAttachmentPixelFormat) : MTLPixelFormatInvalid,
                MTLPixelFormatInvalid
            );
            return;
        }
    }

    #if defined(GFX_BUILD_VULKAN)
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
    #endif

    std::unreachable();
}

void newFrame(const Device& device)
{
    if (dynamic_cast<const MetalDevice*>(&device) != nullptr)
    {
        @autoreleasepool
        {
            ImGui_ImplMetal_NewFrame();
            return;
        }
    }

    #if defined(GFX_BUILD_VULKAN)
    if (dynamic_cast<const VulkanDevice*>(&device) != nullptr)
    {
        ImGui_ImplVulkan_NewFrame();
        return;
    }
    #endif

    std::unreachable();
}

void renderDrawData(CommandBuffer& commandBuffer, ImDrawData* drawData)
{
    if (auto* metalCommandBuffer = dynamic_cast<MetalCommandBuffer*>(&commandBuffer))
    {
        @autoreleasepool
        {
            assert([metalCommandBuffer->commandEncoder() conformsToProtocol:@protocol(MTLRenderCommandEncoder)]);
            auto renderCommandEncoder = (id<MTLRenderCommandEncoder>)metalCommandBuffer->commandEncoder();
            ImGui_ImplMetal_RenderDrawData(drawData, metalCommandBuffer->mtlCommandBuffer(), renderCommandEncoder);
            return;
        }
    }

    #if defined(GFX_BUILD_VULKAN)
    if (auto* vulkanCommandBuffer = dynamic_cast<VulkanCommandBuffer*>(&commandBuffer))
    {
        ImGui_ImplVulkan_RenderDrawData(drawData, vulkanCommandBuffer->vkCommandBuffer());
        return;
    }
    #endif

    std::unreachable();
}

void shutdown(Device& device)
{
    if (dynamic_cast<MetalDevice*>(&device) != nullptr)
    {
        @autoreleasepool
        {
            ImGui_ImplMetal_Shutdown();
            return;
        }
    }

    #if defined(GFX_BUILD_VULKAN)
    if (auto* vulkanDevice = dynamic_cast<VulkanDevice*>(&device))
    {
        vulkanDevice->waitIdle();
        ImGui_ImplVulkan_Shutdown();
        return;
    }
    #endif

    std::unreachable();
}

uint64_t initTextureId(Device& device, Texture& texture)
{
    if (auto* metalTexture = dynamic_cast<MetalTexture*>(&texture))
        return std::bit_cast<uint64_t>((__bridge void*)metalTexture->mtltexture());

    #if defined(GFX_BUILD_VULKAN)
    if (auto* vulkanTexture = dynamic_cast<VulkanTextureView*>(&texture))
    {
        std::shared_ptr<Sampler> sampler = device.newSampler(Sampler::Descriptor{});
        auto vulkanSampler = std::dynamic_pointer_cast<VulkanSampler>(sampler);
        assert(vulkanSampler);

        const auto textureId = std::bit_cast<uint64_t>(ImGui_ImplVulkan_AddTexture(vulkanSampler->vkSampler(), vulkanTexture->vkImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL));
        vulkanTexture->setImTextureId(textureId, vulkanSampler, removeVulkanTextureId);
        return textureId;
    }
    #endif

    std::unreachable();
}

std::optional<uint64_t> textureId(const Texture& texture)
{
    if (const auto* metalTexture = dynamic_cast<const MetalTexture*>(&texture))
        return std::bit_cast<uint64_t>((__bridge void*)metalTexture->mtltexture());

    #if defined(GFX_BUILD_VULKAN)
    if (const auto* vulkanTexture = dynamic_cast<const VulkanTextureView*>(&texture))
        return vulkanTexture->imTextureId();
    #endif

    std::unreachable();
}

void removeTextureId(Texture& texture)
{
    #if defined(GFX_BUILD_VULKAN)
    if (auto* vulkanTexture = dynamic_cast<VulkanTextureView*>(&texture))
    {
        vulkanTexture->removeImTextureId();
        return;
    }
    #endif

    std::unreachable();
}

} // namespace gfx::imgui
