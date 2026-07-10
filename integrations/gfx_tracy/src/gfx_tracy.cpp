#include "gfx_tracy/gfx_tracy.hpp"

#include "Graphics/Device.hpp"
#include "gfx_tracy_private.hpp"

#include <cassert>
#include <utility>

namespace gfx::tracy
{

TracyGfxCtx* createTracyGfxContext(const Device& device, std::string_view name)
{
    switch (device.backend())
    {
#if defined(GFX_BUILD_METAL)
        case Backend::metal:
            return createMetalTracyGfxContext(device, name);
#endif
#if defined(GFX_BUILD_VULKAN)
        case Backend::vulkan:
            return createVulkanTracyGfxContext(device, name);
#endif
    }
    std::unreachable();
}

void destroyTracyGfxContext(TracyGfxCtx* context)
{
    if (context == nullptr)
        return;
    assert(context->destroy);
    context->destroy(context);
}

void collectTracyGfxContext(TracyGfxCtx* context)
{
    assert(context);
    assert(context->collect);
    context->collect(context);
}

TracyGfxCtxPtr makeTracyGfxContext(const Device& device, std::string_view name)
{
    return {createTracyGfxContext(device, name), destroyTracyGfxContext};
}

void beginTracyGfxZone(TracyGfxCtx& context, CommandBuffer& commandBuffer, TracyGfxSourceLocation sourceLocation, bool active)
{
    assert(context.beginZone);
    context.beginZone(context, commandBuffer, sourceLocation, active);
}

void endTracyGfxZone(TracyGfxCtx& context, CommandBuffer& commandBuffer)
{
    assert(context.endZone);
    context.endZone(context, commandBuffer);
}

void beginTracyGfxRenderPass(TracyGfxCtx& context, CommandBuffer& commandBuffer, const Framebuffer& framebuffer, TracyGfxSourceLocation sourceLocation, bool active)
{
    assert(context.beginRenderPass);
    context.beginRenderPass(context, commandBuffer, framebuffer, sourceLocation, active);
}

void endTracyGfxRenderPass(TracyGfxCtx& context, CommandBuffer& commandBuffer)
{
    assert(context.endRenderPass);
    context.endRenderPass(context, commandBuffer);
}

void beginTracyGfxBlitPass(TracyGfxCtx& context, CommandBuffer& commandBuffer, TracyGfxSourceLocation sourceLocation, bool active)
{
    assert(context.beginBlitPass);
    context.beginBlitPass(context, commandBuffer, sourceLocation, active);
}

void endTracyGfxBlitPass(TracyGfxCtx& context, CommandBuffer& commandBuffer)
{
    assert(context.endBlitPass);
    context.endBlitPass(context, commandBuffer);
}

} // namespace gfx::tracy
