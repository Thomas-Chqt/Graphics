/*
 * ---------------------------------------------------
 * Material.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/09/26 07:30:59
 * ---------------------------------------------------
 */

#include "Material.hpp"
#include "Renderer.hpp"
#include "Mesh.hpp"

#include "shaders/flat_color.slang"
#include "shaders/pbr_material.slang"
#include "shaders/textured.slang"

#include <Graphics/GraphicsPipeline.hpp>
#include <Graphics/ParameterBlock.hpp>
#include <Graphics/ParameterBlockPool.hpp>
#include <Graphics/Device.hpp>
#include <Graphics/Sampler.hpp>
#include <Graphics/Enums.hpp>

#if !defined (SCOP_MANDATORY)
    #include <glm/glm.hpp>
#else
    #include "math/math.hpp"
    #ifndef SCOP_MATH_GLM_ALIAS_DEFINED
        #define SCOP_MATH_GLM_ALIAS_DEFINED
        namespace glm = scop::math;
    #endif
#endif

#include <cassert>
#include <memory>
#include <stdexcept>

namespace scop
{

FlatColorMaterial::FlatColorMaterial(const gfx::Device& device)
{
    auto pbLayout = s_parameterBlockLayout.lock();
    if (!pbLayout) {
        pbLayout = device.newParameterBlockLayout(gfx::ParameterBlockLayout::Descriptor{
            .bindings = {
                gfx::ParameterBlockBinding{ .type = gfx::BindingType::constantBuffer, .usages = gfx::BindingUsage::fragmentRead },
            }
        });
        assert(pbLayout);
        s_parameterBlockLayout = pbLayout;
    }
    m_parameterBlockLayout = pbLayout;

    auto pipeline = s_graphicsPipeline.lock();
    if (!pipeline) {
        std::unique_ptr<gfx::ShaderLib> shaderLib = device.newShaderLib(SHADER_DIR "/flat_color.slib");
        assert(shaderLib);
        gfx::GraphicsPipeline::Descriptor gfxPipelineDescriptor = {
            .vertexLayout = gfx::VertexLayout{
                .stride = sizeof(Vertex),
                .attributes = {
                    gfx::VertexAttribute{
                        .format = gfx::VertexAttributeFormat::float3,
                        .offset = offsetof(Vertex, pos)},
                    gfx::VertexAttribute{
                        .format = gfx::VertexAttributeFormat::float3,
                        .offset = offsetof(Vertex, normal)},
                }
            },
            .vertexShader = &shaderLib->getFunction("vertexMain"),
            .fragmentShader = &shaderLib->getFunction("fragmentMain"),
            .colorAttachmentPxFormats = {gfx::PixelFormat::BGRA8_sRGB},
            .depthAttachmentPxFormat = gfx::PixelFormat::Depth32Float,
            .blendOperation = gfx::BlendOperation::blendingOff,
            .cullMode = gfx::CullMode::back,
            .parameterBlockLayouts = { Renderer::vpMatrixBpLayout(), Renderer::sceneDataBpLayout(), m_parameterBlockLayout }
        };
        pipeline = device.newGraphicsPipeline(gfxPipelineDescriptor);
        assert(pipeline);
        s_graphicsPipeline = pipeline;
    }
    m_graphicsPipeline = pipeline;

    m_materialData = device.newBuffer(gfx::Buffer::Descriptor{
        .size = sizeof(shader::flat_color::MaterialData),
        .usages = gfx::BufferUsage::constantBuffer,
        .storageMode = gfx::ResourceStorageMode::hostVisible});
    setDiffuseColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    setSpecularColor(glm::vec3(0.0f, 0.0f, 0.0f));
    setShininess(1.0f);
}

void FlatColorMaterial::makeParameterBlock(gfx::ParameterBlockPool& pool)
{
    m_parameterBlock = pool.get(m_parameterBlockLayout);
    m_parameterBlock->setBinding(0, m_materialData);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TexturedMaterial::TexturedMaterial(const gfx::Device& device)
{
    auto pbLayout = s_parameterBlockLayout.lock();
    if (!pbLayout) {
        pbLayout = device.newParameterBlockLayout(gfx::ParameterBlockLayout::Descriptor{
            .bindings = {
                gfx::ParameterBlockBinding{ .type = gfx::BindingType::sampler,        .usages = gfx::BindingUsage::fragmentRead },
                gfx::ParameterBlockBinding{ .type = gfx::BindingType::sampledTexture, .usages = gfx::BindingUsage::fragmentRead },
                gfx::ParameterBlockBinding{ .type = gfx::BindingType::sampledTexture, .usages = gfx::BindingUsage::fragmentRead },
                gfx::ParameterBlockBinding{ .type = gfx::BindingType::sampledTexture, .usages = gfx::BindingUsage::fragmentRead },
                gfx::ParameterBlockBinding{ .type = gfx::BindingType::constantBuffer,  .usages = gfx::BindingUsage::fragmentRead }
            }
        });
        assert(pbLayout);
        s_parameterBlockLayout = pbLayout;
    }
    m_parameterBlockLayout = pbLayout;

    auto pipeline = s_graphicsPipeline.lock();
    if (!pipeline) {
        std::unique_ptr<gfx::ShaderLib> shaderLib = device.newShaderLib(SHADER_DIR "/textured.slib");
        assert(shaderLib);
        gfx::GraphicsPipeline::Descriptor gfxPipelineDescriptor = {
            .vertexLayout = gfx::VertexLayout{
                .stride = sizeof(Vertex),
                .attributes = {
                    gfx::VertexAttribute{
                        .format = gfx::VertexAttributeFormat::float3,
                        .offset = offsetof(Vertex, pos)},
                    gfx::VertexAttribute{
                        .format = gfx::VertexAttributeFormat::float2,
                        .offset = offsetof(Vertex, uv)},
                    gfx::VertexAttribute{
                        .format = gfx::VertexAttributeFormat::float3,
                        .offset = offsetof(Vertex, normal)},
                    gfx::VertexAttribute{
                        .format = gfx::VertexAttributeFormat::float4,
                        .offset = offsetof(Vertex, tangent)}
                }},
            .vertexShader = &shaderLib->getFunction("vertexMain"),
            .fragmentShader = &shaderLib->getFunction("fragmentMain"),
            .colorAttachmentPxFormats = {gfx::PixelFormat::BGRA8_sRGB},
            .depthAttachmentPxFormat = gfx::PixelFormat::Depth32Float,
            .blendOperation = gfx::BlendOperation::blendingOff,
            .cullMode = gfx::CullMode::back,
            .parameterBlockLayouts = { Renderer::vpMatrixBpLayout(), Renderer::sceneDataBpLayout(), m_parameterBlockLayout }
        };
        pipeline = device.newGraphicsPipeline(gfxPipelineDescriptor);
        assert(pipeline);
        s_graphicsPipeline = pipeline;
    }
    m_graphicsPipeline = pipeline;

    m_materialData = device.newBuffer(gfx::Buffer::Descriptor{
        .size = sizeof(shader::textured::MaterialData),
        .usages = gfx::BufferUsage::constantBuffer,
        .storageMode = gfx::ResourceStorageMode::hostVisible});

    setSampler(device.newSampler(gfx::Sampler::Descriptor{
        .sAddressMode=gfx::SamplerAddressMode::Repeat,
        .tAddressMode=gfx::SamplerAddressMode::Repeat,
        .rAddressMode=gfx::SamplerAddressMode::Repeat,
        .minFilter = gfx::SamplerMinMagFilter::Linear,
        .magFilter = gfx::SamplerMinMagFilter::Linear,
        .mipFilter = gfx::SamplerMipFilter::Linear
    }));

    setDiffuseColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    setSpecularColor(glm::vec3(0.0f, 0.0f, 0.0f));
    setShininess(1.0f);
    setEmissiveColor(glm::vec3(0.0f, 0.0f, 0.0f));
}

void TexturedMaterial::makeParameterBlock(gfx::ParameterBlockPool& pool)
{
    m_parameterBlock = pool.get(m_parameterBlockLayout);
    m_parameterBlock->setBinding(0, m_sampler);
    m_parameterBlock->setBinding(1, m_diffuseTexture);
    m_parameterBlock->setBinding(2, m_emissiveTexture);
    m_parameterBlock->setBinding(3, m_normalTexture);
    m_parameterBlock->setBinding(4, m_materialData);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PbrMaterial::PbrMaterial(const gfx::Device& device)
{
    auto pbLayout = s_parameterBlockLayout.lock();
    if (!pbLayout) {
        pbLayout = device.newParameterBlockLayout(gfx::ParameterBlockLayout::Descriptor{
            .bindings = {
                gfx::ParameterBlockBinding{ .type = gfx::BindingType::sampler,        .usages = gfx::BindingUsage::fragmentRead },
                gfx::ParameterBlockBinding{ .type = gfx::BindingType::sampledTexture, .usages = gfx::BindingUsage::fragmentRead },
                gfx::ParameterBlockBinding{ .type = gfx::BindingType::sampledTexture, .usages = gfx::BindingUsage::fragmentRead },
                gfx::ParameterBlockBinding{ .type = gfx::BindingType::sampledTexture, .usages = gfx::BindingUsage::fragmentRead },
                gfx::ParameterBlockBinding{ .type = gfx::BindingType::sampledTexture, .usages = gfx::BindingUsage::fragmentRead },
                gfx::ParameterBlockBinding{ .type = gfx::BindingType::sampledTexture, .usages = gfx::BindingUsage::fragmentRead },
                gfx::ParameterBlockBinding{ .type = gfx::BindingType::sampler,        .usages = gfx::BindingUsage::fragmentRead },
                gfx::ParameterBlockBinding{ .type = gfx::BindingType::sampledTexture, .usages = gfx::BindingUsage::fragmentRead },
                gfx::ParameterBlockBinding{ .type = gfx::BindingType::sampledTexture, .usages = gfx::BindingUsage::fragmentRead },
                gfx::ParameterBlockBinding{ .type = gfx::BindingType::sampledTexture, .usages = gfx::BindingUsage::fragmentRead },
                gfx::ParameterBlockBinding{ .type = gfx::BindingType::constantBuffer, .usages = gfx::BindingUsage::fragmentRead }
            }
        });
        assert(pbLayout);
        s_parameterBlockLayout = pbLayout;
    }
    m_parameterBlockLayout = pbLayout;

    auto pipeline = s_graphicsPipeline.lock();
    if (!pipeline) {
        std::unique_ptr<gfx::ShaderLib> shaderLib = device.newShaderLib(SHADER_DIR "/pbr_material.slib");
        assert(shaderLib);
        pipeline = device.newGraphicsPipeline(gfx::GraphicsPipeline::Descriptor{
            .vertexLayout = gfx::VertexLayout{
                .stride = sizeof(Vertex),
                .attributes = {
                    gfx::VertexAttribute{ .format = gfx::VertexAttributeFormat::float3, .offset = offsetof(Vertex, pos) },
                    gfx::VertexAttribute{ .format = gfx::VertexAttributeFormat::float2, .offset = offsetof(Vertex, uv) },
                    gfx::VertexAttribute{ .format = gfx::VertexAttributeFormat::float3, .offset = offsetof(Vertex, normal) },
                    gfx::VertexAttribute{ .format = gfx::VertexAttributeFormat::float4, .offset = offsetof(Vertex, tangent) }
                }
            },
            .vertexShader = &shaderLib->getFunction("vertexMain"),
            .fragmentShader = &shaderLib->getFunction("fragmentMain"),
            .colorAttachmentPxFormats = { gfx::PixelFormat::BGRA8_sRGB },
            .depthAttachmentPxFormat = gfx::PixelFormat::Depth32Float,
            .blendOperation = gfx::BlendOperation::blendingOff,
            .cullMode = gfx::CullMode::none,
            .parameterBlockLayouts = { Renderer::vpMatrixBpLayout(), Renderer::sceneDataBpLayout(), m_parameterBlockLayout }
        });
        assert(pipeline);
        s_graphicsPipeline = pipeline;
    }
    m_graphicsPipeline = pipeline;

    m_materialData = device.newBuffer(gfx::Buffer::Descriptor{
        .size = sizeof(shader::pbr_material::MaterialData),
        .usages = gfx::BufferUsage::constantBuffer,
        .storageMode = gfx::ResourceStorageMode::hostVisible
    });
    assert(m_materialData);

    setSampler(device.newSampler(gfx::Sampler::Descriptor{
        .sAddressMode = gfx::SamplerAddressMode::Repeat,
        .tAddressMode = gfx::SamplerAddressMode::Repeat,
        .rAddressMode = gfx::SamplerAddressMode::Repeat,
        .minFilter = gfx::SamplerMinMagFilter::Linear,
        .magFilter = gfx::SamplerMinMagFilter::Linear,
        .mipFilter = gfx::SamplerMipFilter::Linear
    }));
    assert(m_sampler);

    m_environmentSampler = device.newSampler(gfx::Sampler::Descriptor{
        .sAddressMode = gfx::SamplerAddressMode::ClampToEdge,
        .tAddressMode = gfx::SamplerAddressMode::ClampToEdge,
        .rAddressMode = gfx::SamplerAddressMode::ClampToEdge,
        .minFilter = gfx::SamplerMinMagFilter::Linear,
        .magFilter = gfx::SamplerMinMagFilter::Linear,
        .mipFilter = gfx::SamplerMipFilter::Linear
    });
    assert(m_environmentSampler);

    m_irradianceCubemap = Renderer::irradianceCubemap();
    m_prefilteredEnvironmentCubemap = Renderer::prefilteredEnvironmentCubemap();
    m_brdfLut = Renderer::brdfLut();
    assert(m_irradianceCubemap);
    assert(m_prefilteredEnvironmentCubemap);
    assert(m_brdfLut);

    setBaseColor(glm::vec4(1.0f));
    setEmissiveFactor(glm::vec3(0.0f));
    setMetallic(0.0f);
    setRoughness(1.0f);
    setMetallicTextureChannel(0);
    setRoughnessTextureChannel(0);
}

void PbrMaterial::makeParameterBlock(gfx::ParameterBlockPool& pool)
{
    m_parameterBlock = pool.get(m_parameterBlockLayout);
    m_parameterBlock->setBinding(0, m_sampler);
    m_parameterBlock->setBinding(1, m_baseColorTexture);
    m_parameterBlock->setBinding(2, m_normalTexture);
    m_parameterBlock->setBinding(3, m_metallicTexture);
    m_parameterBlock->setBinding(4, m_roughnessTexture);
    m_parameterBlock->setBinding(5, m_emissiveTexture);
    m_parameterBlock->setBinding(6, m_environmentSampler);
    m_parameterBlock->setBinding(7, m_irradianceCubemap);
    m_parameterBlock->setBinding(8, m_prefilteredEnvironmentCubemap);
    m_parameterBlock->setBinding(9, m_brdfLut);
    m_parameterBlock->setBinding(10, m_materialData);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ScopMaterial::ScopMaterial(const gfx::Device& device)
{
    auto pbLayout = s_parameterBlockLayout.lock();
    if (!pbLayout) {
        pbLayout = device.newParameterBlockLayout(gfx::ParameterBlockLayout::Descriptor{
            .bindings = {
                gfx::ParameterBlockBinding{ .type = gfx::BindingType::sampler,        .usages = gfx::BindingUsage::fragmentRead },
                gfx::ParameterBlockBinding{ .type = gfx::BindingType::sampledTexture, .usages = gfx::BindingUsage::fragmentRead },
                gfx::ParameterBlockBinding{ .type = gfx::BindingType::constantBuffer,  .usages = gfx::BindingUsage::fragmentRead },
            }
        });
        assert(pbLayout);
        s_parameterBlockLayout = pbLayout;
    }
    m_parameterBlockLayout = pbLayout;

    auto pipeline = s_graphicsPipeline.lock();
    if (!pipeline) {
        std::unique_ptr<gfx::ShaderLib> shaderLib = device.newShaderLib(SHADER_DIR "/scop.slib");
        assert(shaderLib);

        gfx::GraphicsPipeline::Descriptor gfxPipelineDescriptor = {
            .vertexLayout = gfx::VertexLayout{
                .stride = sizeof(Vertex),
                .attributes = {
                    gfx::VertexAttribute{
                        .format = gfx::VertexAttributeFormat::float3,
                        .offset = offsetof(Vertex, pos)},
                    gfx::VertexAttribute{
                        .format = gfx::VertexAttributeFormat::float3,
                        .offset = offsetof(Vertex, normal)},
                }
            },
            .vertexShader = &shaderLib->getFunction("vertexMain"),
            .fragmentShader = &shaderLib->getFunction("fragmentMain"),
            .colorAttachmentPxFormats = {gfx::PixelFormat::BGRA8_sRGB},
            .depthAttachmentPxFormat = gfx::PixelFormat::Depth32Float,
            .blendOperation = gfx::BlendOperation::blendingOff,
            .cullMode = gfx::CullMode::back,
            .parameterBlockLayouts = { Renderer::vpMatrixBpLayout(), Renderer::sceneDataBpLayout(), m_parameterBlockLayout }
        };
        pipeline = device.newGraphicsPipeline(gfxPipelineDescriptor);
        assert(pipeline);
        s_graphicsPipeline = pipeline;
    }
    m_graphicsPipeline = pipeline;

    m_materialData = device.newBuffer(gfx::Buffer::Descriptor{
        .size = sizeof(shader::scop::MaterialData),
        .usages = gfx::BufferUsage::constantBuffer,
        .storageMode = gfx::ResourceStorageMode::hostVisible});

    setSampler(device.newSampler(gfx::Sampler::Descriptor{
        .sAddressMode = gfx::SamplerAddressMode::Repeat,
        .tAddressMode = gfx::SamplerAddressMode::Repeat,
        .rAddressMode = gfx::SamplerAddressMode::Repeat,
        .minFilter = gfx::SamplerMinMagFilter::Linear,
        .magFilter = gfx::SamplerMinMagFilter::Linear,
        .mipFilter = gfx::SamplerMipFilter::Linear
    }));

    setTextureStrength(0.0f);

    setPaletteColor(0, {1.0f, 1.0f, 1.0f, 1.0f});
    setPaletteColor(1, {0.3f, 0.3f, 0.3f, 1.0f});
    setPaletteColor(2, {0.6f, 0.6f, 0.6f, 1.0f});
    setPaletteColor(3, {1.0f, 1.0f, 1.0f, 1.0f});
    setPaletteColor(4, {0.6f, 0.6f, 0.6f, 1.0f});
    setPaletteColor(5, {0.3f, 0.3f, 0.3f, 1.0f});
}

void ScopMaterial::makeParameterBlock(gfx::ParameterBlockPool& pool)
{
    m_parameterBlock = pool.get(m_parameterBlockLayout);
    m_parameterBlock->setBinding(0, m_sampler);
    m_parameterBlock->setBinding(1, m_diffuseTexture);
    m_parameterBlock->setBinding(2, m_materialData);
}

glm::vec4 ScopMaterial::paletteColor(int i) const
{
    if (i < 0 || i >= 6)
        throw std::out_of_range("ScopMaterial::paletteColor: index out of range (expected 0..5)");
    return m_materialData->content<shader::scop::MaterialData>()->colorPalette[i]; // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
}

void ScopMaterial::setPaletteColor(int i, const glm::vec4& c)
{
    if (i < 0 || i >= 6)
        throw std::out_of_range("ScopMaterial::setPaletteColor: index out of range (expected 0..5)");
    m_materialData->content<shader::scop::MaterialData>()->colorPalette[i] = c; // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
}

} // namespace scop
