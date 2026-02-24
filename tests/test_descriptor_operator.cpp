/*
 * ---------------------------------------------------
 * test_descriptor_operator.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * ---------------------------------------------------
 */

#include "Graphics/Buffer.hpp"
#include "Graphics/Device.hpp"
#include "Graphics/GraphicsPipeline.hpp"
#include "Graphics/Instance.hpp"
#include "Graphics/ParameterBlockLayout.hpp"
#include "Graphics/ParameterBlockPool.hpp"
#include "Graphics/Sampler.hpp"
#include "Graphics/Swapchain.hpp"
#include "Graphics/Texture.hpp"

#include <gtest/gtest.h>

#include <map>

namespace gfx_test
{

namespace
{
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
            .type=gfx::BindingType::uniformBuffer,
            .usages=gfx::BindingUsage::vertexRead
        }
    };

    gfx::ParameterBlockLayout::Descriptor rhs = lhs;
    rhs.bindings.push_back({ gfx::BindingType::sampler, gfx::BindingUsage::fragmentRead });

    expectDescriptorComparableInMap(lhs, rhs);
}

TEST(descriptor_operator, parameter_block_pool_descriptor)
{
    gfx::ParameterBlockPool::Descriptor lhs {};
    gfx::ParameterBlockPool::Descriptor rhs = lhs;
    rhs.maxUniformBuffers = lhs.maxUniformBuffers + 1;

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
        .engineVersion={ 1, 0, 0 }
    };
    gfx::Instance::Descriptor rhs = lhs;
    rhs.appVersion = { 1, 0, 1 };

    expectDescriptorComparableInMap(lhs, rhs);
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
