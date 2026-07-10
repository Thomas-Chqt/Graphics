#ifndef GFX_TRACY_PRIVATE_HPP
#define GFX_TRACY_PRIVATE_HPP

#include "Graphics/Enums.hpp"
#include "gfx_tracy/gfx_tracy.hpp"

namespace gfx::tracy
{

struct TracyGfxCtx
{
    Backend backend;
    void* backendData;

    void (*destroy)(TracyGfxCtx*);
    void (*collect)(TracyGfxCtx*);

    void (*beginZone)(TracyGfxCtx&, CommandBuffer&, TracyGfxSourceLocation, bool);
    void (*endZone)(TracyGfxCtx&, CommandBuffer&);

    void (*beginRenderPass)(TracyGfxCtx&, CommandBuffer&, const Framebuffer&, TracyGfxSourceLocation, bool);
    void (*endRenderPass)(TracyGfxCtx&, CommandBuffer&);

    void (*beginBlitPass)(TracyGfxCtx&, CommandBuffer&, TracyGfxSourceLocation, bool);
    void (*endBlitPass)(TracyGfxCtx&, CommandBuffer&);
};

#if defined(GFX_BUILD_METAL)
TracyGfxCtx* createMetalTracyGfxContext(const Device&, std::string_view);
#endif

#if defined(GFX_BUILD_VULKAN)
TracyGfxCtx* createVulkanTracyGfxContext(const Device&, std::string_view);
#endif

} // namespace gfx::tracy

#endif // GFX_TRACY_PRIVATE_HPP
