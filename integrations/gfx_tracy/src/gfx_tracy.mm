#include "gfx_tracy/gfx_tracy.hpp"

#include "Metal/MetalDevice.hpp"

#if defined (GFX_BUILD_VULKAN)
    #include "Vulkan/VulkanDevice.hpp"
    #include "Vulkan/VulkanInstance.hpp"
#endif

#include <tracy/TracyMetal.hmm>

#if defined (GFX_BUILD_VULKAN)
    #include <tracy/TracyVulkan.hpp>
#endif

#include <utility>

namespace gfx::tracy
{

TracyGfxCtx* TracyGFXContext(const gfx::Device& device)
{
    if (const auto* metalDevice = dynamic_cast<const gfx::MetalDevice*>(&device))
    {
        return (TracyGfxCtx*)::tracy::MetalCtx::Create(metalDevice->mtlDevice());
    }

    #if defined (GFX_BUILD_VULKAN)
    if (const auto* vulkanDevice = dynamic_cast<const gfx::VulkanDevice*>(&device))
    {
        return (TracyGfxCtx*)::tracy::CreateVkContext(
            vulkanDevice->instance().vkInstance(),
            vulkanDevice->physicalDevice(),
            vulkanDevice->vkDevice(),
            vulkanDevice->vkGetInstanceProcAddr(),
            vulkanDevice->vkGetDeviceProcAddr());
    }
    #endif

    std::unreachable();
}

void TracyGFXDestroy(const gfx::Device& device, TracyGfxCtx* tracyCtx)
{
    if ([[maybe_unused]] const auto* metalDevice = dynamic_cast<const gfx::MetalDevice*>(&device))
    {
        ::tracy::MetalCtx::Destroy(reinterpret_cast<::tracy::MetalCtx*>(tracyCtx)); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
        return;
    }

    #if defined (GFX_BUILD_VULKAN)
    if ([[maybe_unused]] const auto* vulkanDevice = dynamic_cast<const gfx::VulkanDevice*>(&device))
    {
        ::tracy::DestroyVkContext(reinterpret_cast<::tracy::VkCtx*>(tracyCtx)); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
        return;
    }
    #endif

    std::unreachable();
}

void TracyGFXCollect(const gfx::Device& device, TracyGfxCtx* tracyCtx)
{
    if ([[maybe_unused]] const auto* metalDevice = dynamic_cast<const gfx::MetalDevice*>(&device))
    {
        reinterpret_cast<::tracy::MetalCtx*>(tracyCtx)->Collect(); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
        return;
    }

    #if defined (GFX_BUILD_VULKAN)
    if ([[maybe_unused]] const auto* vulkanDevice = dynamic_cast<const gfx::VulkanDevice*>(&device))
    {
        reinterpret_cast<::tracy::VkCtx*>(tracyCtx)->Collect(VK_NULL_HANDLE); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
        return;
    }
    #endif

    std::unreachable();
}

}
