#include "gfx_tracy_private.hpp"

#if defined(TRACY_ENABLE)

#include "Graphics/BlitPassDescriptor.hpp"
#include "Graphics/Device.hpp"
#include "Graphics/RenderPassDescriptor.hpp"
#include "Metal/MetalDevice.hpp"
#include "Metal/MetalPassDescriptor.hpp"

#include <cassert>

#include <tracy/TracyMetal.hmm>

namespace gfx::tracy
{

namespace
{

struct MetalGraphicsData
{
    ::tracy::MetalCtx* context = nullptr;
};

MetalGraphicsData& metalData(GraphicsContext& context)
{
    assert(context.backend == Backend::metal);
    assert(context.backendData);
    return *static_cast<MetalGraphicsData*>(context.backendData);
}

const MetalDevice& metalDevice(const Device& device)
{
    auto* result = dynamic_cast<const MetalDevice*>(&device);
    assert(result);
    return *result;
}

void destroyMetalGraphicsContext(GraphicsContext* context)
{
    assert(context);
    auto& data = metalData(*context);
    ::tracy::MetalCtx::Destroy(data.context);
    delete &data;
    delete context;
}

void collectMetalGraphicsContext(GraphicsContext* context)
{
    assert(context);
    metalData(*context).context->Collect();
}

void* createMetalRenderZone(GraphicsContext& context, RenderPassDescriptor& descriptor, const ::tracy::SourceLocationData* sourceLocation, bool active)
{
    auto* metalDescriptor = dynamic_cast<MetalRenderPassDescriptor*>(&descriptor);
    assert(metalDescriptor);
    return new ::tracy::MetalZoneScope(metalData(context).context, metalDescriptor->mtlRenderPassDescriptor(), sourceLocation, active);
}

void* createMetalBlitZone(GraphicsContext& context, BlitPassDescriptor& descriptor, const ::tracy::SourceLocationData* sourceLocation, bool active)
{
    auto* metalDescriptor = dynamic_cast<MetalBlitPassDescriptor*>(&descriptor);
    assert(metalDescriptor);
    return new ::tracy::MetalZoneScope(metalData(context).context, metalDescriptor->mtlBlitPassDescriptor(), sourceLocation, active);
}

void destroyMetalZone(void* zone)
{
    assert(zone);
    delete static_cast<::tracy::MetalZoneScope*>(zone);
}

} // namespace

GraphicsContext* createMetalGraphicsContext(const Device& device)
{
    auto* data = new MetalGraphicsData{
        .context = ::tracy::MetalCtx::Create(metalDevice(device).mtlDevice())
    };

    return new GraphicsContext{
        .backend = Backend::metal,
        .backendData = data,
        .destroy = destroyMetalGraphicsContext,
        .collect = collectMetalGraphicsContext,
        .createRenderZone = createMetalRenderZone,
        .createBlitZone = createMetalBlitZone,
        .destroyZone = destroyMetalZone};
}

} // namespace gfx::tracy

#endif
