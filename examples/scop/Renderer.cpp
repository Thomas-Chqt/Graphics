/*
 * ---------------------------------------------------
 * Renderer.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/09/18 09:26:08
 * ---------------------------------------------------
 */

#include "Renderer.hpp"
#include "Mesh.hpp"
#include "shaders/SceneData.slang"
#include "shaders/Light.slang"

#include <Graphics/Buffer.hpp>
#include <Graphics/CommandBuffer.hpp>
#include <Graphics/ComputePipeline.hpp>
#include <Graphics/Enums.hpp>
#include <Graphics/ParameterBlock.hpp>
#include <Graphics/ParameterBlockPool.hpp>
#include <Graphics/PassDescriptor.hpp>
#include <Graphics/Sampler.hpp>
#include <Graphics/ShaderLib.hpp>
#include <Graphics/Texture.hpp>

#include <GLFW/glfw3.h>
#include <stb_image/stb_image.h>
#if !defined (SCOP_MANDATORY)
    #include <imgui.h>
    #include <gfx_imgui/gfx_imgui.hpp>
    #include <backends/imgui_impl_glfw.h>
    #include <glm/glm.hpp>
    #include <glm/gtc/matrix_transform.hpp>
    #include <tracy/Tracy.hpp>
    #include <tracy/TracyC.h>
    #include <gfx_tracy/gfx_tracy.hpp>
#else
    #include "math/math.hpp"
    #ifndef SCOP_MATH_GLM_ALIAS_DEFINED
        #define SCOP_MATH_GLM_ALIAS_DEFINED
        namespace glm = scop::math;
    #endif
#endif

#include <array>
#include <algorithm>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <format>
#include <functional>
#include <cassert>
#include <memory>
#include <stdexcept>
#include <utility>

namespace scop
{

namespace
{

uint16_t floatToSaturatedHalf(float value)
{
    constexpr float maxFiniteHalf = 65504.0f;
    value = std::clamp(value, -maxFiniteHalf, maxFiniteHalf);

    const uint32_t bits = std::bit_cast<uint32_t>(value);
    const uint32_t sign = (bits >> 16) & 0x8000u;
    const int32_t exponent = static_cast<int32_t>((bits >> 23) & 0xFFu) - 127 + 15;
    const uint32_t mantissa = bits & 0x7FFFFFu;

    if (exponent >= 31)
        return static_cast<uint16_t>(sign | 0x7BFFu);
    if (exponent <= 0)
        return static_cast<uint16_t>(sign); // denormals flushed to zero
    return static_cast<uint16_t>(sign | (static_cast<uint32_t>(exponent) << 10) | (mantissa >> 13));
}

std::shared_ptr<gfx::Texture> createEnvironmentCubemap(gfx::Device& device, gfx::ShaderLib& pbrPrecomputeSlib)
{
    constexpr const char* environmentPath = RESOURCE_DIR"/environment.hdr";

    int width = 0;
    int height = 0;
    std::unique_ptr<float, decltype(&stbi_image_free)> pixels(
        stbi_loadf(environmentPath, &width, &height, nullptr, STBI_rgb_alpha),
        stbi_image_free
    );
    if (!pixels)
    {
        const char* failureReason = stbi_failure_reason();
        throw std::runtime_error(std::format(
            "failed to load HDR image: {}",
            failureReason != nullptr ? failureReason : "unknown error"
        ));
    }
    if (width != height * 2 || height < 2)
        throw std::runtime_error("environment HDR must be a 2:1 equirectangular image");

    const auto faceSize = static_cast<uint32_t>(height / 2);
    const size_t componentCount = static_cast<size_t>(width) * static_cast<size_t>(height) * 4;
    const size_t uploadSize =
        static_cast<size_t>(width) *
        static_cast<size_t>(height) *
        gfx::pixelFormatSize(gfx::PixelFormat::RGBA16_float);

    std::shared_ptr<gfx::Buffer> uploadBuffer = device.newBuffer({
        .size = uploadSize,
        .usages = gfx::BufferUsage::copySource,
        .storageMode = gfx::ResourceStorageMode::hostVisible
    });
    assert(uploadBuffer);
    auto* halfPixels = uploadBuffer->content<uint16_t>();
    for (size_t i = 0; i < componentCount; ++i)
        halfPixels[i] = floatToSaturatedHalf(pixels.get()[i]); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)

