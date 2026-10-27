#include <tracy/Tracy.hpp>

#include "gfx_tracy/gfx_tracy.hpp"

#include "Vulkan/VulkanDevice.hpp"
#include "Vulkan/VulkanInstance.hpp"

#include "gfx_tracy_vulkan.hpp"

#include <cassert>
#include <memory>
#include <utility>

namespace gfx::tracy
{

TracyGfxCtx* TracyGFXContext(const gfx::Device& device)
{
    if (const auto* vulkanDevice = dynamic_cast<const gfx::VulkanDevice*>(&device))
    {
        return (TracyGfxCtx*)::tracy::CreateVkContext(
            vulkanDevice->instance().vkInstance(),
            vulkanDevice->physicalDevice(),
            vulkanDevice->vkDevice(),
            vulkanDevice->vkGetInstanceProcAddr(),
            vulkanDevice->vkGetDeviceProcAddr());
    }
    return nullptr;
}

void TracyGFXDestroy(const gfx::Device& device, TracyGfxCtx* tracyCtx)
{
    if (tracyCtx == nullptr)
        return;

    if ([[maybe_unused]] const auto* vulkanDevice = dynamic_cast<const gfx::VulkanDevice*>(&device))
    {
        ::tracy::DestroyVkContext(reinterpret_cast<::tracy::VkCtx*>(tracyCtx)); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
        return;
    }
}

void TracyGFXCollect(const gfx::Device& device, TracyGfxCtx* tracyCtx)
{
    if (tracyCtx == nullptr)
        return;

    if ([[maybe_unused]] const auto* vulkanDevice = dynamic_cast<const gfx::VulkanDevice*>(&device))
    {
        reinterpret_cast<::tracy::VkCtx*>(tracyCtx)->Collect(VK_NULL_HANDLE); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
        return;
    }
}

void TracyGFXZoneBegin(TracyGFXZoneState& state, TracyGfxCtx* context, gfx::PassDescriptor& descriptor, const ::tracy::SourceLocationData* sourceLocation)
{
    if (context == nullptr)
        return;

    assert(state.m_active == false);
    assert(state.m_destroy == nullptr);

    if (dynamic_cast<VulkanRenderPassDescriptor*>(&descriptor)
        || dynamic_cast<VulkanBlitPassDescriptor*>(&descriptor)
        || dynamic_cast<VulkanComputePassDescriptor*>(&descriptor))
    {
        std::construct_at(reinterpret_cast<VulkanZoneImpl*>(state.m_storage.data()), context, descriptor, sourceLocation); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
        state.m_destroy = [](void* storage) noexcept {
            std::destroy_at(reinterpret_cast<VulkanZoneImpl*>(storage)); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
        };
        state.m_active = true;
        return;
    }
}

void TracyGFXZoneEnd(TracyGFXZoneState& state) noexcept
{
    if (state.m_destroy == nullptr)
        return;

    assert(state.m_active);
    assert(state.m_destroy);

    state.m_destroy(state.m_storage.data());
    state.m_destroy = nullptr;
    state.m_active = false;
}

}
