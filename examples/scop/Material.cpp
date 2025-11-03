/*
 * ---------------------------------------------------
 * Material.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/09/26 07:30:59
 * ---------------------------------------------------
 */

#include "Material.hpp"
#include "Graphics/Enums.hpp"
#include "Renderer.hpp"

#include "shaders/Vertex.slang"
#include "shaders/flat_color.slang"
#include "shaders/textured.slang"

#include <Graphics/GraphicsPipeline.hpp>
#include <Graphics/ParameterBlock.hpp>
#include <Graphics/ParameterBlockPool.hpp>
#include <Graphics/Device.hpp>
#include <Graphics/Sampler.hpp>

#include <memory>

#include <glm/glm.hpp>

namespace scop
{

const gfx::ParameterBlock::Layout flatColorMaterialBpLayout = {
    .bindings = {
        gfx::ParameterBlock::Binding{ .type = gfx::BindingType::uniformBuffer, .usages = gfx::BindingUsage::fragmentRead },
    }
};

FlatColorMaterial::FlatColorMaterial(gfx::Device& device)
{
    auto pipeline = s_graphicsPipeline.lock();
    if (!pipeline) {
        std::unique_ptr<gfx::ShaderLib> shaderLib = device.newShaderLib(SHADER_DIR "/flat_color.slib");
        assert(shaderLib);
        gfx::GraphicsPipeline::Descriptor gfxPipelineDescriptor = {
            .vertexLayout = gfx::VertexLayout{
                .stride = sizeof(shader::Vertex),
                .attributes = {
                    gfx::VertexAttribute{
                        .format = gfx::VertexAttributeFormat::float3,
                        .offset = offsetof(shader::Vertex, pos)},
                    gfx::VertexAttribute{
                        .format = gfx::VertexAttributeFormat::float2,
                        .offset = offsetof(shader::Vertex, uv)},
                    gfx::VertexAttribute{
                        .format = gfx::VertexAttributeFormat::float3,
                        .offset = offsetof(shader::Vertex, normal)}
                }
            },
            .vertexShader = &shaderLib->getFunction("vertexMain"),
            .fragmentShader = &shaderLib->getFunction("fragmentMain"),
            .colorAttachmentPxFormats = {gfx::PixelFormat::BGRA8Unorm},
            .depthAttachmentPxFormat = gfx::PixelFormat::Depth32Float,
            .blendOperation = gfx::BlendOperation::blendingOff,
            .cullMode = gfx::CullMode::back,
            .parameterBlockLayouts = { vpMatrixBpLayout, modelMatrixBpLayout, sceneDataBpLayout, flatColorMaterialBpLayout }
        };
        pipeline = device.newGraphicsPipeline(gfxPipelineDescriptor);
        assert(pipeline);
        s_graphicsPipeline = pipeline;
    }
    m_graphicsPipeline = pipeline;
    m_materialData = device.newBuffer(gfx::Buffer::Descriptor{
        .size = sizeof(shader::flat_color::MaterialData),
        .usages = gfx::BufferUsage::uniformBuffer,
        .storageMode = gfx::ResourceStorageMode::hostVisible});
    setDiffuseColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    setSpecularColor(glm::vec3(0.0f, 0.0f, 0.0f));
    setShininess(1.0f);
}

std::unique_ptr<gfx::ParameterBlock> FlatColorMaterial::makeParameterBlock(gfx::ParameterBlockPool& pool) const
{
    std::unique_ptr<gfx::ParameterBlock> parameterBlock = pool.get(flatColorMaterialBpLayout);
    parameterBlock->setBinding(0, m_materialData);
    return parameterBlock;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const gfx::ParameterBlock::Layout texturedMaterialBpLayout = {
    .bindings = {
        gfx::ParameterBlock::Binding{ .type = gfx::BindingType::sampler, .usages = gfx::BindingUsage::fragmentRead },
        gfx::ParameterBlock::Binding{ .type = gfx::BindingType::sampledTexture, .usages = gfx::BindingUsage::fragmentRead },
        gfx::ParameterBlock::Binding{ .type = gfx::BindingType::sampledTexture, .usages = gfx::BindingUsage::fragmentRead },
        gfx::ParameterBlock::Binding{ .type = gfx::BindingType::sampledTexture, .usages = gfx::BindingUsage::fragmentRead },
        gfx::ParameterBlock::Binding{ .type = gfx::BindingType::uniformBuffer, .usages = gfx::BindingUsage::fragmentRead }
    }
};

TexturedMaterial::TexturedMaterial(gfx::Device& device)
{
    auto pipeline = s_graphicsPipeline.lock();
    if (!pipeline) {
        std::unique_ptr<gfx::ShaderLib> shaderLib = device.newShaderLib(SHADER_DIR "/textured.slib");
        assert(shaderLib);
        gfx::GraphicsPipeline::Descriptor gfxPipelineDescriptor = {
            .vertexLayout = gfx::VertexLayout{
                .stride = sizeof(shader::Vertex),
                .attributes = {
                    gfx::VertexAttribute{
                        .format = gfx::VertexAttributeFormat::float3,
                        .offset = offsetof(shader::Vertex, pos)},
                    gfx::VertexAttribute{
                        .format = gfx::VertexAttributeFormat::float2,
                        .offset = offsetof(shader::Vertex, uv)},
                    gfx::VertexAttribute{
                        .format = gfx::VertexAttributeFormat::float3,
                        .offset = offsetof(shader::Vertex, normal)},
                    gfx::VertexAttribute{
                        .format = gfx::VertexAttributeFormat::float3,
                        .offset = offsetof(shader::Vertex, tangent)}
                }},
            .vertexShader = &shaderLib->getFunction("vertexMain"),
            .fragmentShader = &shaderLib->getFunction("fragmentMain"),
            .colorAttachmentPxFormats = {gfx::PixelFormat::BGRA8Unorm},
            .depthAttachmentPxFormat = gfx::PixelFormat::Depth32Float,
            .blendOperation = gfx::BlendOperation::blendingOff,
            .cullMode = gfx::CullMode::back,
            .parameterBlockLayouts = {vpMatrixBpLayout, modelMatrixBpLayout, sceneDataBpLayout, texturedMaterialBpLayout}
        };
        pipeline = device.newGraphicsPipeline(gfxPipelineDescriptor);
        assert(pipeline);
        s_graphicsPipeline = pipeline;
    }
    m_graphicsPipeline = pipeline;
    setSampler(device.newSampler(gfx::Sampler::Descriptor{
        .sAddressMode=gfx::SamplerAddressMode::Repeat,
        .tAddressMode=gfx::SamplerAddressMode::Repeat,
        .rAddressMode=gfx::SamplerAddressMode::Repeat,
        .minFilter = gfx::SamplerMinMagFilter::Linear,
        .magFilter = gfx::SamplerMinMagFilter::Linear
    }));
    m_materialData = device.newBuffer(gfx::Buffer::Descriptor{
        .size = sizeof(shader::textured::MaterialData),
        .usages = gfx::BufferUsage::uniformBuffer,
        .storageMode = gfx::ResourceStorageMode::hostVisible});
    setDiffuseColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    setSpecularColor(glm::vec3(0.0f, 0.0f, 0.0f));
    setShininess(1.0f);
    setEmissiveColor(glm::vec3(0.0f, 0.0f, 0.0f));
}

std::unique_ptr<gfx::ParameterBlock> TexturedMaterial::makeParameterBlock(gfx::ParameterBlockPool& pool) const
{
    std::unique_ptr<gfx::ParameterBlock> parameterBlock = pool.get(texturedMaterialBpLayout);
    parameterBlock->setBinding(0, m_sampler);
    parameterBlock->setBinding(1, m_diffuseTexture);
    parameterBlock->setBinding(2, m_emissiveTexture);
    parameterBlock->setBinding(3, m_normalTexture);
    parameterBlock->setBinding(4, m_materialData);
    return parameterBlock;
}

} // namespace scop