    std::shared_ptr<gfx::Texture> equirectangularTexture = device.newTexture({
        .type = gfx::TextureType::texture2d,
        .width = static_cast<uint32_t>(width),
        .height = static_cast<uint32_t>(height),
        .mipLevelCount = 1,
        .arrayLayerCount = 1,
        .pixelFormat = gfx::PixelFormat::RGBA16_float,
        .usages = gfx::TextureUsage::copyDestination | gfx::TextureUsage::shaderRead,
        .storageMode = gfx::ResourceStorageMode::deviceLocal
    });
    assert(equirectangularTexture);

    std::shared_ptr<gfx::Texture> cubemap = device.newTexture({
        .type = gfx::TextureType::textureCube,
        .width = faceSize,
        .height = faceSize,
        .mipLevelCount = static_cast<uint32_t>(std::bit_width(faceSize)),
        .arrayLayerCount = 1,
        .pixelFormat = gfx::PixelFormat::RGBA16_float,
        .usages =
            gfx::TextureUsage::copySource |
            gfx::TextureUsage::copyDestination |
            gfx::TextureUsage::shaderRead |
            gfx::TextureUsage::shaderWrite,
        .storageMode = gfx::ResourceStorageMode::deviceLocal
    });
    assert(cubemap);

    std::shared_ptr<gfx::Texture> writableFaces = device.newTextureView(cubemap, {
        .type = gfx::TextureType::texture2dArray,
        .baseMipLevel = 0,
        .mipLevelCount = 1,
        .baseArrayLayer = 0,
        .arrayLayerCount = 6
    });
    assert(writableFaces);

    std::shared_ptr<gfx::Sampler> sampler = device.newSampler({
        .sAddressMode = gfx::SamplerAddressMode::Repeat,
        .tAddressMode = gfx::SamplerAddressMode::ClampToEdge,
        .rAddressMode = gfx::SamplerAddressMode::ClampToEdge,
        .minFilter = gfx::SamplerMinMagFilter::Linear,
        .magFilter = gfx::SamplerMinMagFilter::Linear
    });
    assert(sampler);

    std::shared_ptr<gfx::ParameterBlockLayout> parameterBlockLayout = device.newParameterBlockLayout({
        .bindings = {
            { .type = gfx::BindingType::sampledTexture, .usages = gfx::BindingUsage::computeRead },
            { .type = gfx::BindingType::sampler, .usages = gfx::BindingUsage::computeRead },
            { .type = gfx::BindingType::storageTexture, .usages = gfx::BindingUsage::computeWrite }
        }
    });
    assert(parameterBlockLayout);

    std::shared_ptr<gfx::ComputePipeline> conversionPipeline = device.newComputePipeline({
        .computeShader = &pbrPrecomputeSlib.getFunction("equirectangularToCubemap"),
        .threadsPerThreadgroupX = 8,
        .threadsPerThreadgroupY = 8,
        .threadsPerThreadgroupZ = 1,
        .parameterBlockLayouts = { parameterBlockLayout }
    });
    assert(conversionPipeline);

    std::unique_ptr<gfx::ParameterBlockPool> parameterBlockPool = device.newParameterBlockPool({
        .maxBindingCount = {
            { gfx::BindingType::sampledTexture, 1 },
            { gfx::BindingType::storageTexture, 1 },
            { gfx::BindingType::sampler, 1 }
        }
    });
    assert(parameterBlockPool);

    std::shared_ptr<gfx::ParameterBlock> parameters = parameterBlockPool->get(parameterBlockLayout);
    parameters->setBinding(0, equirectangularTexture);
    parameters->setBinding(1, sampler);
    parameters->setBinding(2, writableFaces);

    std::unique_ptr<gfx::CommandBufferPool> commandBufferPool = device.newCommandBufferPool();
    assert(commandBufferPool);
    std::shared_ptr<gfx::CommandBuffer> commandBuffer = commandBufferPool->get();

    std::unique_ptr<gfx::BlitPassDescriptor> blitPass = device.newBlitPassDescriptor();
    commandBuffer->beginBlitPass(*blitPass);
    commandBuffer->copyBufferToTexture(uploadBuffer, equirectangularTexture);
    commandBuffer->endBlitPass();

    std::unique_ptr<gfx::ComputePassDescriptor> computePass = device.newComputePassDescriptor();
    commandBuffer->beginComputePass(*computePass);
    commandBuffer->usePipeline(conversionPipeline);
    commandBuffer->setParameterBlock(parameters, 0);
    commandBuffer->dispatchThreadgroups((faceSize + 7) / 8, (faceSize + 7) / 8, 6);
    commandBuffer->endComputePass();

    std::unique_ptr<gfx::BlitPassDescriptor> mipmapPass = device.newBlitPassDescriptor();
    commandBuffer->beginBlitPass(*mipmapPass);
    commandBuffer->generateMipmaps(cubemap);
    commandBuffer->endBlitPass();

