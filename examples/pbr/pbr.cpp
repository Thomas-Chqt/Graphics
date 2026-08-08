#include "Graphics/Buffer.hpp"
#include "Graphics/CommandBuffer.hpp"
#include "Graphics/ComputePipeline.hpp"
#include "Graphics/Device.hpp"
#include "Graphics/Drawable.hpp"
#include "Graphics/Enums.hpp"
#include "Graphics/GraphicsPipeline.hpp"
#include "Graphics/Instance.hpp"
#include "Graphics/ParameterBlock.hpp"
#include "Graphics/ParameterBlockPool.hpp"
#include "Graphics/PassDescriptor.hpp"
#include "Graphics/Sampler.hpp"
#include "Graphics/ShaderLib.hpp"
#include "Graphics/Surface.hpp"
#include "Graphics/Swapchain.hpp"
#include "Graphics/Texture.hpp"
#include "Graphics/VertexLayout.hpp"

#include "shaders/pbr.slang"
#include "shaders/skybox.slang"

#include <GLFW/glfw3.h>
#include <backends/imgui_impl_glfw.h>
#include <gfx_glfw/gfx_glfw.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <imgui.h>
#include <gfx_imgui/gfx_imgui.hpp>

#include <stb_image/stb_image.h>

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <format>
#include <memory>
#include <numbers>
#include <stdexcept>
#include <vector>
#include <bit>

#if __XCODE__
    #include <unistd.h>
#endif

constexpr uint32_t windowWidth = 800;
constexpr uint32_t windowHeight = 600;
constexpr uint8_t maxFrameInFlight = 3;

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
};

struct SphereMesh
{
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
};

