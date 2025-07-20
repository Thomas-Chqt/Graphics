/*
 * ---------------------------------------------------
 * VulkanGraphicsPipeline.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/28 09:48:05
 * ---------------------------------------------------
 */

#include "Graphics/GraphicsPipeline.hpp"

#include "Vulkan/VulkanGraphicsPipeline.hpp"
#include "Vulkan/VulkanDevice.hpp"
#include "Vulkan/VulkanShaderFunction.hpp"
#include "Vulkan/VulkanEnums.hpp"

#include <vulkan/vulkan.hpp>

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <stdexcept>
    #include <utility>
    #include <array>
    namespace ext = std;
#endif

namespace gfx
{

VulkanGraphicsPipeline::VulkanGraphicsPipeline(const VulkanDevice* device, const GraphicsPipeline::Descriptor& desc)
    : m_device(device)
{
    VulkanShaderFunction* vertFunc = dynamic_cast<VulkanShaderFunction*>(desc.vertexShader);
    VulkanShaderFunction* fragFunc = dynamic_cast<VulkanShaderFunction*>(desc.fragmentShader);

    auto vertShaderStageCreateInfo = vk::PipelineShaderStageCreateInfo{}
        .setStage(vk::ShaderStageFlagBits::eVertex)
        .setModule(vertFunc->shaderModule())
        .setPName(vertFunc->name().c_str());

    auto fragShaderStageCreateInfo = vk::PipelineShaderStageCreateInfo{}
        .setStage(vk::ShaderStageFlagBits::eFragment)
        .setModule(fragFunc->shaderModule())
        .setPName(fragFunc->name().c_str());

    ext::array<vk::PipelineShaderStageCreateInfo, 2> shaderStages = { vertShaderStageCreateInfo, fragShaderStageCreateInfo };

    ext::array<vk::DynamicState, 2> dynamicStates = {
        vk::DynamicState::eViewport,
        vk::DynamicState::eScissor
    };

    auto dynamicStateCreateInfo = vk::PipelineDynamicStateCreateInfo()
        .setDynamicStates(dynamicStates);

    auto vertexInputStateCreateInfo = vk::PipelineVertexInputStateCreateInfo()
        .setVertexBindingDescriptions(nullptr)
        .setVertexAttributeDescriptions(nullptr);

    auto inputAssemblyStateCreateInfo = vk::PipelineInputAssemblyStateCreateInfo()
        .setTopology(vk::PrimitiveTopology::eTriangleList)
        .setPrimitiveRestartEnable(false);

    auto viewportStateCreateInfo = vk::PipelineViewportStateCreateInfo()
        .setViewportCount(1)
        .setScissorCount(1);

    auto rasterizationStateCreateInfo = vk::PipelineRasterizationStateCreateInfo()
        .setDepthClampEnable(false)
        .setRasterizerDiscardEnable(false)
        .setPolygonMode(vk::PolygonMode::eFill)
        .setLineWidth(1.0f)
        .setCullMode(vk::CullModeFlagBits::eNone)
        .setFrontFace(vk::FrontFace::eClockwise)
        .setDepthBiasEnable(false);

    auto multisampleStateCreateInfo = vk::PipelineMultisampleStateCreateInfo()
        .setSampleShadingEnable(false)
        .setRasterizationSamples(vk::SampleCountFlagBits::e1);

    auto colorBlendAttachmentState = vk::PipelineColorBlendAttachmentState()
        .setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);

    if (desc.blendOperation == BlendOperation::blendingOff)
    {
        colorBlendAttachmentState.setBlendEnable(false);
    }
    else if (desc.blendOperation == BlendOperation::srcA_plus_1_minus_srcA)
    {
        colorBlendAttachmentState
            .setBlendEnable(true)
            .setSrcColorBlendFactor(vk::BlendFactor::eSrcAlpha)
            .setDstColorBlendFactor(vk::BlendFactor::eOneMinusSrcAlpha)
            .setColorBlendOp(vk::BlendOp::eAdd)
            .setSrcAlphaBlendFactor(vk::BlendFactor::eOne)
            .setDstAlphaBlendFactor(vk::BlendFactor::eZero)
            .setAlphaBlendOp(vk::BlendOp::eAdd);
    }
    else if (desc.blendOperation == BlendOperation::one_minus_srcA_plus_srcA)
    {
        colorBlendAttachmentState
            .setBlendEnable(true)
            .setSrcColorBlendFactor(vk::BlendFactor::eOneMinusSrcAlpha)
            .setDstColorBlendFactor(vk::BlendFactor::eSrcAlpha)
            .setColorBlendOp(vk::BlendOp::eAdd)
            .setSrcAlphaBlendFactor(vk::BlendFactor::eOne)
            .setDstAlphaBlendFactor(vk::BlendFactor::eZero)
            .setAlphaBlendOp(vk::BlendOp::eAdd);
    }

    ext::vector<vk::PipelineColorBlendAttachmentState> colorBlendAttachmentStates(desc.colorAttachmentPxFormats.size(), colorBlendAttachmentState);

    auto colorBlendStateCreateInfo = vk::PipelineColorBlendStateCreateInfo()
        .setLogicOpEnable(false)
        .setAttachments(colorBlendAttachmentStates);

    auto pipelineLayoutCreateInfo = vk::PipelineLayoutCreateInfo();

    m_pipelineLayout = m_device->vkDevice().createPipelineLayout(pipelineLayoutCreateInfo);

    ext::vector<vk::Format> colorAttachmentFormats;
    colorAttachmentFormats.reserve(desc.colorAttachmentPxFormats.size());
    for (PixelFormat pxf : desc.colorAttachmentPxFormats)
        colorAttachmentFormats.push_back(toVkFormat(pxf));

    auto pipelineRenderingCreateInfo = vk::PipelineRenderingCreateInfo()
        .setColorAttachmentFormats(colorAttachmentFormats);

    if (desc.depthAttachmentPxFormat.has_value())
        pipelineRenderingCreateInfo.setDepthAttachmentFormat(toVkFormat(desc.depthAttachmentPxFormat.value()));

    auto graphicsPipelineCreateInfo = vk::GraphicsPipelineCreateInfo()
        .setStages(shaderStages)
        .setPVertexInputState(&vertexInputStateCreateInfo)
        .setPInputAssemblyState(&inputAssemblyStateCreateInfo)
        .setPViewportState(&viewportStateCreateInfo)
        .setPRasterizationState(&rasterizationStateCreateInfo)
        .setPMultisampleState(&multisampleStateCreateInfo)
        .setPColorBlendState(&colorBlendStateCreateInfo)
        .setPDynamicState(&dynamicStateCreateInfo)
        .setLayout(m_pipelineLayout)
        .setPNext(&pipelineRenderingCreateInfo);

    auto [result, pipelines] = m_device->vkDevice().createGraphicsPipelines(vk::PipelineCache{}, graphicsPipelineCreateInfo);
    if (result != vk::Result::eSuccess)
        throw ext::runtime_error("failed to create the GraphicsPipeline");
    m_vkPipeline = ext::move(pipelines.front());
}

VulkanGraphicsPipeline::~VulkanGraphicsPipeline()
{
    m_device->vkDevice().destroyPipeline(m_vkPipeline);
    m_device->vkDevice().destroyPipelineLayout(m_pipelineLayout);
}

}
