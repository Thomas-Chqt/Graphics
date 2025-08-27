/*
 * ---------------------------------------------------
 * VulkanGraphicsPipeline.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/28 09:39:54
 * ---------------------------------------------------
 */

#ifndef VULKANGRAPHICSPIPELINE_HPP
#define VULKANGRAPHICSPIPELINE_HPP

#include "Graphics/GraphicsPipeline.hpp"

namespace gfx
{

class VulkanDevice;

class VulkanGraphicsPipeline : public GraphicsPipeline
{
public:
    VulkanGraphicsPipeline() = delete;
    VulkanGraphicsPipeline(const VulkanGraphicsPipeline&) = delete;
    VulkanGraphicsPipeline(VulkanGraphicsPipeline&&) = delete;

    VulkanGraphicsPipeline(const VulkanDevice*, const GraphicsPipeline::Descriptor&);

    inline const vk::Pipeline& vkPipeline() const { return m_vkPipeline; }
    inline const vk::PipelineLayout& pipelineLayout() const { return m_pipelineLayout; }

    ~VulkanGraphicsPipeline() override;

private:
    const VulkanDevice* const m_device;
    vk::PipelineLayout m_pipelineLayout;
    vk::Pipeline m_vkPipeline;

public:
    VulkanGraphicsPipeline& operator=(const VulkanGraphicsPipeline&) = delete;
    VulkanGraphicsPipeline& operator=(VulkanGraphicsPipeline&&) = delete;
};

} // namespace gfx

#endif // VULKANGRAPHICSPIPELINE_HPP
