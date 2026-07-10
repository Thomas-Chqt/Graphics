#ifndef GFX_TRACY_HPP
#define GFX_TRACY_HPP

#include <cstdint>
#include <memory>
#include <string_view>

namespace gfx
{

class CommandBuffer;
class Device;
struct Framebuffer;

namespace tracy
{

struct TracyGfxSourceLocation
{
    const char* name;
    const char* function;
    const char* file;
    std::uint32_t line;
    std::uint32_t color;
};

struct TracyGfxCtx;

using TracyGfxCtxPtr = std::unique_ptr<TracyGfxCtx, void (*)(TracyGfxCtx*)>;

TracyGfxCtx* createTracyGfxContext(const Device&, std::string_view name = {});
void destroyTracyGfxContext(TracyGfxCtx*);
void collectTracyGfxContext(TracyGfxCtx*);

TracyGfxCtxPtr makeTracyGfxContext(const Device&, std::string_view name = {});

void beginTracyGfxZone(TracyGfxCtx&, CommandBuffer&, TracyGfxSourceLocation, bool active = true);
void endTracyGfxZone(TracyGfxCtx&, CommandBuffer&);

void beginTracyGfxRenderPass(TracyGfxCtx&, CommandBuffer&, const Framebuffer&, TracyGfxSourceLocation, bool active = true);
void endTracyGfxRenderPass(TracyGfxCtx&, CommandBuffer&);

void beginTracyGfxBlitPass(TracyGfxCtx&, CommandBuffer&, TracyGfxSourceLocation, bool active = true);
void endTracyGfxBlitPass(TracyGfxCtx&, CommandBuffer&);

} // namespace tracy

} // namespace gfx

// NOLINTBEGIN(cppcoreguidelines-macro-usage)
#define GFX_TRACY_SOURCE_LOCATION(name) ::gfx::tracy::TracyGfxSourceLocation{(name), __func__, __FILE__, static_cast<std::uint32_t>(__LINE__), 0}
#define GFX_TRACY_GPU_ZONE(ctx, cmd, name) ::gfx::tracy::beginTracyGfxZone((ctx), (cmd), GFX_TRACY_SOURCE_LOCATION(name))
#define GFX_TRACY_GPU_ZONE_ACTIVE(ctx, cmd, name, active) ::gfx::tracy::beginTracyGfxZone((ctx), (cmd), GFX_TRACY_SOURCE_LOCATION(name), (active))
#define GFX_TRACY_GPU_ZONE_END(ctx, cmd) ::gfx::tracy::endTracyGfxZone((ctx), (cmd))
#define GFX_TRACY_GPU_RENDER_PASS(ctx, cmd, framebuffer, name) ::gfx::tracy::beginTracyGfxRenderPass((ctx), (cmd), (framebuffer), GFX_TRACY_SOURCE_LOCATION(name))
#define GFX_TRACY_GPU_RENDER_PASS_ACTIVE(ctx, cmd, framebuffer, name, active) ::gfx::tracy::beginTracyGfxRenderPass((ctx), (cmd), (framebuffer), GFX_TRACY_SOURCE_LOCATION(name), (active))
#define GFX_TRACY_GPU_RENDER_PASS_END(ctx, cmd) ::gfx::tracy::endTracyGfxRenderPass((ctx), (cmd))
#define GFX_TRACY_GPU_BLIT_PASS(ctx, cmd, name) ::gfx::tracy::beginTracyGfxBlitPass((ctx), (cmd), GFX_TRACY_SOURCE_LOCATION(name))
#define GFX_TRACY_GPU_BLIT_PASS_ACTIVE(ctx, cmd, name, active) ::gfx::tracy::beginTracyGfxBlitPass((ctx), (cmd), GFX_TRACY_SOURCE_LOCATION(name), (active))
#define GFX_TRACY_GPU_BLIT_PASS_END(ctx, cmd) ::gfx::tracy::endTracyGfxBlitPass((ctx), (cmd))
// NOLINTEND(cppcoreguidelines-macro-usage)

#endif // GFX_TRACY_HPP
