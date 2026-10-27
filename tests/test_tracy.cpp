#include <tracy/Tracy.hpp>
#include <gfx_tracy/gfx_tracy.hpp>

#include "Graphics/Device.hpp"

#include <gtest/gtest.h>

#if !defined(TRACY_ENABLE) || (!defined(GFX_TRACY_METAL_ENABLED) && !defined(GFX_TRACY_VULKAN_ENABLED))
#error This regression test must exercise enabled Tracy GPU profiling.
#endif

namespace
{

class MockDevice final : public gfx::Device
{
public:
    gfx::Backend backend() const override { return gfx::Backend::vulkan; }

    std::unique_ptr<gfx::Swapchain> newSwapchain(const gfx::Swapchain::Descriptor&) const override { return nullptr; }
    std::unique_ptr<gfx::ShaderLib> newShaderLib(const std::filesystem::path&) const override { return nullptr; }
    std::unique_ptr<gfx::ParameterBlockLayout> newParameterBlockLayout(const gfx::ParameterBlockLayout::Descriptor&) const override { return nullptr; }
    std::unique_ptr<gfx::GraphicsPipeline> newGraphicsPipeline(const gfx::GraphicsPipeline::Descriptor&) const override { return nullptr; }
    std::unique_ptr<gfx::ComputePipeline> newComputePipeline(const gfx::ComputePipeline::Descriptor&) const override { return nullptr; }
    std::unique_ptr<gfx::Buffer> newBuffer(const gfx::Buffer::Descriptor&) const override { return nullptr; }
    std::unique_ptr<gfx::Texture> newTexture(const gfx::Texture::Descriptor&) const override { return nullptr; }
    std::unique_ptr<gfx::Texture> newTextureView(const std::shared_ptr<gfx::Texture>&, const gfx::Texture::ViewDescriptor&) const override { return nullptr; }
    std::unique_ptr<gfx::RenderPassDescriptor> newRenderPassDescriptor() const override { return nullptr; }
    std::unique_ptr<gfx::BlitPassDescriptor> newBlitPassDescriptor() const override { return nullptr; }
    std::unique_ptr<gfx::ComputePassDescriptor> newComputePassDescriptor() const override { return nullptr; }
    std::unique_ptr<gfx::CommandBufferPool> newCommandBufferPool() const override { return nullptr; }
    std::unique_ptr<gfx::ParameterBlockPool> newParameterBlockPool(const gfx::ParameterBlockPool::Descriptor&) const override { return nullptr; }
    std::unique_ptr<gfx::Sampler> newSampler(const gfx::Sampler::Descriptor&) const override { return nullptr; }

    void submitCommandBuffers(const std::shared_ptr<gfx::CommandBuffer>&) override {}
    void submitCommandBuffers(const std::vector<std::shared_ptr<gfx::CommandBuffer>>&) override {}
    void waitCommandBuffer(const gfx::CommandBuffer&) override {}
    void waitIdle() override {}
};

class MockRenderPassDescriptor final : public gfx::RenderPassDescriptor
{
public:
    const std::vector<Attachment>& colorAttachments() const override { return m_colors; }
    void setColorAttachments(std::vector<Attachment> colors) override { m_colors = std::move(colors); }
    const std::optional<Attachment>& depthAttachment() const override { return m_depth; }
    void setDepthAttachment(std::optional<Attachment> depth) override { m_depth = std::move(depth); }

private:
    std::vector<Attachment> m_colors;
    std::optional<Attachment> m_depth;
};

class MockBlitPassDescriptor final : public gfx::BlitPassDescriptor {};
class MockComputePassDescriptor final : public gfx::ComputePassDescriptor {};

TEST(TracyMockDevice, ContextIsNull)
{
    MockDevice device;
    EXPECT_EQ(TracyGFXContext(device), nullptr);
}

TEST(TracyMockDevice, NullContextCollection)
{
    MockDevice device;
    TracyGFXCollect(device, nullptr);
}

TEST(TracyMockDevice, NullContextDestruction)
{
    MockDevice device;
    TracyGFXDestroy(device, nullptr);
}

void exerciseZones(gfx::PassDescriptor& descriptor)
{
    TracyGFXZoneBegin(zone, nullptr, descriptor, "mock explicit zone");
    TracyGFXZoneEnd(zone);
    {
        TracyGFXZone(nullptr, descriptor, "mock scoped zone");
    }
}

TEST(TracyMockDevice, NullContextZones)
{
    MockRenderPassDescriptor render;
    MockBlitPassDescriptor blit;
    MockComputePassDescriptor compute;
    exerciseZones(render);
    exerciseZones(blit);
    exerciseZones(compute);
}

TEST(TracyMockDevice, EndInactiveZone)
{
    gfx::tracy::TracyGFXZoneState state;
    TracyGFXZoneEnd(state);
}

}
