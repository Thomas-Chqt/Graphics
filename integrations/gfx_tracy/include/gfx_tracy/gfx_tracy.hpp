#pragma once

namespace gfx::tracy
{
    using TracyGfxCtx = void;
}

#if defined (GFX_TRACY_METAL_AVAILABLE) && (defined (__arm64__) || defined (__aarch64__))
    #define GFX_TRACY_METAL_ENABLED
#endif

#if defined (GFX_TRACY_METAL_ENABLED) || defined (GFX_TRACY_VULKAN_ENABLED)
    #define GFX_TRACY_GPU_ENABLED
#endif

#if defined (TRACY_ENABLE) && defined (GFX_TRACY_GPU_ENABLED)

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>

namespace gfx
{

class Device;
class PassDescriptor;

namespace tracy
{

class TracyGFXZoneState
{
public:
    static constexpr std::size_t storageSize = 64;
    static constexpr std::size_t storageAlignment = alignof(std::max_align_t);

    TracyGFXZoneState() = default;
    TracyGFXZoneState(const TracyGFXZoneState&) = delete;
    TracyGFXZoneState(TracyGFXZoneState&&) = delete;

    ~TracyGFXZoneState() { assert(m_active == false); }

    TracyGFXZoneState& operator=(const TracyGFXZoneState&) = delete;
    TracyGFXZoneState& operator=(TracyGFXZoneState&&) = delete;

private:
    using DestroyFunction = void (*)(void*) noexcept;

    alignas(storageAlignment) std::array<std::byte, storageSize> m_storage{};
    DestroyFunction m_destroy = nullptr;
    bool m_active = false;

    friend void TracyGFXZoneBegin(TracyGFXZoneState&, TracyGfxCtx*, gfx::PassDescriptor&, const ::tracy::SourceLocationData*);
    friend void TracyGFXZoneEnd(TracyGFXZoneState&) noexcept;
};

TracyGfxCtx* TracyGFXContext(const gfx::Device&);
void TracyGFXDestroy(const gfx::Device&, TracyGfxCtx*);
void TracyGFXCollect(const gfx::Device&, TracyGfxCtx*);
void TracyGFXZoneBegin(TracyGFXZoneState&, TracyGfxCtx*, gfx::PassDescriptor&, const ::tracy::SourceLocationData*);
void TracyGFXZoneEnd(TracyGFXZoneState&) noexcept;

class TracyGFXZoneScope
{
public:
    TracyGFXZoneScope(TracyGfxCtx* context, gfx::PassDescriptor& descriptor, const ::tracy::SourceLocationData* sourceLocation)
    {
        TracyGFXZoneBegin(m_state, context, descriptor, sourceLocation);
    }

    TracyGFXZoneScope(const TracyGFXZoneScope&) = delete;
    TracyGFXZoneScope(TracyGFXZoneScope&&) = delete;

    ~TracyGFXZoneScope() { TracyGFXZoneEnd(m_state); }

    TracyGFXZoneScope& operator=(const TracyGFXZoneScope&) = delete;
    TracyGFXZoneScope& operator=(TracyGFXZoneScope&&) = delete;

private:
    TracyGFXZoneState m_state;
};

} // namespace tracy
} // namespace gfx

#ifndef GFX_TRACY_IMPL

// NOLINTBEGIN(cppcoreguidelines-macro-usage)
#define TracyGFXContext(device)      ::gfx::tracy::TracyGFXContext(device)
#define TracyGFXDestroy(device, ctx) ::gfx::tracy::TracyGFXDestroy(device, ctx)
#define TracyGFXCollect(device, ctx) ::gfx::tracy::TracyGFXCollect(device, ctx)
#define TracyGFXZoneBegin(varname, ctx, passDescriptor, name) \
    static constexpr ::tracy::SourceLocationData TracyConcat(__tracy_gfx_source_location, TracyLine) { name, TracyFunction, TracyFile, static_cast<std::uint32_t>(TracyLine), 0 }; \
    ::gfx::tracy::TracyGFXZoneState varname; \
    ::gfx::tracy::TracyGFXZoneBegin(varname, ctx, passDescriptor, &TracyConcat(__tracy_gfx_source_location, TracyLine))
#define TracyGFXZoneEnd(varname) ::gfx::tracy::TracyGFXZoneEnd(varname)
#define TracyGFXZone(ctx, passDescriptor, name) \
    static constexpr ::tracy::SourceLocationData TracyConcat(__tracy_gfx_source_location, TracyLine) { name, TracyFunction, TracyFile, static_cast<std::uint32_t>(TracyLine), 0 }; \
    ::gfx::tracy::TracyGFXZoneScope TracyConcat(__tracy_gfx_zone, TracyLine) { ctx, passDescriptor, &TracyConcat(__tracy_gfx_source_location, TracyLine) }
// NOLINTEND(cppcoreguidelines-macro-usage)

#endif

#else

// NOLINTBEGIN(cppcoreguidelines-macro-usage)
#define TracyGFXContext(device) nullptr
#define TracyGFXDestroy(device, ctx)
#define TracyGFXCollect(device, ctx)
#define TracyGFXZoneBegin(varname, ctx, passDescriptor, name)
#define TracyGFXZoneEnd(varname)
#define TracyGFXZone(ctx, passDescriptor, name)
// NOLINTEND(cppcoreguidelines-macro-usage)


#endif
