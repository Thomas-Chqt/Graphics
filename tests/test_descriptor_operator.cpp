/*
 * ---------------------------------------------------
 * test_descriptor_operator.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * ---------------------------------------------------
 */

#include "Graphics/Buffer.hpp"
#include "Graphics/Device.hpp"
#include "Graphics/Enums.hpp"
#include "Graphics/GraphicsPipeline.hpp"
#include "Graphics/Instance.hpp"
#include "Graphics/ParameterBlockLayout.hpp"
#include "Graphics/ParameterBlockPool.hpp"
#include "Graphics/RenderPassDescriptor.hpp"
#include "Graphics/Sampler.hpp"
#include "Graphics/Swapchain.hpp"
#include "Graphics/Texture.hpp"

#include <gtest/gtest.h>

#include <cstdint>
#include <map>
#include <variant>

namespace gfx_test
{

namespace
{
    class TestRenderPassDescriptor final : public gfx::RenderPassDescriptor
    {
    public:
        const std::vector<Attachment>& colorAttachments() const override { return m_colorAttachments; }
        void setColorAttachments(std::vector<Attachment> attachments) override { m_colorAttachments = std::move(attachments); }

        const std::optional<Attachment>& depthAttachment() const override { return m_depthAttachment; }
        void setDepthAttachment(std::optional<Attachment> attachment) override { m_depthAttachment = std::move(attachment); }

    private:
        std::vector<Attachment> m_colorAttachments;
        std::optional<Attachment> m_depthAttachment;
    };

