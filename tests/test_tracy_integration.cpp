#include <gtest/gtest.h>

#if defined(GFX_BUILD_TRACY_INTEGRATION)

#include "Graphics/BlitPassDescriptor.hpp"
#include "Graphics/Device.hpp"
#include "Graphics/RenderPassDescriptor.hpp"

#include <gfx_tracy/gfx_tracy.hpp>

void compileTracyGraphicsApi(
    TracyGraphicsCtx* context,
    gfx::Device& device,
    gfx::RenderPassDescriptor& renderPassDescriptor,
    gfx::BlitPassDescriptor& blitPassDescriptor)
{
    TracyGraphicsCtx* createdContext = TracyGraphicsContext(device);
    TracyGraphicsCollect(context);
    TracyGraphicsDestroy(createdContext);

    {
        TracyGraphicsNamedZone(context, renderZone, renderPassDescriptor, "render", false);
    }
    {
        TracyGraphicsNamedZone(context, blitZone, blitPassDescriptor, "blit", false);
    }
}

TEST(tracy_integration, api_compiles_and_links)
{
    EXPECT_NE(&compileTracyGraphicsApi, nullptr);
}

#endif
