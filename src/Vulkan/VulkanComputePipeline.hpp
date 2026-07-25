#ifndef VULKANCOMPUTEPIPELINE_HPP
#define VULKANCOMPUTEPIPELINE_HPP

#include "Graphics/ComputePipeline.hpp"

namespace gfx
{

class VulkanDevice;

class VulkanComputePipeline final : public ComputePipeline
{
public:
    VulkanComputePipeline() = delete;
    VulkanComputePipeline(const VulkanComputePipeline&) = delete;
    VulkanComputePipeline(VulkanComputePipeline&&) = delete;

    VulkanComputePipeline(const VulkanDevice*, const ComputePipeline::Descriptor&);

    const vk::Pipeline& vkPipeline() const { return m_vkPipeline; }
    const vk::PipelineLayout& pipelineLayout() const { return m_pipelineLayout; }

    ~VulkanComputePipeline() override;

private:
    const VulkanDevice* const m_device;
    vk::PipelineLayout m_pipelineLayout;
    vk::Pipeline m_vkPipeline;
};

} // namespace gfx

#endif // VULKANCOMPUTEPIPELINE_HPP