    device.submitCommandBuffers(commandBuffer);

    return cubemap;
}

std::shared_ptr<gfx::Texture> createIrradianceCubemap(
    gfx::Device& device,
    gfx::ShaderLib& pbrPrecomputeSlib,
    const std::shared_ptr<gfx::Texture>& environmentCubemap
)
{
    assert(environmentCubemap);

    constexpr uint32_t irradianceFaceSize = 32;

    std::shared_ptr<gfx::Texture> irradianceCubemap = device.newTexture({
        .type = gfx::TextureType::textureCube,
        .width = irradianceFaceSize,
        .height = irradianceFaceSize,
        .mipLevelCount = 1,
        .arrayLayerCount = 1,
        .pixelFormat = gfx::PixelFormat::RGBA16_float,
        .usages = gfx::TextureUsage::shaderRead | gfx::TextureUsage::shaderWrite,
        .storageMode = gfx::ResourceStorageMode::deviceLocal
    });
    assert(irradianceCubemap);

    std::shared_ptr<gfx::Texture> writableFaces = device.newTextureView(irradianceCubemap, {
        .type = gfx::TextureType::texture2dArray,
        .baseMipLevel = 0,
        .mipLevelCount = 1,
        .baseArrayLayer = 0,
        .arrayLayerCount = 6
    });
    assert(writableFaces);

    std::shared_ptr<gfx::Sampler> sampler = device.newSampler({
        .sAddressMode = gfx::SamplerAddressMode::ClampToEdge,
        .tAddressMode = gfx::SamplerAddressMode::ClampToEdge,
        .rAddressMode = gfx::SamplerAddressMode::ClampToEdge,
        .minFilter = gfx::SamplerMinMagFilter::Linear,
        .magFilter = gfx::SamplerMinMagFilter::Linear,
        .mipFilter = gfx::SamplerMipFilter::Linear
    });
    assert(sampler);

    std::shared_ptr<gfx::ParameterBlockLayout> parameterBlockLayout = device.newParameterBlockLayout({
        .bindings = {
            { .type = gfx::BindingType::sampledTexture, .usages = gfx::BindingUsage::computeRead },
            { .type = gfx::BindingType::sampler, .usages = gfx::BindingUsage::computeRead },
            { .type = gfx::BindingType::storageTexture, .usages = gfx::BindingUsage::computeWrite }
        }
    });
    assert(parameterBlockLayout);

    std::shared_ptr<gfx::ComputePipeline> pipeline = device.newComputePipeline({
        .computeShader = &pbrPrecomputeSlib.getFunction("convolveDiffuseIrradiance"),
        .threadsPerThreadgroupX = 8,
        .threadsPerThreadgroupY = 8,
        .threadsPerThreadgroupZ = 1,
        .parameterBlockLayouts = { parameterBlockLayout }
    });
    assert(pipeline);

    std::unique_ptr<gfx::ParameterBlockPool> parameterBlockPool = device.newParameterBlockPool({
        .maxBindingCount = {
            { gfx::BindingType::sampledTexture, 1 },
            { gfx::BindingType::storageTexture, 1 },
            { gfx::BindingType::sampler, 1 }
        }
    });
    assert(parameterBlockPool);

    std::shared_ptr<gfx::ParameterBlock> parameters = parameterBlockPool->get(parameterBlockLayout);
    parameters->setBinding(0, environmentCubemap);
    parameters->setBinding(1, sampler);
    parameters->setBinding(2, writableFaces);

    std::unique_ptr<gfx::CommandBufferPool> commandBufferPool = device.newCommandBufferPool();
    assert(commandBufferPool);
    std::shared_ptr<gfx::CommandBuffer> commandBuffer = commandBufferPool->get();

    std::unique_ptr<gfx::ComputePassDescriptor> computePass = device.newComputePassDescriptor();
    commandBuffer->beginComputePass(*computePass);
    commandBuffer->usePipeline(pipeline);
    commandBuffer->setParameterBlock(parameters, 0);
    commandBuffer->dispatchThreadgroups(
        (irradianceFaceSize + 7) / 8,
        (irradianceFaceSize + 7) / 8,
        6
    );
    commandBuffer->endComputePass();

    device.submitCommandBuffers(commandBuffer);

    return irradianceCubemap;
}

std::shared_ptr<gfx::Texture> createPrefilteredEnvironmentCubemap(
    gfx::Device& device,
    gfx::ShaderLib& pbrPrecomputeSlib,
    const std::shared_ptr<gfx::Texture>& environmentCubemap
)
{
    assert(environmentCubemap);

    constexpr uint32_t faceSize = 128;
    constexpr uint32_t mipLevelCount = 5;

    std::shared_ptr<gfx::Texture> prefilteredCubemap = device.newTexture({
        .type = gfx::TextureType::textureCube,
        .width = faceSize,
        .height = faceSize,
        .mipLevelCount = mipLevelCount,
        .arrayLayerCount = 1,
        .pixelFormat = gfx::PixelFormat::RGBA16_float,
        .usages = gfx::TextureUsage::shaderRead | gfx::TextureUsage::shaderWrite,
        .storageMode = gfx::ResourceStorageMode::deviceLocal
    });
    assert(prefilteredCubemap);

    std::shared_ptr<gfx::Sampler> sampler = device.newSampler({
        .sAddressMode = gfx::SamplerAddressMode::ClampToEdge,
        .tAddressMode = gfx::SamplerAddressMode::ClampToEdge,
        .rAddressMode = gfx::SamplerAddressMode::ClampToEdge,
        .minFilter = gfx::SamplerMinMagFilter::Linear,
        .magFilter = gfx::SamplerMinMagFilter::Linear,
        .mipFilter = gfx::SamplerMipFilter::Linear
    });
    assert(sampler);

    std::shared_ptr<gfx::ParameterBlockLayout> resourcePBlockLayout = device.newParameterBlockLayout({
        .bindings = {
            { .type = gfx::BindingType::sampledTexture, .usages = gfx::BindingUsage::computeRead },
            { .type = gfx::BindingType::sampler, .usages = gfx::BindingUsage::computeRead },
            { .type = gfx::BindingType::storageTexture, .usages = gfx::BindingUsage::computeWrite }
        }
    });
    assert(resourcePBlockLayout);

    std::shared_ptr<gfx::ParameterBlockLayout> roughnessPBlockLayout = device.newParameterBlockLayout({
        .bindings = {
            { .type = gfx::BindingType::constantBuffer, .usages = gfx::BindingUsage::computeRead }
        }
    });
    assert(roughnessPBlockLayout);

    std::shared_ptr<gfx::ComputePipeline> pipeline = device.newComputePipeline({
        .computeShader = &pbrPrecomputeSlib.getFunction("prefilterEnvironment"),
        .threadsPerThreadgroupX = 8,
        .threadsPerThreadgroupY = 8,
        .threadsPerThreadgroupZ = 1,
        .parameterBlockLayouts = { resourcePBlockLayout, roughnessPBlockLayout }
    });
    assert(pipeline);

    std::unique_ptr<gfx::ParameterBlockPool> parameterBlockPool = device.newParameterBlockPool({
        .maxBindingCount = {
            { gfx::BindingType::sampledTexture, mipLevelCount },
            { gfx::BindingType::storageTexture, mipLevelCount },
            { gfx::BindingType::sampler, mipLevelCount },
            { gfx::BindingType::constantBuffer, mipLevelCount }
        }
    });
    assert(parameterBlockPool);

    std::unique_ptr<gfx::CommandBufferPool> commandBufferPool = device.newCommandBufferPool();
    assert(commandBufferPool);
    std::shared_ptr<gfx::CommandBuffer> commandBuffer = commandBufferPool->get();

    std::unique_ptr<gfx::ComputePassDescriptor> computePass = device.newComputePassDescriptor();
    commandBuffer->beginComputePass(*computePass);
    commandBuffer->usePipeline(pipeline);

    for (uint32_t mipLevel = 0; mipLevel < mipLevelCount; ++mipLevel)
    {
        std::shared_ptr<gfx::Texture> writableFaces = device.newTextureView(prefilteredCubemap, {
            .type = gfx::TextureType::texture2dArray,
            .baseMipLevel = mipLevel,
            .mipLevelCount = 1,
            .baseArrayLayer = 0,
            .arrayLayerCount = 6
        });
        assert(writableFaces);

        std::shared_ptr<gfx::Buffer> roughnessBuffer = device.newBuffer({
            .size = sizeof(float),
            .usages = gfx::BufferUsage::constantBuffer,
            .storageMode = gfx::ResourceStorageMode::hostVisible
        });
        assert(roughnessBuffer);
        *roughnessBuffer->content<float>() = static_cast<float>(mipLevel) / static_cast<float>(mipLevelCount - 1);

        std::shared_ptr<gfx::ParameterBlock> resourcePBlock = parameterBlockPool->get(resourcePBlockLayout);
        resourcePBlock->setBinding(0, environmentCubemap);
        resourcePBlock->setBinding(1, sampler);
        resourcePBlock->setBinding(2, writableFaces);

        std::shared_ptr<gfx::ParameterBlock> roughnessPBlock = parameterBlockPool->get(roughnessPBlockLayout);
        roughnessPBlock->setBinding(0, roughnessBuffer);

        commandBuffer->setParameterBlock(resourcePBlock, 0);
        commandBuffer->setParameterBlock(roughnessPBlock, 1);

        const uint32_t mipSize = std::max(faceSize >> mipLevel, 1u);
        commandBuffer->dispatchThreadgroups(
            (mipSize + 7) / 8,
            (mipSize + 7) / 8,
            6
        );
    }

    commandBuffer->endComputePass();
    device.submitCommandBuffers(commandBuffer);

    return prefilteredCubemap;
}

std::shared_ptr<gfx::Texture> createBrdfLut(gfx::Device& device, gfx::ShaderLib& pbrPrecomputeSlib)
{
    constexpr uint32_t lutSize = 512;

    std::shared_ptr<gfx::Texture> brdfLut = device.newTexture({
        .type = gfx::TextureType::texture2d,
        .width = lutSize,
        .height = lutSize,
        .mipLevelCount = 1,
        .arrayLayerCount = 1,
        .pixelFormat = gfx::PixelFormat::RG16_float,
        .usages = gfx::TextureUsage::shaderRead | gfx::TextureUsage::shaderWrite,
        .storageMode = gfx::ResourceStorageMode::deviceLocal
    });
    assert(brdfLut);

    std::shared_ptr<gfx::ParameterBlockLayout> parameterBlockLayout = device.newParameterBlockLayout({
        .bindings = {
            { .type = gfx::BindingType::storageTexture, .usages = gfx::BindingUsage::computeWrite }
        }
    });
    assert(parameterBlockLayout);

    std::shared_ptr<gfx::ComputePipeline> pipeline = device.newComputePipeline({
        .computeShader = &pbrPrecomputeSlib.getFunction("integrateBrdf"),
        .threadsPerThreadgroupX = 8,
        .threadsPerThreadgroupY = 8,
        .threadsPerThreadgroupZ = 1,
        .parameterBlockLayouts = { parameterBlockLayout }
    });
    assert(pipeline);

    std::unique_ptr<gfx::ParameterBlockPool> parameterBlockPool = device.newParameterBlockPool({
        .maxBindingCount = {
            { gfx::BindingType::storageTexture, 1 }
        }
    });
    assert(parameterBlockPool);

    std::shared_ptr<gfx::ParameterBlock> parameters = parameterBlockPool->get(parameterBlockLayout);
    parameters->setBinding(0, brdfLut);

    std::unique_ptr<gfx::CommandBufferPool> commandBufferPool = device.newCommandBufferPool();
    assert(commandBufferPool);
    std::shared_ptr<gfx::CommandBuffer> commandBuffer = commandBufferPool->get();

    std::unique_ptr<gfx::ComputePassDescriptor> computePass = device.newComputePassDescriptor();
    commandBuffer->beginComputePass(*computePass);
    commandBuffer->usePipeline(pipeline);
    commandBuffer->setParameterBlock(parameters, 0);
    commandBuffer->dispatchThreadgroups((lutSize + 7) / 8, (lutSize + 7) / 8, 1);
    commandBuffer->endComputePass();

    device.submitCommandBuffers(commandBuffer);

    return brdfLut;
}

}

