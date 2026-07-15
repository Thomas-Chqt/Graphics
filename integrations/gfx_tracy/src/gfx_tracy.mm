#include <tracy/Tracy.hpp>

#include "gfx_tracy/gfx_tracy.hpp"

#if defined (GFX_TRACY_GPU_ENABLED)

#include "Graphics/Device.hpp"

#include <deque>

#if defined (GFX_TRACY_METAL_ENABLED)
#include <tracy/TracyMetal.hmm>

#include "Metal/MetalDevice.hpp"
#include "Metal/MetalPassDescriptor.hpp"
#endif

#include <cassert>
#include <memory>
#include <utility>

namespace gfx::tracy
{

namespace
{

#if defined (GFX_TRACY_METAL_ENABLED)
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
#endif

}

TracyGfxCtx* TracyGFXContext(const gfx::Device& device)
{
    #if defined (GFX_TRACY_METAL_ENABLED)
    if (const auto* metalDevice = dynamic_cast<const gfx::MetalDevice*>(&device))
    {
        return (TracyGfxCtx*)::tracy::MetalCtx::Create(metalDevice->mtlDevice());
    }
    #endif

    assert(device.backend() == Backend::vulkan);
    return nullptr;
}

void TracyGFXDestroy(const gfx::Device& device, TracyGfxCtx* tracyCtx)
{
    if (tracyCtx == nullptr)
        return;

    #if defined (GFX_TRACY_METAL_ENABLED)
    if ([[maybe_unused]] const auto* metalDevice = dynamic_cast<const gfx::MetalDevice*>(&device))
    {
        ::tracy::MetalCtx::Destroy(reinterpret_cast<::tracy::MetalCtx*>(tracyCtx)); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
        return;
    }
    #endif

    std::unreachable();
}

void TracyGFXCollect(const gfx::Device& device, TracyGfxCtx* tracyCtx)
{
    if (tracyCtx == nullptr)
        return;

    #if defined (GFX_TRACY_METAL_ENABLED)
    if ([[maybe_unused]] const auto* metalDevice = dynamic_cast<const gfx::MetalDevice*>(&device))
    {
        reinterpret_cast<::tracy::MetalCtx*>(tracyCtx)->Collect(); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
        return;
    }
    #endif

    std::unreachable();
}

void TracyGFXZoneBegin(TracyGFXZoneState& state, TracyGfxCtx* context, gfx::PassDescriptor& descriptor, const ::tracy::SourceLocationData* sourceLocation)
{
    assert(state.m_active == false);
    assert(state.m_destroy == nullptr);

    if (context == nullptr)
    {
        state.m_destroy = [](void*) noexcept {};
        state.m_active = true;
        return;
    }

    #if defined (GFX_TRACY_METAL_ENABLED)
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

#endif
