/*
 * ---------------------------------------------------
 * Material.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/09/26 07:30:59
 * ---------------------------------------------------
 */

#include "Material.hpp"
#include "AssetLoader.hpp"
#include "Graphics/Sampler.hpp"
#include "Renderer.hpp"

#include <Graphics/GraphicsPipeline.hpp>
#include <Graphics/ParameterBlock.hpp>
#include <Graphics/ParameterBlockPool.hpp>
#include <Graphics/Device.hpp>

#include <memory>

#include <glm/glm.hpp>

namespace scop
{

const gfx::ParameterBlock::Layout flatColorMaterialBpLayout = {
    .bindings = {
        gfx::ParameterBlock::Binding{ .type = gfx::BindingType::uniformBuffer, .usages = gfx::BindingUsage::fragmentRead },
        gfx::ParameterBlock::Binding{ .type = gfx::BindingType::uniformBuffer, .usages = gfx::BindingUsage::fragmentRead },
        gfx::ParameterBlock::Binding{ .type = gfx::BindingType::uniformBuffer, .usages = gfx::BindingUsage::fragmentRead },
    }
};

FlatColorMaterial::FlatColorMaterial(const gfx::Device& device)
{
    m_color = device.newBuffer(gfx::Buffer::Descriptor{
        .size = sizeof(glm::vec3),
        .usages = gfx::BufferUsage::uniformBuffer,
        .storageMode = gfx::ResourceStorageMode::hostVisible});
    setColor(glm::vec3(1.0f, 1.0f, 1.0f));

    m_shininess = device.newBuffer(gfx::Buffer::Descriptor{
        .size = sizeof(float),
        .usages = gfx::BufferUsage::uniformBuffer,
        .storageMode = gfx::ResourceStorageMode::hostVisible});
    setShininess(32.0f);

    m_specular = device.newBuffer(gfx::Buffer::Descriptor{
        .size = sizeof(float),
        .usages = gfx::BufferUsage::uniformBuffer,
        .storageMode = gfx::ResourceStorageMode::hostVisible});
    setSpecular(0.5f);
}

void FlatColorMaterial::createPipeline(gfx::Device& device)
{
    if (s_graphicsPipeline != nullptr)
        return;
    ext::unique_ptr<gfx::ShaderLib> shaderLib = device.newShaderLib(SHADER_DIR "/flat_color.slib");
    assert(shaderLib);

    gfx::GraphicsPipeline::Descriptor gfxPipelineDescriptor = {
        .vertexLayout = gfx::VertexLayout{
            .stride = sizeof(Vertex),
            .attributes = {
                gfx::VertexAttribute{
                    .format = gfx::VertexAttributeFormat::float3,
                    .offset = offsetof(scop::Vertex, pos)},
                gfx::VertexAttribute{
                    .format = gfx::VertexAttributeFormat::float3,
                    .offset = offsetof(scop::Vertex, normal)}
            }},
        .vertexShader = &shaderLib->getFunction("vertexMain"),
        .fragmentShader = &shaderLib->getFunction("fragmentMain"),
        .colorAttachmentPxFormats = {gfx::PixelFormat::BGRA8Unorm},
        .depthAttachmentPxFormat = gfx::PixelFormat::Depth32Float,
        .parameterBlockLayouts = {vpMatrixBpLayout, modelMatrixBpLayout, sceneDataBpLayout, flatColorMaterialBpLayout}};

    s_graphicsPipeline = device.newGraphicsPipeline(gfxPipelineDescriptor);
}

std::unique_ptr<gfx::ParameterBlock> FlatColorMaterial::makeParameterBlock(gfx::ParameterBlockPool& pool) const
{
    ext::unique_ptr<gfx::ParameterBlock> parameterBlock = pool.get(flatColorMaterialBpLayout);
    parameterBlock->setBinding(0, m_color);
    parameterBlock->setBinding(1, m_shininess);
    parameterBlock->setBinding(2, m_specular);
    return parameterBlock;
}

const gfx::ParameterBlock::Layout texturedCubeMaterialBpLayout = {
    .bindings = {
        gfx::ParameterBlock::Binding{ .type = gfx::BindingType::sampledTexture, .usages = gfx::BindingUsage::fragmentRead },
        gfx::ParameterBlock::Binding{ .type = gfx::BindingType::sampler, .usages = gfx::BindingUsage::fragmentRead }
    }};

TexturedCubeMaterial::TexturedCubeMaterial(const gfx::Device& device)
{
    m_sampler = device.newSampler(gfx::Sampler::Descriptor{});
}

void TexturedCubeMaterial::createPipeline(gfx::Device& device)
{
    if (s_graphicsPipeline != nullptr)
        return;
    ext::unique_ptr<gfx::ShaderLib> shaderLib = device.newShaderLib(SHADER_DIR "/textured_cube.slib");
    assert(shaderLib);

    gfx::GraphicsPipeline::Descriptor gfxPipelineDescriptor = {
        .vertexLayout = gfx::VertexLayout{
            .stride = sizeof(Vertex),
            .attributes = {
                gfx::VertexAttribute{
                    .format = gfx::VertexAttributeFormat::float3,
                    .offset = offsetof(scop::Vertex, pos)}
            }},
        .vertexShader = &shaderLib->getFunction("vertexMain"),
        .fragmentShader = &shaderLib->getFunction("fragmentMain"),
        .colorAttachmentPxFormats = {gfx::PixelFormat::BGRA8Unorm},
        .depthAttachmentPxFormat = gfx::PixelFormat::Depth32Float,
        .parameterBlockLayouts = {vpMatrixBpLayout, modelMatrixBpLayout, texturedCubeMaterialBpLayout}};

    s_graphicsPipeline = device.newGraphicsPipeline(gfxPipelineDescriptor);
}

std::unique_ptr<gfx::ParameterBlock> TexturedCubeMaterial::makeParameterBlock(gfx::ParameterBlockPool& pool) const
{
    ext::unique_ptr<gfx::ParameterBlock> parameterBlock = pool.get(texturedCubeMaterialBpLayout);
    parameterBlock->setBinding(0, m_texture);
    parameterBlock->setBinding(1, m_sampler);
    return parameterBlock;
}

} // namespace scop
