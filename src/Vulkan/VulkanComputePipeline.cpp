#include "Vulkan/VulkanComputePipeline.hpp"

#include "Vulkan/VulkanDevice.hpp"
#include "Vulkan/VulkanParameterBlockLayout.hpp"
#include "Vulkan/VulkanPhysicalDevice.hpp"
#include "Vulkan/VulkanShaderFunction.hpp"

#include <cassert>
#include <ranges>

namespace gfx
{

VulkanComputePipeline::VulkanComputePipeline(const VulkanDevice* device, const ComputePipeline::Descriptor& desc)
    : m_device(device)
{
    assert(m_device);
    assert(desc.computeShader);
    assert(desc.threadsPerThreadgroupX > 0);
    assert(desc.threadsPerThreadgroupY > 0);
    assert(desc.threadsPerThreadgroupZ > 0);

    const auto limits = m_device->physicalDevice().getProperties().limits;
    assert(desc.threadsPerThreadgroupX <= limits.maxComputeWorkGroupSize[0]);
    assert(desc.threadsPerThreadgroupY <= limits.maxComputeWorkGroupSize[1]);
    assert(desc.threadsPerThreadgroupZ <= limits.maxComputeWorkGroupSize[2]);
    const uint64_t threadCount = static_cast<uint64_t>(desc.threadsPerThreadgroupX)
        * desc.threadsPerThreadgroupY
        * desc.threadsPerThreadgroupZ;
    assert(threadCount <= limits.maxComputeWorkGroupInvocations);

    auto* computeFunction = dynamic_cast<VulkanShaderFunction*>(desc.computeShader);
    assert(computeFunction);

    std::vector<vk::DescriptorSetLayout> descriptorSetLayouts;
    descriptorSetLayouts.reserve(desc.parameterBlockLayouts.size());
    for (const auto& layout : desc.parameterBlockLayouts | std::views::transform([](const auto& value) {
        return std::dynamic_pointer_cast<VulkanParameterBlockLayout>(value);
    }))
    {
        assert(layout);
        descriptorSetLayouts.push_back(layout->vkDescriptorSetLayout());
    }

    auto pushConstantRange = vk::PushConstantRange{}
        .setStageFlags(vk::ShaderStageFlagBits::eCompute)
        .setOffset(0)
        .setSize(128);

    m_pipelineLayout = m_device->vkDevice().createPipelineLayout(vk::PipelineLayoutCreateInfo{}
        .setSetLayouts(descriptorSetLayouts)
        .setPushConstantRanges(pushConstantRange));

    auto shaderStage = vk::PipelineShaderStageCreateInfo{}
        .setStage(vk::ShaderStageFlagBits::eCompute)
        .setModule(computeFunction->shaderModule())
        .setPName(computeFunction->name().c_str());

    auto [result, pipelines] = m_device->vkDevice().createComputePipelines(
        vk::PipelineCache{},
        vk::ComputePipelineCreateInfo{}
            .setStage(shaderStage)
            .setLayout(m_pipelineLayout));
    if (result != vk::Result::eSuccess)
        throw std::runtime_error("failed to create the ComputePipeline");
    m_vkPipeline = std::move(pipelines.front());
}

VulkanComputePipeline::~VulkanComputePipeline()
{
    m_device->vkDevice().destroyPipeline(m_vkPipeline);
    m_device->vkDevice().destroyPipelineLayout(m_pipelineLayout);
}

} // namespace gfx
