#include <tracy/Tracy.hpp>

#include "gfx_tracy/gfx_tracy.hpp"

#include "Metal/MetalDevice.hpp"
#include "Metal/MetalPassDescriptor.hpp"

#if defined (GFX_BUILD_VULKAN)
    #include "Vulkan/VulkanDevice.hpp"
    #include "Vulkan/VulkanInstance.hpp"
    #include "gfx_tracy_vulkan.hpp"
#endif

#include <tracy/TracyMetal.hmm>

#include <cassert>
#include <memory>
#include <utility>

namespace gfx::tracy
{

namespace
{

::tracy::MetalCtx* metalContext(TracyGfxCtx* context)
{
    assert(context);
    return reinterpret_cast<::tracy::MetalCtx*>(context); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
}

const ::tracy::SourceLocationData* checkedSourceLocation(const ::tracy::SourceLocationData* sourceLocation)
{
    assert(sourceLocation);
    return sourceLocation;
}

class MetalZoneImpl
{
public:
    MetalZoneImpl(TracyGfxCtx* context, MTLRenderPassDescriptor* descriptor, const ::tracy::SourceLocationData* sourceLocation)
        : m_scope(metalContext(context), descriptor, checkedSourceLocation(sourceLocation), true)
    {
    }

    MetalZoneImpl(TracyGfxCtx* context, MTLBlitPassDescriptor* descriptor, const ::tracy::SourceLocationData* sourceLocation)
        : m_scope(metalContext(context), descriptor, checkedSourceLocation(sourceLocation), true)
    {
    }

    MetalZoneImpl(const MetalZoneImpl&) = delete;
    MetalZoneImpl(MetalZoneImpl&&) = delete;

    ~MetalZoneImpl() = default;

    MetalZoneImpl& operator=(const MetalZoneImpl&) = delete;
    MetalZoneImpl& operator=(MetalZoneImpl&&) = delete;

private:
    ::tracy::MetalZoneScope m_scope;
};

static_assert(sizeof(MetalZoneImpl) <= TracyGFXZoneState::storageSize);
static_assert(alignof(MetalZoneImpl) <= TracyGFXZoneState::storageAlignment);

}

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

void TracyGFXZoneBegin(TracyGFXZoneState& state, TracyGfxCtx* context, gfx::PassDescriptor& descriptor, const ::tracy::SourceLocationData* sourceLocation)
{
    assert(state.m_active == false);
    assert(state.m_destroy == nullptr);

    if (auto* renderDescriptor = dynamic_cast<MetalRenderPassDescriptor*>(&descriptor))
    {
        std::construct_at(reinterpret_cast<MetalZoneImpl*>(state.m_storage.data()), context, renderDescriptor->mtlRenderPassDescriptor(), sourceLocation); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
        state.m_destroy = [](void* storage) noexcept {
            std::destroy_at(reinterpret_cast<MetalZoneImpl*>(storage)); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
        };
        state.m_active = true;
        return;
    }

    if (auto* blitDescriptor = dynamic_cast<MetalBlitPassDescriptor*>(&descriptor))
    {
        std::construct_at(reinterpret_cast<MetalZoneImpl*>(state.m_storage.data()), context, blitDescriptor->mtlBlitPassDescriptor(), sourceLocation); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
        state.m_destroy = [](void* storage) noexcept {
            std::destroy_at(reinterpret_cast<MetalZoneImpl*>(storage)); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
        };
        state.m_active = true;
        return;
    }

    #if defined (GFX_BUILD_VULKAN)
    if (dynamic_cast<VulkanRenderPassDescriptor*>(&descriptor) || dynamic_cast<VulkanBlitPassDescriptor*>(&descriptor))
    {
        std::construct_at(reinterpret_cast<VulkanZoneImpl*>(state.m_storage.data()), context, descriptor, sourceLocation); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
        state.m_destroy = [](void* storage) noexcept {
            std::destroy_at(reinterpret_cast<VulkanZoneImpl*>(storage)); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
        };
        state.m_active = true;
        return;
    }
    #endif

    std::unreachable();
}

void TracyGFXZoneEnd(TracyGFXZoneState& state) noexcept
{
    assert(state.m_active);
    assert(state.m_destroy);

    state.m_destroy(state.m_storage.data());
    state.m_destroy = nullptr;
    state.m_active = false;
}

}
