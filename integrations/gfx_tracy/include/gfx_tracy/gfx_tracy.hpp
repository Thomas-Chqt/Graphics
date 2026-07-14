#pragma once

#if defined (TRACY_ENABLE)

namespace gfx
{

class Device;
class PassDescriptor;

namespace tracy
{

using TracyGfxCtx = void;

TracyGfxCtx* TracyGFXContext(const gfx::Device&);
void TracyGFXDestroy(const gfx::Device&, TracyGfxCtx*);
void TracyGFXCollect(const gfx::Device&, TracyGfxCtx*);
void TracyGFXZone(TracyGfxCtx*, gfx::PassDescriptor&, const char* name);

}

}

#ifndef GFX_TRACY_IMPL

// NOLINTBEGIN(cppcoreguidelines-macro-usage)
#define TracyGFXContext(device)          ::gfx::tracy::TracyGFXContext(device)
#define TracyGFXDestroy(device, ctx)     ::gfx::tracy::TracyGFXDestroy(device, ctx)
#define TracyGFXCollect(device, ctx) ::gfx::tracy::TracyGFXCollect(device, ctx)
#define TracyGFXZone(ctx, passDescriptor, name)
// NOLINTEND(cppcoreguidelines-macro-usage)

#endif

#else

// NOLINTBEGIN(cppcoreguidelines-macro-usage)
#define TracyGFXContext(device) nullptr
#define TracyGFXDestroy(device, ctx)
#define TracyGFXCollect(device, ctx)
#define TracyGFXZone(ctx, passDescriptor, name)
// NOLINTEND(cppcoreguidelines-macro-usage)


#endif