namespace
{

SphereMesh generateSphere(uint32_t stacks, uint32_t slices)
{
    assert(stacks >= 2);
    assert(slices >= 3);

    SphereMesh mesh;
    mesh.vertices.reserve(static_cast<size_t>(stacks + 1) * (slices + 1));
    mesh.indices.reserve(static_cast<size_t>(stacks) * slices * 6);

    for (uint32_t stack = 0; stack <= stacks; ++stack)
    {
        const float theta = std::numbers::pi_v<float> * static_cast<float>(stack) / static_cast<float>(stacks);
        const float y = std::cos(theta);
        const float ringRadius = std::sin(theta);

        for (uint32_t slice = 0; slice <= slices; ++slice)
        {
            const float phi = 2.0f * std::numbers::pi_v<float> * static_cast<float>(slice) / static_cast<float>(slices);
            const glm::vec3 position = {
                ringRadius * std::cos(phi),
                y,
                ringRadius * std::sin(phi)
            };
            mesh.vertices.push_back(Vertex{ .position = position, .normal = position });
        }
    }

    const uint32_t verticesPerRing = slices + 1;
    for (uint32_t stack = 0; stack < stacks; ++stack)
    {
        for (uint32_t slice = 0; slice < slices; ++slice)
        {
            const uint32_t current = stack * verticesPerRing + slice;
            const uint32_t next = current + verticesPerRing;
            mesh.indices.insert(mesh.indices.end(), {
                current, current + 1, next,
                current + 1, next + 1, next
            });
        }
    }

    return mesh;
}

std::shared_ptr<gfx::Texture> createEnvironmentCubemap(gfx::Device& device, gfx::ShaderLib& pbrPrecomputeSlib)
{
    constexpr const char* environmentPath = RESOURCE_DIR "/environment.hdr";

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
    const size_t uploadSize =
        static_cast<size_t>(width) *
        static_cast<size_t>(height) *
        gfx::pixelFormatSize(gfx::PixelFormat::RGBA32_float);

    std::shared_ptr<gfx::Buffer> uploadBuffer = device.newBuffer({
        .size = uploadSize,
        .usages = gfx::BufferUsage::copySource,
        .storageMode = gfx::ResourceStorageMode::hostVisible
    });
    assert(uploadBuffer);
    uploadBuffer->setContent(pixels.get(), uploadSize);

    std::shared_ptr<gfx::Texture> equirectangularTexture = device.newTexture({
        .type = gfx::TextureType::texture2d,
        .width = static_cast<uint32_t>(width),
        .height = static_cast<uint32_t>(height),
        .mipLevelCount = 1,
        .arrayLayerCount = 1,
        .pixelFormat = gfx::PixelFormat::RGBA32_float,
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
        .pixelFormat = gfx::PixelFormat::RGBA32_float,
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
    {
        commandBuffer->usePipeline(conversionPipeline);
        commandBuffer->setParameterBlock(parameters, 0);
        commandBuffer->dispatchThreadgroups((faceSize + 7) / 8, (faceSize + 7) / 8, 6);
    }
    commandBuffer->endComputePass();

    std::unique_ptr<gfx::BlitPassDescriptor> blitPassDescriptor = device.newBlitPassDescriptor();
    commandBuffer->beginBlitPass(*blitPassDescriptor);
    {
        commandBuffer->generateMipmaps(cubemap);
    }
    commandBuffer->endBlitPass();

    device.submitCommandBuffers(commandBuffer);

    return cubemap;
}

std::shared_ptr<gfx::Texture> createIrradianceCubemap(gfx::Device& device, gfx::ShaderLib& pbrPrecomputeSlib, const std::shared_ptr<gfx::Texture>& environmentCubemap)
{
    assert(environmentCubemap);

    constexpr uint32_t irradianceFaceSize = 32;

    std::shared_ptr<gfx::Texture> irradianceCubemap = device.newTexture({
        .type = gfx::TextureType::textureCube,
        .width = irradianceFaceSize,
        .height = irradianceFaceSize,
        .mipLevelCount = 1,
        .arrayLayerCount = 1,
        .pixelFormat = gfx::PixelFormat::RGBA32_float,
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

std::shared_ptr<gfx::Texture> createPrefilteredEnvironmentCubemap(gfx::Device& device, gfx::ShaderLib& pbrPrecomputeSlib, const std::shared_ptr<gfx::Texture>& environmentCubemap)
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
        .pixelFormat = gfx::PixelFormat::RGBA32_float,
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

    std::shared_ptr<gfx::ParameterBlockLayout> ressourcePBlockLayout = device.newParameterBlockLayout({
        .bindings = {
            { .type = gfx::BindingType::sampledTexture, .usages = gfx::BindingUsage::computeRead },
            { .type = gfx::BindingType::sampler, .usages = gfx::BindingUsage::computeRead },
            { .type = gfx::BindingType::storageTexture, .usages = gfx::BindingUsage::computeWrite }
        }
    });
    assert(ressourcePBlockLayout);

    std::shared_ptr<gfx::ParameterBlockLayout> roughnessPBlockLayout = device.newParameterBlockLayout({
        .bindings = {
            { .type = gfx::BindingType::constantBuffer, .usages = gfx::BindingUsage::computeRead },
        }
    });
    assert(roughnessPBlockLayout);

    std::shared_ptr<gfx::ComputePipeline> pipeline = device.newComputePipeline({
        .computeShader = &pbrPrecomputeSlib.getFunction("prefilterEnvironment"),
        .threadsPerThreadgroupX = 8,
        .threadsPerThreadgroupY = 8,
        .threadsPerThreadgroupZ = 1,
        .parameterBlockLayouts = { ressourcePBlockLayout, roughnessPBlockLayout }
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

        std::shared_ptr<gfx::Buffer> roughnessBuffer = device.newBuffer(gfx::Buffer::Descriptor{
            .size = sizeof(float),
            .usages = gfx::BufferUsage::constantBuffer,
            .storageMode = gfx::ResourceStorageMode::hostVisible
        });
        *roughnessBuffer->content<float>() = static_cast<float>(mipLevel) / static_cast<float>(mipLevelCount - 1);

        std::shared_ptr<gfx::ParameterBlock> ressourcePBlock = parameterBlockPool->get(ressourcePBlockLayout);
        ressourcePBlock->setBinding(0, environmentCubemap);
        ressourcePBlock->setBinding(1, sampler);
        ressourcePBlock->setBinding(2, writableFaces);

        std::shared_ptr<gfx::ParameterBlock> roughnessPBlock = parameterBlockPool->get(roughnessPBlockLayout);
        roughnessPBlock->setBinding(0, roughnessBuffer);

        commandBuffer->setParameterBlock(ressourcePBlock, 0);
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
        .pixelFormat = gfx::PixelFormat::RG32Float,
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

class Application
{
public:
    void init()
    {
        #if __XCODE__
        sleep(1); // XCODE BUG https://github.com/glfw/glfw/issues/2634
        #endif

        [[maybe_unused]] const auto glfwResult = glfwInit();
        assert(glfwResult == GLFW_TRUE);

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        m_window = glfwCreateWindow(windowWidth, windowHeight, "PBR", nullptr, nullptr);
        assert(m_window);

        glfwSetWindowUserPointer(m_window, this);
        glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int, int) {
            static_cast<Application*>(glfwGetWindowUserPointer(window))->m_swapchain = nullptr;
        });

        m_instance = gfx::Instance::newInstance(gfx::Instance::Descriptor{
            .instanceExtension = gfx::glfw::getInstanceExtension()
        });
        assert(m_instance);

        m_surface = gfx::glfw::createSurface(*m_instance, m_window);
        assert(m_surface);

        m_device = m_instance->newDevice(gfx::Device::Descriptor{
            .queueCaps = {
                .graphics = true,
                .compute = true,
                .transfer = true,
                .present = { m_surface.get() }
            }
        });
        assert(m_device);

        assert(std::ranges::contains(m_surface->supportedSurfaceFormat(*m_device), gfx::PixelFormat::BGRA8_sRGB, &gfx::SurfaceFormat::pixelFormat));
        assert(m_surface->supportedPresentModes(*m_device).contains(gfx::PresentMode::fifo));

        std::unique_ptr<gfx::ShaderLib> environmentPrecomputeSlib = m_device->newShaderLib(SHADER_DIR"/equirectangularToCubemap.slib");
        assert(environmentPrecomputeSlib);

        std::unique_ptr<gfx::ShaderLib> irradiancePrecomputeSlib = m_device->newShaderLib(SHADER_DIR"/convolveDiffuseIrradiance.slib");
        assert(irradiancePrecomputeSlib);

        std::unique_ptr<gfx::ShaderLib> prefilterPrecomputeSlib = m_device->newShaderLib(SHADER_DIR"/prefilterEnvironment.slib");
        assert(prefilterPrecomputeSlib);

        std::unique_ptr<gfx::ShaderLib> brdfPrecomputeSlib = m_device->newShaderLib(SHADER_DIR"/integrateBrdf.slib");
        assert(brdfPrecomputeSlib);

        std::unique_ptr<gfx::ShaderLib> pbrSlib = m_device->newShaderLib(SHADER_DIR"/pbr.slib");
        assert(pbrSlib);

        std::unique_ptr<gfx::ShaderLib> skyboxSlib = m_device->newShaderLib(SHADER_DIR"/skybox.slib");
        assert(skyboxSlib);

        m_environmentCubemap = createEnvironmentCubemap(*m_device, *environmentPrecomputeSlib);
        assert(m_environmentCubemap);

        m_irradianceCubemap = createIrradianceCubemap(*m_device, *irradiancePrecomputeSlib, m_environmentCubemap);
        assert(m_irradianceCubemap);

        m_prefilteredEnvironmentCubemap = createPrefilteredEnvironmentCubemap(*m_device, *prefilterPrecomputeSlib, m_environmentCubemap);
        assert(m_prefilteredEnvironmentCubemap);

        m_brdfLut = createBrdfLut(*m_device, *brdfPrecomputeSlib);
        assert(m_brdfLut);

        m_uniformPbLayout = m_device->newParameterBlockLayout(gfx::ParameterBlockLayout::Descriptor{
            .bindings = {
                gfx::ParameterBlockBinding{ .type = gfx::BindingType::constantBuffer, .usages = gfx::BindingUsage::vertexRead | gfx::BindingUsage::fragmentRead }
            }
        });
        assert(m_uniformPbLayout);

        m_texturesPbLayout = m_device->newParameterBlockLayout(gfx::ParameterBlockLayout::Descriptor{
            .bindings = {
                gfx::ParameterBlockBinding{ .type = gfx::BindingType::sampledTexture, .usages = gfx::BindingUsage::fragmentRead },
                gfx::ParameterBlockBinding{ .type = gfx::BindingType::sampledTexture, .usages = gfx::BindingUsage::fragmentRead },
                gfx::ParameterBlockBinding{ .type = gfx::BindingType::sampledTexture, .usages = gfx::BindingUsage::fragmentRead },
                gfx::ParameterBlockBinding{ .type = gfx::BindingType::sampler, .usages = gfx::BindingUsage::fragmentRead }
            }
        });
        assert(m_texturesPbLayout);

        m_graphicsPipeline = m_device->newGraphicsPipeline(gfx::GraphicsPipeline::Descriptor{
            .vertexLayout = gfx::VertexLayout{
                .stride = sizeof(Vertex),
                .attributes = {
                    gfx::VertexAttribute{ .format = gfx::VertexAttributeFormat::float3, .offset = offsetof(Vertex, position) },
                    gfx::VertexAttribute{ .format = gfx::VertexAttributeFormat::float3, .offset = offsetof(Vertex, normal) }
                }
            },
            .vertexShader = &pbrSlib->getFunction("vertexMain"),
            .fragmentShader = &pbrSlib->getFunction("fragmentMain"),
            .colorAttachmentPxFormats = { gfx::PixelFormat::BGRA8_sRGB },
            .depthAttachmentPxFormat = gfx::PixelFormat::Depth32Float,
            .cullMode = gfx::CullMode::back,
            .parameterBlockLayouts = { m_uniformPbLayout, m_texturesPbLayout }
        });
        assert(m_graphicsPipeline);

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

        m_sampler = m_device->newSampler({
            .sAddressMode = gfx::SamplerAddressMode::ClampToEdge,
            .tAddressMode = gfx::SamplerAddressMode::ClampToEdge,
            .rAddressMode = gfx::SamplerAddressMode::ClampToEdge,
            .minFilter = gfx::SamplerMinMagFilter::Linear,
            .magFilter = gfx::SamplerMinMagFilter::Linear,
            .mipFilter = gfx::SamplerMipFilter::Linear
        });
        assert(m_sampler);

        for (uint8_t i = 0; i < maxFrameInFlight; ++i)
        {
            m_commandBufferPools.at(i) = m_device->newCommandBufferPool();
            m_parameterBlockPools.at(i) = m_device->newParameterBlockPool(gfx::ParameterBlockPool::Descriptor{
                .maxBindingCount = {
                    { gfx::BindingType::constantBuffer, 1 },
                    { gfx::BindingType::sampledTexture, 4 },
                    { gfx::BindingType::sampler, 1 }
                }
            });
        }

        const SphereMesh sphere = generateSphere(32, 64);
        m_vertexBuffer = m_device->newBuffer(gfx::Buffer::Descriptor{
            .size = sizeof(Vertex) * sphere.vertices.size(),
            .usages = gfx::BufferUsage::vertexBuffer | gfx::BufferUsage::copyDestination,
            .storageMode = gfx::ResourceStorageMode::deviceLocal
        });
        assert(m_vertexBuffer);

        m_indexBuffer = m_device->newBuffer(gfx::Buffer::Descriptor{
            .size = sizeof(uint32_t) * sphere.indices.size(),
            .usages = gfx::BufferUsage::indexBuffer | gfx::BufferUsage::copyDestination,
            .storageMode = gfx::ResourceStorageMode::deviceLocal
        });
        assert(m_indexBuffer);

        std::shared_ptr<gfx::Buffer> vertexStagingBuffer = m_device->newBuffer(gfx::Buffer::Descriptor{
            .size = m_vertexBuffer->size(),
            .usages = gfx::BufferUsage::copySource,
            .storageMode = gfx::ResourceStorageMode::hostVisible
        });
        assert(vertexStagingBuffer);
        std::ranges::copy(sphere.vertices, vertexStagingBuffer->content<Vertex>());

        std::shared_ptr<gfx::Buffer> indexStagingBuffer = m_device->newBuffer(gfx::Buffer::Descriptor{
            .size = m_indexBuffer->size(),
            .usages = gfx::BufferUsage::copySource,
            .storageMode = gfx::ResourceStorageMode::hostVisible
        });
        assert(indexStagingBuffer);
        std::ranges::copy(sphere.indices, indexStagingBuffer->content<uint32_t>());

        std::shared_ptr<gfx::CommandBuffer> commandBuffer = m_commandBufferPools.at(m_frameIndex)->get();
        auto blitPassDescriptor = m_device->newBlitPassDescriptor();
        commandBuffer->beginBlitPass(*blitPassDescriptor);
        commandBuffer->copyBufferToBuffer(vertexStagingBuffer, m_vertexBuffer, m_vertexBuffer->size());
        commandBuffer->copyBufferToBuffer(indexStagingBuffer, m_indexBuffer, m_indexBuffer->size());
        commandBuffer->endBlitPass();
        m_device->submitCommandBuffers(commandBuffer);

        for (uint8_t i = 0; i < maxFrameInFlight; ++i)
        {
            m_uniforms.at(i) = m_device->newBuffer(gfx::Buffer::Descriptor{
                .size = sizeof(shader::pbr::Uniforms),
                .usages = gfx::BufferUsage::constantBuffer,
                .storageMode = gfx::ResourceStorageMode::hostVisible
            });
            assert(m_uniforms.at(i));
        }

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

        gfx::imgui::init(*m_device, {
            .colorAttachmentPixelFormats = { gfx::PixelFormat::BGRA8_sRGB },
            .depthAttachmentPixelFormat = gfx::PixelFormat::Depth32Float
        });
    }

    void loop()
    {
        while (true)
        {
            ::glfwPollEvents();
            if (::glfwWindowShouldClose(m_window))
                break;

            if (m_swapchain == nullptr) {
                int width = 0, height = 0;
                ::glfwGetFramebufferSize(m_window, &width, &height);
                m_swapchain = m_device->newSwapchain(gfx::Swapchain::Descriptor{
                    .surface = m_surface.get(),
                    .width = static_cast<uint32_t>(width),
                    .height = static_cast<uint32_t>(height),
                    .imageCount = 3,
                    .drawableCount = maxFrameInFlight,
                    .pixelFormat = gfx::PixelFormat::BGRA8_sRGB,
                    .presentMode = gfx::PresentMode::fifo
                });
                assert(m_swapchain);

                const gfx::Texture::Descriptor depthTextureDescriptor = {
                    .width = static_cast<uint32_t>(width),
                    .height = static_cast<uint32_t>(height),
                    .pixelFormat = gfx::PixelFormat::Depth32Float,
                    .usages = gfx::TextureUsage::depthStencilAttachment,
                    .storageMode = gfx::ResourceStorageMode::deviceLocal
                };
                for (auto& depthTexture : m_depthTextures)
                    depthTexture = m_device->newTexture(depthTextureDescriptor);
                m_device->waitIdle();
            }

            if (m_lastCommandBuffers.at(m_frameIndex) != nullptr) {
                m_device->waitCommandBuffer(*m_lastCommandBuffers.at(m_frameIndex));
                m_lastCommandBuffers.at(m_frameIndex).reset();
                m_commandBufferPools.at(m_frameIndex)->reset();
                m_parameterBlockPools.at(m_frameIndex)->reset();
            }

            int width = 0, height = 0;
            ::glfwGetFramebufferSize(m_window, &width, &height);
            constexpr glm::vec3 camPos = glm::vec3(0.0f, 0.0f,  3.0f);
            constexpr glm::vec3 camDir = glm::vec3(0.0f, 0.0f, -1.0f);
            constexpr glm::vec3 camUp  = glm::vec3(0.0f, 1.0f,  0.0f);
            const glm::mat4 viewMatrix = glm::lookAt(camPos, camPos + camDir, camUp);
            glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 10.0f);
            m_uniforms.at(m_frameIndex)->content<shader::pbr::Uniforms>()->vpMatrix = projectionMatrix * viewMatrix;
            m_uniforms.at(m_frameIndex)->content<shader::pbr::Uniforms>()->cameraPosition = camPos;
            m_uniforms.at(m_frameIndex)->content<shader::pbr::Uniforms>()->lightPosition = {2.0f, 2.0f, 3.0f};

            gfx::imgui::newFrame(*m_device);
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            ImGui::Begin("PBR material");

            ImGui::ColorEdit3("Base color", &m_albedo.x);
            m_uniforms.at(m_frameIndex)->content<shader::pbr::Uniforms>()->albedo = m_albedo;

            ImGui::DragFloat("Metallic", &m_metallic, 0.01f, 0.0f, 1.0f);
            m_uniforms.at(m_frameIndex)->content<shader::pbr::Uniforms>()->metallic = m_metallic;

            ImGui::DragFloat("Roughness", &m_roughness, 0.01f, 0.01f, 1.0f);
            m_uniforms.at(m_frameIndex)->content<shader::pbr::Uniforms>()->roughness = m_roughness;

            ImGui::ColorEdit3("Light color", &m_lightColor.x);
            ImGui::DragFloat("Light intensity", &m_lightIntensity, 0.05f, 0.0f, 100.0f);
            m_uniforms.at(m_frameIndex)->content<shader::pbr::Uniforms>()->lightColor = m_lightColor * m_lightIntensity;

            ImGui::DragFloat("Exposure", &m_exposure, 0.01f, 0.05f, 16.0f);
            m_uniforms.at(m_frameIndex)->content<shader::pbr::Uniforms>()->exposure = m_exposure;

            ImGui::End();
            ImGui::Render();

            const shader::skybox::PushConstants skyboxPushConstants{
                .viewProjection = projectionMatrix * glm::mat4(glm::mat3(viewMatrix)),
                .exposure = m_exposure
            };

            glm::mat4 model(1.0f);
            model = glm::translate(model, {0.0f, 0.0f, 0.0f});
            model = glm::rotate(model, glm::vec3{0.0f, 0.0f, 0.0f}.x, glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::rotate(model, glm::vec3{0.0f, 0.0f, 0.0f}.y, glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::rotate(model, glm::vec3{0.0f, 0.0f, 0.0f}.z, glm::vec3(0.0f, 0.0f, 1.0f));
            model = glm::scale(model, glm::vec3{1.0f, 1.0f, 1.0f});
            m_uniforms.at(m_frameIndex)->content<shader::pbr::Uniforms>()->modelMatrix = model;

            std::shared_ptr<gfx::ParameterBlock> uniformsPBlock = m_parameterBlockPools.at(m_frameIndex)->get(m_uniformPbLayout);
            uniformsPBlock->setBinding(0, m_uniforms.at(m_frameIndex));

            std::shared_ptr<gfx::ParameterBlock> texturesPBlock = m_parameterBlockPools.at(m_frameIndex)->get(m_texturesPbLayout);
            texturesPBlock->setBinding(0, m_irradianceCubemap);
            texturesPBlock->setBinding(1, m_prefilteredEnvironmentCubemap);
            texturesPBlock->setBinding(2, m_brdfLut);
            texturesPBlock->setBinding(3, m_sampler);

            std::shared_ptr<gfx::ParameterBlock> skyboxPBlock = m_parameterBlockPools.at(m_frameIndex)->get(m_skyboxPbLayout);
            skyboxPBlock->setBinding(0, m_environmentCubemap);
            skyboxPBlock->setBinding(1, m_sampler);

            std::shared_ptr<gfx::Drawable> drawable = m_swapchain->nextDrawable();
            if (drawable == nullptr)
            {
                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault();
                m_swapchain = nullptr;
                continue;
            }

            auto renderPassDescriptor = m_device->newRenderPassDescriptor();
            renderPassDescriptor->setColorAttachments({
                gfx::RenderPassDescriptor::Attachment{
                    .loadAction = gfx::LoadAction::clear,
                    .clearValue = gfx::ClearValue::color({ 0.0f, 0.0f, 0.0f, 1.0f }),
                    .texture = drawable->texture()
                }
            });
            renderPassDescriptor->setDepthAttachment(gfx::RenderPassDescriptor::Attachment{
                .loadAction = gfx::LoadAction::clear,
                .clearValue = gfx::ClearValue::depth(1.0f),
                .texture = m_depthTextures.at(m_frameIndex)
            });

            std::shared_ptr<gfx::CommandBuffer> commandBuffer = m_commandBufferPools.at(m_frameIndex)->get();
            commandBuffer->beginRenderPass(*renderPassDescriptor);
            {
                commandBuffer->usePipeline(m_graphicsPipeline);
                commandBuffer->setParameterBlock(uniformsPBlock, 0);
                commandBuffer->setParameterBlock(texturesPBlock, 1);
                commandBuffer->useVertexBuffer(m_vertexBuffer);
                commandBuffer->drawIndexedVertices(m_indexBuffer);

                commandBuffer->usePipeline(m_skyboxPipeline);
                commandBuffer->setPushConstants(&skyboxPushConstants);
                commandBuffer->setParameterBlock(skyboxPBlock, 0);
                commandBuffer->drawVertices(0, 36);

                gfx::imgui::renderDrawData(*commandBuffer, ImGui::GetDrawData());
            }
            commandBuffer->endRenderPass();
            commandBuffer->presentDrawable(drawable);

            m_lastCommandBuffers.at(m_frameIndex) = commandBuffer;
            m_device->submitCommandBuffers(commandBuffer);

            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            m_frameIndex = (m_frameIndex + 1) % maxFrameInFlight;
        }
    }

    void clean()
    {
        m_device->waitIdle();
        gfx::imgui::shutdown(*m_device);
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        glfwDestroyWindow(m_window);
        glfwTerminate();
    }

private:
    GLFWwindow* m_window = nullptr;
    std::unique_ptr<gfx::Instance> m_instance;
    std::unique_ptr<gfx::Surface> m_surface;
    std::unique_ptr<gfx::Device> m_device;
    std::unique_ptr<gfx::Swapchain> m_swapchain;

    std::shared_ptr<gfx::ParameterBlockLayout> m_uniformPbLayout;
    std::shared_ptr<gfx::ParameterBlockLayout> m_texturesPbLayout;
    std::shared_ptr<gfx::ParameterBlockLayout> m_skyboxPbLayout;
    std::shared_ptr<gfx::GraphicsPipeline> m_graphicsPipeline;
    std::shared_ptr<gfx::GraphicsPipeline> m_skyboxPipeline;
    std::shared_ptr<gfx::Texture> m_environmentCubemap;
    std::shared_ptr<gfx::Texture> m_irradianceCubemap;
    std::shared_ptr<gfx::Texture> m_prefilteredEnvironmentCubemap;
    std::shared_ptr<gfx::Texture> m_brdfLut;
    std::shared_ptr<gfx::Sampler> m_sampler;

    std::shared_ptr<gfx::Buffer> m_vertexBuffer;
    std::shared_ptr<gfx::Buffer> m_indexBuffer;
    std::array<std::shared_ptr<gfx::Texture>, maxFrameInFlight> m_depthTextures;

    std::array<std::unique_ptr<gfx::CommandBufferPool>, maxFrameInFlight> m_commandBufferPools;
    std::array<std::unique_ptr<gfx::ParameterBlockPool>, maxFrameInFlight> m_parameterBlockPools;

    std::array<std::shared_ptr<gfx::Buffer>, maxFrameInFlight> m_uniforms;

    uint8_t m_frameIndex = 0;
    std::array<std::shared_ptr<gfx::CommandBuffer>, maxFrameInFlight> m_lastCommandBuffers = {};

    glm::vec4 m_albedo = {1.0f, 0.0f, 0.0f, 1.0f};
    float m_metallic = 0.0f;
    float m_roughness = 0.2f;
    glm::vec3 m_lightColor = {1.0f, 1.0f, 1.0f};
    float m_lightIntensity = 50.0f;
    float m_exposure = 1.0f;
};

int main()
{
    Application application;
    application.init();
    application.loop();
    application.clean();
    return 0;
}