Renderer::Renderer(gfx::Device* device, GLFWwindow* window, gfx::Surface* surface)
    : m_device(device), m_window(window), m_surface(surface)
{
    assert(m_device);

    std::unique_ptr<gfx::ShaderLib> environmentPrecomputeSlib = m_device->newShaderLib(SHADER_DIR "/equirectangularToCubemap.slib");
    assert(environmentPrecomputeSlib);

    std::unique_ptr<gfx::ShaderLib> irradiancePrecomputeSlib = m_device->newShaderLib(SHADER_DIR "/convolveDiffuseIrradiance.slib");
    assert(irradiancePrecomputeSlib);

    std::unique_ptr<gfx::ShaderLib> prefilterPrecomputeSlib = m_device->newShaderLib(SHADER_DIR "/prefilterEnvironment.slib");
    assert(prefilterPrecomputeSlib);

    std::unique_ptr<gfx::ShaderLib> brdfPrecomputeSlib = m_device->newShaderLib(SHADER_DIR "/integrateBrdf.slib");
    assert(brdfPrecomputeSlib);

    m_environmentCubemap = createEnvironmentCubemap(*m_device, *environmentPrecomputeSlib);
    assert(m_environmentCubemap);

    m_irradianceCubemap = createIrradianceCubemap(*m_device, *irradiancePrecomputeSlib, m_environmentCubemap);
    assert(m_irradianceCubemap);
    s_irradianceCubemap = m_irradianceCubemap;

    m_prefilteredEnvironmentCubemap = createPrefilteredEnvironmentCubemap(*m_device, *prefilterPrecomputeSlib, m_environmentCubemap);
    assert(m_prefilteredEnvironmentCubemap);
    s_prefilteredEnvironmentCubemap = m_prefilteredEnvironmentCubemap;

    m_brdfLut = createBrdfLut(*m_device, *brdfPrecomputeSlib);
    assert(m_brdfLut);
    s_brdfLut = m_brdfLut;

    std::unique_ptr<gfx::ShaderLib> skyboxSlib = m_device->newShaderLib(SHADER_DIR "/skybox.slib");
    assert(skyboxSlib);

    m_skyboxPbLayout = m_device->newParameterBlockLayout({
        .bindings = {
            { .type = gfx::BindingType::sampledTexture, .usages = gfx::BindingUsage::fragmentRead },
            { .type = gfx::BindingType::sampler, .usages = gfx::BindingUsage::fragmentRead }
        }
    });
    assert(m_skyboxPbLayout);

    m_skyboxPipeline = m_device->newGraphicsPipeline({
        .vertexShader = &skyboxSlib->getFunction("vertexMain"),
        .fragmentShader = &skyboxSlib->getFunction("fragmentMain"),
        .colorAttachmentPxFormats = { gfx::PixelFormat::BGRA8_sRGB },
        .depthAttachmentPxFormat = gfx::PixelFormat::Depth32Float,
        .cullMode = gfx::CullMode::front,
        .depthCompareOperation = gfx::DepthCompareOperation::lessEqual,
        .parameterBlockLayouts = { m_skyboxPbLayout }
    });
    assert(m_skyboxPipeline);

    m_skyboxSampler = m_device->newSampler({
        .sAddressMode = gfx::SamplerAddressMode::ClampToEdge,
        .tAddressMode = gfx::SamplerAddressMode::ClampToEdge,
        .rAddressMode = gfx::SamplerAddressMode::ClampToEdge,
        .minFilter = gfx::SamplerMinMagFilter::Linear,
        .magFilter = gfx::SamplerMinMagFilter::Linear,
        .mipFilter = gfx::SamplerMipFilter::Linear
    });
    assert(m_skyboxSampler);

    m_tracyGraphicsContext = TracyGFXContext(*m_device);

    glfwSetWindowUserPointer(m_window, this);
    glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int, int){
        static_cast<Renderer*>(glfwGetWindowUserPointer(window))->m_swapchain = nullptr;
    });

    for (auto& frameData : m_frameDatas)
    {
        frameData.commandBufferPool = m_device->newCommandBufferPool();
        assert(frameData.commandBufferPool);

        frameData.parameterBlockPool = m_device->newParameterBlockPool({
            .maxBindingCount = {
                {gfx::BindingType::constantBuffer, 2},
                {gfx::BindingType::sampledTexture, 1},
                {gfx::BindingType::sampler, 1},
            }
        });
        assert(frameData.parameterBlockPool);

        frameData.vpMatrix = m_device->newBuffer(gfx::Buffer::Descriptor{
            .size = sizeof(glm::mat4x4),
            .usages = gfx::BufferUsage::constantBuffer,
            .storageMode = gfx::ResourceStorageMode::hostVisible});
        assert(frameData.vpMatrix);

        frameData.sceneDataBuffer = m_device->newBuffer(gfx::Buffer::Descriptor{
            .size = sizeof(shader::SceneData),
            .usages = gfx::BufferUsage::constantBuffer,
            .storageMode = gfx::ResourceStorageMode::hostVisible});
    }

    m_vpMatrixBpLayout = m_device->newParameterBlockLayout(gfx::ParameterBlockLayout::Descriptor{
        .bindings = {
            gfx::ParameterBlockBinding{ .type = gfx::BindingType::constantBuffer, .usages = gfx::BindingUsage::vertexRead }
        }
    });
    assert(m_vpMatrixBpLayout);
    s_vpMatrixBpLayout = m_vpMatrixBpLayout;

    m_sceneDataBpLayout = m_device->newParameterBlockLayout(gfx::ParameterBlockLayout::Descriptor{
        .bindings = {
            gfx::ParameterBlockBinding{ .type = gfx::BindingType::constantBuffer, .usages = gfx::BindingUsage::fragmentRead }
        }
    });
    assert(m_sceneDataBpLayout);
    s_sceneDataBpLayout = m_sceneDataBpLayout;

