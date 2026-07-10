#include "gfx_tracy_private.hpp"

#include "Graphics/Device.hpp"
#include "Graphics/Framebuffer.hpp"
#include "Metal/MetalCommandBuffer.hpp"
#include "Metal/MetalDevice.hpp"

#include <cassert>
#include <cstdint>
#include <deque>
#include <limits>
#include <memory>
#include <mutex>

#if defined(TRACY_ENABLE)
#include <tracy/TracyMetal.hmm>
#endif

namespace gfx::tracy
{

namespace
{

struct MetalTracyGfxData
{
#if defined(TRACY_ENABLE)
    ::tracy::MetalCtx* context = nullptr;
    std::mutex sourceLocationsMtx;
    std::deque<::tracy::SourceLocationData> sourceLocations;
#endif
};

MetalTracyGfxData& metalData(TracyGfxCtx& context)
{
    assert(context.backend == Backend::metal);
    assert(context.backendData);
    return *static_cast<MetalTracyGfxData*>(context.backendData);
}

const MetalDevice& metalDevice(const Device& device)
{
    auto* metalDevice = dynamic_cast<const MetalDevice*>(&device);
    assert(metalDevice);
    return *metalDevice;
}

MetalCommandBuffer& metalCommandBuffer(CommandBuffer& commandBuffer)
{
    auto* metalCommandBuffer = dynamic_cast<MetalCommandBuffer*>(&commandBuffer);
    assert(metalCommandBuffer);
    return *metalCommandBuffer;
}

#if defined(TRACY_ENABLE)
const ::tracy::SourceLocationData* storeSourceLocation(MetalTracyGfxData& data, TracyGfxSourceLocation sourceLocation)
{
    std::scoped_lock lock(data.sourceLocationsMtx);
    return &data.sourceLocations.emplace_back(::tracy::SourceLocationData{
        sourceLocation.name,
        sourceLocation.function,
        sourceLocation.file,
        sourceLocation.line,
        sourceLocation.color});
}
#endif

void destroyMetalTracyGfxContext(TracyGfxCtx* context)
{
    assert(context);
    auto& data = metalData(*context);
#if defined(TRACY_ENABLE)
    ::tracy::MetalCtx::Destroy(data.context);
#endif
    delete &data;
    delete context;
}

void collectMetalTracyGfxContext(TracyGfxCtx* context)
{
    assert(context);
#if defined(TRACY_ENABLE)
    auto& data = metalData(*context);
    data.context->Collect();
#else
    (void)context;
#endif
}

void beginMetalTracyGfxZone(TracyGfxCtx& context, CommandBuffer& commandBuffer, TracyGfxSourceLocation sourceLocation, bool active)
{
    (void)context;
    (void)commandBuffer;
    (void)sourceLocation;
    (void)active;
    assert(false && "Metal Tracy GPU zones must wrap encoder creation; use beginTracyGfxRenderPass or beginTracyGfxBlitPass");
}

void endMetalTracyGfxZone(TracyGfxCtx& context, CommandBuffer& commandBuffer)
{
    (void)context;
    (void)commandBuffer;
    assert(false && "Metal Tracy GPU zones must wrap encoder creation; use endTracyGfxRenderPass or endTracyGfxBlitPass");
}

void beginMetalTracyGfxRenderPass(TracyGfxCtx& context, CommandBuffer& commandBuffer, const Framebuffer& framebuffer, TracyGfxSourceLocation sourceLocation, bool active)
{
    auto& mtlCommandBuffer = metalCommandBuffer(commandBuffer);
#if defined(TRACY_ENABLE)
    auto& data = metalData(context);
    std::unique_ptr<::tracy::MetalZoneScope> zone;
    mtlCommandBuffer.beginRenderPass(
        framebuffer,
        [&](MTLRenderPassDescriptor* descriptor) {
            zone = std::make_unique<::tracy::MetalZoneScope>(data.context, descriptor, storeSourceLocation(data, sourceLocation), active);
        },
        [&] {
            zone.reset();
        });
#else
    (void)context;
    (void)sourceLocation;
    (void)active;
    mtlCommandBuffer.beginRenderPass(framebuffer);
#endif
}

void endMetalTracyGfxRenderPass(TracyGfxCtx& context, CommandBuffer& commandBuffer)
{
    (void)context;
    metalCommandBuffer(commandBuffer).endRenderPass();
}

void beginMetalTracyGfxBlitPass(TracyGfxCtx& context, CommandBuffer& commandBuffer, TracyGfxSourceLocation sourceLocation, bool active)
{
    auto& mtlCommandBuffer = metalCommandBuffer(commandBuffer);
#if defined(TRACY_ENABLE)
    auto& data = metalData(context);
    std::unique_ptr<::tracy::MetalZoneScope> zone;
    mtlCommandBuffer.beginBlitPass(
        [&](MTLBlitPassDescriptor* descriptor) {
            zone = std::make_unique<::tracy::MetalZoneScope>(data.context, descriptor, storeSourceLocation(data, sourceLocation), active);
        },
        [&] {
            zone.reset();
        });
#else
    (void)context;
    (void)sourceLocation;
    (void)active;
    mtlCommandBuffer.beginBlitPass();
#endif
}

void endMetalTracyGfxBlitPass(TracyGfxCtx& context, CommandBuffer& commandBuffer)
{
    (void)context;
    metalCommandBuffer(commandBuffer).endBlitPass();
}

} // namespace

TracyGfxCtx* createMetalTracyGfxContext(const Device& device, std::string_view name)
{
    const auto& mtlDevice = metalDevice(device);
    auto* data = new MetalTracyGfxData();

#if defined(TRACY_ENABLE)
    data->context = ::tracy::MetalCtx::Create(mtlDevice.mtlDevice());
    if (name.empty() == false)
    {
        assert(name.size() <= std::numeric_limits<std::uint16_t>::max());
        data->context->Name(name.data(), static_cast<std::uint16_t>(name.size()));
    }
#else
    (void)mtlDevice;
    (void)name;
#endif

    return new TracyGfxCtx{
        .backend = Backend::metal,
        .backendData = data,
        .destroy = destroyMetalTracyGfxContext,
        .collect = collectMetalTracyGfxContext,
        .beginZone = beginMetalTracyGfxZone,
        .endZone = endMetalTracyGfxZone,
        .beginRenderPass = beginMetalTracyGfxRenderPass,
        .endRenderPass = endMetalTracyGfxRenderPass,
        .beginBlitPass = beginMetalTracyGfxBlitPass,
        .endBlitPass = endMetalTracyGfxBlitPass};
}

} // namespace gfx::tracy
