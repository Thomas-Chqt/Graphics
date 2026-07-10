#ifndef GFX_TRACY_PRIVATE_HPP
#define GFX_TRACY_PRIVATE_HPP

#include "Graphics/Enums.hpp"
#include "gfx_tracy/gfx_tracy.hpp"

#if defined(TRACY_ENABLE)

namespace gfx::tracy
{

struct GraphicsContext
{
    Backend backend;
    void* backendData;

    void (*destroy)(GraphicsContext*);
    void (*collect)(GraphicsContext*);

    void* (*createRenderZone)(GraphicsContext&, RenderPassDescriptor&, const ::tracy::SourceLocationData*, bool);
    void* (*createBlitZone)(GraphicsContext&, BlitPassDescriptor&, const ::tracy::SourceLocationData*, bool);
    void (*destroyZone)(void*);
};

#if defined(GFX_BUILD_METAL)
GraphicsContext* createMetalGraphicsContext(const Device&);
#endif

#if defined(GFX_BUILD_VULKAN)
GraphicsContext* createVulkanGraphicsContext(const Device&);
#endif

} // namespace gfx::tracy

#endif

#endif // GFX_TRACY_PRIVATE_HPP