#if !defined (SCOP_MANDATORY)
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    switch (m_device->backend())
    {
        case gfx::Backend::vulkan:
            ImGui_ImplGlfw_InitForVulkan(m_window, true);
            break;
        default:
            ImGui_ImplGlfw_InitForOther(m_window, true);
            break;
    }

    gfx::imgui::init(*m_device, {.colorAttachmentPixelFormats = {gfx::PixelFormat::BGRA8_sRGB}, .depthAttachmentPixelFormat = gfx::PixelFormat::Depth32Float});
#endif
}

void Renderer::beginFrame(const glm::mat4x4& viewMatrix, const glm::vec3& cameraPosition, float fov, float near, float far)
{
    ZoneScoped;
    if (m_swapchain == nullptr) {
        int width = 0, height = 0;
        ::glfwGetFramebufferSize(m_window, &width, &height);
        gfx::Swapchain::Descriptor swapchainDescriptor = {
            .surface = m_surface,
            .width = (uint32_t)width,
            .height = (uint32_t)height,
            .imageCount = 3,
            .drawableCount = maxFrameInFlight,
            .pixelFormat = gfx::PixelFormat::BGRA8_sRGB,
            .presentMode = gfx::PresentMode::fifo,
        };
        m_swapchain = m_device->newSwapchain(swapchainDescriptor);
        assert(m_swapchain);

        gfx::Texture::Descriptor depthTextureDescriptor = {
            .width = (uint32_t)width, .height = (uint32_t)height,
            .pixelFormat = gfx::PixelFormat::Depth32Float,
            .usages = gfx::TextureUsage::depthStencilAttachment,
            .storageMode = gfx::ResourceStorageMode::deviceLocal
        };
        for (auto& frameData : m_frameDatas)
            frameData.depthTexture = m_device->newTexture(depthTextureDescriptor);
        m_device->waitIdle();
    }

    if (cfd.lastCommandBuffer != nullptr) {
        m_device->waitCommandBuffer(*cfd.lastCommandBuffer);
        TracyGFXCollect(*m_device, m_tracyGraphicsContext);
        cfd.lastCommandBuffer = nullptr;
        cfd.commandBufferPool->reset();
        cfd.parameterBlockPool->reset();
    }


    cfd.renderables.clear();
    cfsd = shader::SceneData{
        .cameraPosition = cameraPosition,
        .ambientLightColor = glm::vec3(0),
        .exposure = 1.0f,
        .directionalLightCount = 0,
        .directionalLights = {},
        .pointLightCount = 0,
        .pointLights = {}
    };

    int width = 0, height = 0;
    ::glfwGetFramebufferSize(m_window, &width, &height);
    const float aspectRatio = static_cast<float>(width) / static_cast<float>(height == 0 ? 1 : height);
    glm::mat4 projectionMatrix = glm::perspective(glm::radians(fov), aspectRatio, near, far);
    *cfd.vpMatrix->content<glm::mat4x4>() = projectionMatrix * viewMatrix;
    glm::mat4x4 skyboxView = viewMatrix;
    skyboxView[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    cfd.skyboxPushConstants.viewProjection = projectionMatrix * skyboxView;

#if !defined (SCOP_MANDATORY)
    {
        ZoneScopedN("imguiNewFrame");
        gfx::imgui::newFrame(*m_device);
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }
#endif
}

void Renderer::addMesh(const Mesh& mesh, const glm::mat4x4& worlTransform)
{
    ZoneScoped;
    std::function<void(const SubMesh&, glm::mat4x4)> addSubmesh = [&](const SubMesh& submesh, const glm::mat4x4& transform) {
        glm::mat4x4 modelMatrix = transform * submesh.transform;

        for (auto& childSubmesh : submesh.subMeshes)
            addSubmesh(childSubmesh, modelMatrix);

        cfd.renderables[submesh.material->graphicsPipleine()][submesh.material][std::make_pair(submesh.vertexBuffer, submesh.indexBuffer)].push_back(modelMatrix);
    };

    for (auto& submesh : mesh.subMeshes)
        addSubmesh(submesh, worlTransform);
}

void Renderer::addPointLight(const glm::vec3& position, const glm::vec3& color)
{
    ZoneScoped;
    if (static_cast<size_t>(cfsd.pointLightCount) >= sizeof(cfsd.pointLights) / sizeof(cfsd.pointLights[0]))
        return;
    cfsd.pointLights[cfsd.pointLightCount++] = shader::PointLight{ // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
        .position = position,
        .color = color
    };
}

void Renderer::addDirectionalLight(const glm::vec3& direction, const glm::vec3& color)
{
    ZoneScoped;
    if (static_cast<size_t>(cfsd.directionalLightCount) >= sizeof(cfsd.directionalLights) / sizeof(cfsd.directionalLights[0]))
        return;
    cfsd.directionalLights[cfsd.directionalLightCount++] = shader::DirectionalLight{ // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
        .position = direction,
        .color = color
    };
}

void Renderer::endFrame()
{
    ZoneScoped;
    #if !defined (SCOP_MANDATORY)
    ImGui::Render();
    #endif

    std::shared_ptr<gfx::CommandBuffer> commandBuffer = cfd.commandBufferPool->get();

    std::shared_ptr<gfx::Drawable> drawable = m_swapchain->nextDrawable();
    if (drawable == nullptr) {
        #if !defined (SCOP_MANDATORY)
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        #endif
        m_swapchain = nullptr;
        return;
    }

    auto renderPassDescriptor = m_device->newRenderPassDescriptor();
    renderPassDescriptor->setColorAttachments({
        gfx::RenderPassDescriptor::Attachment{
            .loadAction = gfx::LoadAction::clear,
            .clearValue = gfx::ClearValue::color({0.0f, 0.0f, 0.0f, 0.0f}),
            .texture = drawable->texture()
        }
    });
    renderPassDescriptor->setDepthAttachment(gfx::RenderPassDescriptor::Attachment{
        .loadAction = gfx::LoadAction::clear,
        .clearValue = gfx::ClearValue::depth(1.0f),
        .texture = cfd.depthTexture
    });

    {
        TracyGFXZone(m_tracyGraphicsContext, *renderPassDescriptor, "main pass");
        commandBuffer->beginRenderPass(*renderPassDescriptor);

        ZoneScopedN("renderPass");
        std::shared_ptr<gfx::ParameterBlock> vpMatrixPBlock = cfd.parameterBlockPool->get(vpMatrixBpLayout());
        vpMatrixPBlock->setBinding(0, cfd.vpMatrix);

        std::shared_ptr<gfx::ParameterBlock> sceneDataPBlock = cfd.parameterBlockPool->get(sceneDataBpLayout());
        sceneDataPBlock->setBinding(0, cfd.sceneDataBuffer);

        for (auto& [pipeline, renderables] : cfd.renderables)
        {
            commandBuffer->usePipeline(pipeline);
            commandBuffer->setParameterBlock(vpMatrixPBlock, 0);
            commandBuffer->setParameterBlock(sceneDataPBlock, 1);

            for (auto& [material, buffers] : renderables)
            {
                commandBuffer->setParameterBlock(material->getParameterBlock(), 2);
                for (auto& [vtxIdxBuffer, modelMatrices] : buffers)
                {
                    auto& [vertexBuffer, indexBuffer] = vtxIdxBuffer;
                    commandBuffer->useVertexBuffer(vertexBuffer);
                    for (auto& modelMatrix : modelMatrices)
                    {
                        commandBuffer->setPushConstants(&modelMatrix);
                        commandBuffer->drawIndexedVertices(indexBuffer);
                    }
                }
            }
        }

        std::shared_ptr<gfx::ParameterBlock> skyboxPBlock = cfd.parameterBlockPool->get(m_skyboxPbLayout);
        skyboxPBlock->setBinding(0, m_environmentCubemap);
        skyboxPBlock->setBinding(1, m_skyboxSampler);

        cfd.skyboxPushConstants.exposure = cfsd.exposure;
        commandBuffer->usePipeline(m_skyboxPipeline);
        commandBuffer->setPushConstants(&cfd.skyboxPushConstants);
        commandBuffer->setParameterBlock(skyboxPBlock, 0);
        commandBuffer->drawVertices(0, 36);

        #if !defined (SCOP_MANDATORY)
        gfx::imgui::renderDrawData(*commandBuffer, ImGui::GetDrawData());
        #endif
        commandBuffer->endRenderPass();
    }

    commandBuffer->presentDrawable(drawable);

    cfd.lastCommandBuffer = commandBuffer.get();
    m_device->submitCommandBuffers(commandBuffer);

#if !defined (SCOP_MANDATORY)
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
#endif

    m_frameIdx = (m_frameIdx + 1) % maxFrameInFlight;
}

Renderer::~Renderer()
{
    m_device->waitIdle();

    TracyGFXCollect(*m_device, m_tracyGraphicsContext);
    TracyGFXDestroy(*m_device, m_tracyGraphicsContext);

#if !defined (SCOP_MANDATORY)
    gfx::imgui::shutdown(*m_device);
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
#endif
}

} // namespace scop