    template<typename T>
    void expectDescriptorComparableInMap(const T& lower, const T& higher)
    {
        EXPECT_TRUE(lower == lower);
        EXPECT_TRUE(lower < higher);
        EXPECT_TRUE(higher > lower);

        std::map<T, int> values;
        values.emplace(higher, 1);
        values.emplace(lower, 2);

        EXPECT_EQ(values.size(), 2u);
        EXPECT_EQ(values.begin()->first, lower);
        EXPECT_EQ(values.begin()->second, 2);

        values[lower] = 5;
        EXPECT_EQ(values.size(), 2u);
        EXPECT_EQ(values.at(lower), 5);
    }
}

TEST(render_pass_descriptor, stores_attachments_through_accessors)
{
    TestRenderPassDescriptor descriptor;
    descriptor.setColorAttachments({
        gfx::RenderPassDescriptor::Attachment{
            .loadAction = gfx::LoadAction::clear,
            .clearValue = gfx::ClearValue::color({1.0f, 0.5f, 0.25f, 1.0f}),
            .texture = nullptr
        }
    });
    descriptor.setDepthAttachment(gfx::RenderPassDescriptor::Attachment{
        .loadAction = gfx::LoadAction::clear,
        .clearValue = gfx::ClearValue::depth(1.0f),
        .texture = nullptr
    });

    ASSERT_EQ(descriptor.colorAttachments().size(), 1u);
    EXPECT_EQ(descriptor.colorAttachments().front().loadAction, gfx::LoadAction::clear);
    ASSERT_TRUE(descriptor.depthAttachment().has_value());
    EXPECT_EQ(descriptor.depthAttachment()->loadAction, gfx::LoadAction::clear);
}

TEST(descriptor_operator, buffer_descriptor)
{
    gfx::Buffer::Descriptor lhs {};
    gfx::Buffer::Descriptor rhs = lhs;
    rhs.size = 64;

    expectDescriptorComparableInMap(lhs, rhs);
}

TEST(descriptor_operator, texture_descriptor)
{
    gfx::Texture::Descriptor lhs {};
    gfx::Texture::Descriptor rhs = lhs;
    rhs.width = 32;

    expectDescriptorComparableInMap(lhs, rhs);
}

TEST(clear_value, default_value_is_zero_float_color)
{
    gfx::ClearValue clearValue {};

    const auto* clearColor = std::get_if<gfx::ClearFloatColor>(&clearValue.value);
    ASSERT_NE(clearColor, nullptr);

    EXPECT_EQ(clearColor->value, (std::array<float, 4>{0.0f, 0.0f, 0.0f, 0.0f}));
}

TEST(clear_value, uint64_packs_low_and_high_bits)
{
    gfx::ClearValue clearValue = gfx::ClearValue::uint64(0x0123456789abcdef);

    const auto* clearColor = std::get_if<gfx::ClearUIntColor>(&clearValue.value);
    ASSERT_NE(clearColor, nullptr);

    EXPECT_EQ(clearColor->value, (std::array<uint32_t, 4>{0x89abcdef, 0x01234567, 0, 0}));
}

TEST(descriptor_operator, sampler_descriptor)
{
    gfx::Sampler::Descriptor lhs {};
    gfx::Sampler::Descriptor rhs = lhs;
    rhs.magFilter = gfx::SamplerMinMagFilter::Linear;

    expectDescriptorComparableInMap(lhs, rhs);
}

TEST(descriptor_operator, parameter_block_layout_descriptor)
{
    gfx::ParameterBlockLayout::Descriptor lhs {};
    lhs.bindings = {
        {
            .type=gfx::BindingType::constantBuffer,
            .usages=gfx::BindingUsage::vertexRead
        }
    };

    gfx::ParameterBlockLayout::Descriptor rhs = lhs;
    rhs.bindings.push_back({ gfx::BindingType::sampler, gfx::BindingUsage::fragmentRead });

    expectDescriptorComparableInMap(lhs, rhs);
}

TEST(descriptor_operator, parameter_block_binding)
{
    gfx::ParameterBlockBinding lhs {
        .type = gfx::BindingType::sampledTexture,
        .usages = gfx::BindingUsage::fragmentRead,
        .count = 1
    };
    gfx::ParameterBlockBinding rhs = lhs;
    rhs.count = 8;

    expectDescriptorComparableInMap(lhs, rhs);
}

TEST(descriptor_operator, parameter_block_pool_descriptor)
{
    gfx::ParameterBlockPool::Descriptor lhs {};
    gfx::ParameterBlockPool::Descriptor rhs = lhs;
    rhs.updateAfterBind = true;

    expectDescriptorComparableInMap(lhs, rhs);
}

TEST(descriptor_operator, swapchain_descriptor)
{
    gfx::Swapchain::Descriptor lhs {};
    gfx::Swapchain::Descriptor rhs = lhs;
    rhs.width = 1280;

    expectDescriptorComparableInMap(lhs, rhs);
}

TEST(descriptor_operator, device_descriptor)
{
    gfx::Device::Descriptor lhs {};
    lhs.queueCaps = {
        .graphics=true,
        .compute=false,
        .transfer=false,
        .present={}
    };

    gfx::Device::Descriptor rhs = lhs;
    rhs.queueCaps.compute = true;

    expectDescriptorComparableInMap(lhs, rhs);
}

TEST(descriptor_operator, instance_descriptor)
{
    gfx::Instance::Descriptor lhs {
        .appName="App",
        .appVersion={ 1, 0, 0 },
        .engineName="Engine",
        .engineVersion={ 1, 0, 0 },
        .instanceExtension=reinterpret_cast<const gfx::InstanceExtension*>(static_cast<std::uintptr_t>(0x1))
    };
    gfx::Instance::Descriptor rhs = lhs;
    rhs.instanceExtension = reinterpret_cast<const gfx::InstanceExtension*>(static_cast<std::uintptr_t>(0x2));
    rhs.appVersion = { 1, 0, 1 };

    expectDescriptorComparableInMap(lhs, rhs);

    rhs = lhs;
    rhs.instanceExtension = reinterpret_cast<const gfx::InstanceExtension*>(static_cast<std::uintptr_t>(0x2));

    EXPECT_NE(lhs, rhs);
}

TEST(descriptor_operator, graphics_pipeline_descriptor)
{
    gfx::GraphicsPipeline::Descriptor lhs {};
    lhs.vertexShader = nullptr;
    lhs.fragmentShader = nullptr;
    lhs.cullMode = gfx::CullMode::none;

    gfx::GraphicsPipeline::Descriptor rhs = lhs;
    rhs.cullMode = gfx::CullMode::back;

    expectDescriptorComparableInMap(lhs, rhs);
}

}
