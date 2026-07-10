#include "gfx_tracy/gfx_tracy.hpp"

#if defined(TRACY_ENABLE)

#include "Graphics/BlitPassDescriptor.hpp"
#include "Graphics/Device.hpp"
#include "Graphics/RenderPassDescriptor.hpp"
#include "gfx_tracy_private.hpp"

#include <cassert>
#include <utility>

namespace gfx::tracy
{

GraphicsContext* createGraphicsContext(const Device& device)
{
    switch (device.backend())
    {
        case Backend::metal:
#if defined(GFX_BUILD_METAL)
            return createMetalGraphicsContext(device);
#else
            std::unreachable();
#endif
        case Backend::vulkan:
#if defined(GFX_BUILD_VULKAN)
            return createVulkanGraphicsContext(device);
#else
            std::unreachable();
#endif
    }
    std::unreachable();
}

void destroyGraphicsContext(GraphicsContext* context)
{
    assert(context);
    assert(context->destroy);
    context->destroy(context);
}

void collectGraphicsContext(GraphicsContext* context)
{
    assert(context);
    assert(context->collect);
    context->collect(context);
}

GraphicsZoneScope::GraphicsZoneScope(GraphicsContext* context, RenderPassDescriptor& descriptor, const ::tracy::SourceLocationData* sourceLocation, bool active)
{
    assert(context);
    assert(context->createRenderZone);
    assert(context->destroyZone);
    m_backendData = context->createRenderZone(*context, descriptor, sourceLocation, active);
    m_destroy = context->destroyZone;
}

GraphicsZoneScope::GraphicsZoneScope(GraphicsContext* context, BlitPassDescriptor& descriptor, const ::tracy::SourceLocationData* sourceLocation, bool active)
{
    assert(context);
    assert(context->createBlitZone);
    assert(context->destroyZone);
    m_backendData = context->createBlitZone(*context, descriptor, sourceLocation, active);
    m_destroy = context->destroyZone;
}

GraphicsZoneScope::~GraphicsZoneScope()
{
    assert(m_backendData);
    assert(m_destroy);
    m_destroy(m_backendData);
}

} // namespace gfx::tracy

#endif
