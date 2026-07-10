#ifndef GFX_TRACY_HPP
#define GFX_TRACY_HPP

#if defined(TRACY_ENABLE)

#include <tracy/Tracy.hpp>

namespace gfx
{

class BlitPassDescriptor;
class Device;
class RenderPassDescriptor;

namespace tracy
{

struct GraphicsContext;

class GraphicsZoneScope
{
public:
    GraphicsZoneScope(GraphicsContext*, RenderPassDescriptor&, const ::tracy::SourceLocationData*, bool active);
    GraphicsZoneScope(GraphicsContext*, BlitPassDescriptor&, const ::tracy::SourceLocationData*, bool active);

    GraphicsZoneScope(const GraphicsZoneScope&) = delete;
    GraphicsZoneScope(GraphicsZoneScope&&) = delete;

    ~GraphicsZoneScope();

private:
    void* m_backendData = nullptr;
    void (*m_destroy)(void*) = nullptr;

public:
    GraphicsZoneScope& operator=(const GraphicsZoneScope&) = delete;
    GraphicsZoneScope& operator=(GraphicsZoneScope&&) = delete;
};

GraphicsContext* createGraphicsContext(const Device&);
void destroyGraphicsContext(GraphicsContext*);
void collectGraphicsContext(GraphicsContext*);

} // namespace tracy

} // namespace gfx

using TracyGraphicsCtx = gfx::tracy::GraphicsContext;

// NOLINTBEGIN(cppcoreguidelines-macro-usage)
#define TracyGraphicsContext(device) ::gfx::tracy::createGraphicsContext((device))
#define TracyGraphicsDestroy(ctx) ::gfx::tracy::destroyGraphicsContext((ctx))
#define TracyGraphicsCollect(ctx) ::gfx::tracy::collectGraphicsContext((ctx))
#define TracyGraphicsZone(ctx, passDescriptor, name) TracyGraphicsNamedZone((ctx), ___tracy_gpu_zone, (passDescriptor), (name), true)
#define TracyGraphicsNamedZone(ctx, varname, passDescriptor, name, active)                                                               \
    static constexpr ::tracy::SourceLocationData TracyConcat(__tracy_gpu_source_location, TracyLine){                                   \
        (name), TracyFunction, TracyFile, static_cast<uint32_t>(TracyLine), 0};                                                          \
    ::gfx::tracy::GraphicsZoneScope varname((ctx), (passDescriptor), &TracyConcat(__tracy_gpu_source_location, TracyLine), (active))
// NOLINTEND(cppcoreguidelines-macro-usage)

#else

using TracyGraphicsCtx = void;

// NOLINTBEGIN(cppcoreguidelines-macro-usage)
#define TracyGraphicsContext(device) nullptr
#define TracyGraphicsDestroy(ctx)
#define TracyGraphicsCollect(ctx)
#define TracyGraphicsZone(ctx, passDescriptor, name)
#define TracyGraphicsNamedZone(ctx, varname, passDescriptor, name, active)
// NOLINTEND(cppcoreguidelines-macro-usage)

#endif

#endif // GFX_TRACY_HPP
