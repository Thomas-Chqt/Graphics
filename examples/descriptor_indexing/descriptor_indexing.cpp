/*
 * ---------------------------------------------------
 * descriptor_indexing.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * ---------------------------------------------------
 */

#include "Graphics/Buffer.hpp"
#include "Graphics/CommandBuffer.hpp"
#include "Graphics/Device.hpp"
#include "Graphics/Drawable.hpp"
#include "Graphics/Enums.hpp"
#include "Graphics/Framebuffer.hpp"
#include "Graphics/GraphicsPipeline.hpp"
#include "Graphics/Instance.hpp"
#include "Graphics/ParameterBlock.hpp"
#include "Graphics/ParameterBlockLayout.hpp"
#include "Graphics/ParameterBlockPool.hpp"
#include "Graphics/Sampler.hpp"
#include "Graphics/ShaderLib.hpp"
#include "Graphics/Surface.hpp"
#include "Graphics/Swapchain.hpp"
#include "Graphics/Texture.hpp"

#include <GLFW/glfw3.h>
#include <cstddef>
#include <glm/glm.hpp>

#include <algorithm>
#include <array>
#include <atomic>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <memory>
#include <mutex>
#include <numbers>
#include <random>
#include <set>
#include <thread>
#include <vector>

#if __XCODE__
    #include <unistd.h>
#endif

constexpr uint32_t WINDOW_WIDTH = 800;
constexpr uint32_t WINDOW_HEIGHT = 600;
constexpr uint8_t maxFrameInFlight = 3;
constexpr uint32_t maxTextures = 4096;
constexpr float initialSpriteSize = 0.25f;
constexpr float spriteShrinkPerFrame = 0.0025f;

using namespace std::chrono_literals;

struct Vertex
{
    glm::vec2 pos;
};

struct PushConstants
{
    glm::vec2 pos;
    float rotation;
    float size;
    uint32_t textureIdx;
    uint32_t _padding;
};

struct Sprite
{
    glm::vec2 pos;
    float rotation;
    float size = initialSpriteSize;
    uint32_t textureIdx = 0;
};

constexpr auto sprite_vertices = std::to_array<Vertex>({
    Vertex{ .pos = { -0.5f,  0.5f } },
    Vertex{ .pos = {  0.5f,  0.5f } },
    Vertex{ .pos = {  0.5f, -0.5f } },
    Vertex{ .pos = { -0.5f,  0.5f } },
    Vertex{ .pos = {  0.5f, -0.5f } },
    Vertex{ .pos = { -0.5f, -0.5f } },
});

class Application
{
public:
    void init()
    {
        #if __XCODE__
        sleep(1); // XCODE BUG https://github.com/glfw/glfw/issues/2634
        #endif

        auto res = glfwInit();
        assert(res == GLFW_TRUE);
        (void)res;

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        m_window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Descriptor indexing", nullptr, nullptr);
        assert(m_window);

        glfwSetWindowUserPointer(m_window, this);
        glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int, int){
            static_cast<Application*>(glfwGetWindowUserPointer(window))->m_swapchain = nullptr;
        });

        m_instance = gfx::Instance::newInstance(gfx::Instance::Descriptor{});
        assert(m_instance);

        m_surface = m_instance->createSurface(m_window);
        assert(m_surface);

        gfx::Device::Descriptor deviceDescriptor = {
            .queueCaps = {
                .graphics = true,
                .compute = true,
                .transfer = true,
                .present = { m_surface.get() }
            }
        };
        m_device = m_instance->newDevice(deviceDescriptor);
        assert(m_device);

        assert(m_surface->supportedPixelFormats(*m_device).contains(gfx::PixelFormat::BGRA8Unorm));
        assert(m_surface->supportedPresentModes(*m_device).contains(gfx::PresentMode::fifo));

        std::unique_ptr<gfx::ShaderLib> shaderLib = m_device->newShaderLib(SHADER_SLIB);
        assert(shaderLib);

        m_textureArrayPBLayout = m_device->newParameterBlockLayout(gfx::ParameterBlockLayout::Descriptor{
            .bindings = {
                gfx::ParameterBlockBinding{ .type = gfx::BindingType::sampler,        .usages = gfx::BindingUsage::fragmentRead },
                gfx::ParameterBlockBinding{ .type = gfx::BindingType::sampledTexture, .usages = gfx::BindingUsage::fragmentRead, .count = maxTextures },
            }
        });

        gfx::GraphicsPipeline::Descriptor pipelineDesc = {
            .vertexLayout = gfx::VertexLayout{
                .stride = sizeof(Vertex),
                .attributes = {
                    gfx::VertexAttribute{ .format = gfx::VertexAttributeFormat::float2, .offset = offsetof(Vertex, pos) },
                }
            },
            .vertexShader = &shaderLib->getFunction("vertexMain"),
            .fragmentShader = &shaderLib->getFunction("fragmentMain"),
            .colorAttachmentPxFormats = { gfx::PixelFormat::BGRA8Unorm },
            .parameterBlockLayouts = { m_textureArrayPBLayout },
        };
        m_graphicsPipeline = m_device->newGraphicsPipeline(pipelineDesc);
        assert(m_graphicsPipeline);

        m_vertexBuffer = m_device->newBuffer(gfx::Buffer::Descriptor{
            .size = sizeof(Vertex) * sprite_vertices.size(),
            .usages = gfx::BufferUsage::vertexBuffer | gfx::BufferUsage::copyDestination,
            .storageMode = gfx::ResourceStorageMode::deviceLocal
        });
        assert(m_vertexBuffer);

        std::shared_ptr<gfx::Buffer> stagingBuffer = m_device->newBuffer(gfx::Buffer::Descriptor{
            .size = m_vertexBuffer->size(),
            .usages = gfx::BufferUsage::copySource,
            .storageMode = gfx::ResourceStorageMode::hostVisible
        });
        assert(stagingBuffer);

        std::ranges::copy(sprite_vertices, stagingBuffer->content<Vertex>());

        std::shared_ptr<gfx::CommandBuffer> commandBuffer = m_device->newCommandBufferPool()->get();
        commandBuffer->beginBlitPass();
        commandBuffer->copyBufferToBuffer(stagingBuffer, m_vertexBuffer, m_vertexBuffer->size());
        commandBuffer->endBlitPass();
        m_device->submitCommandBuffers(commandBuffer);
        m_device->waitCommandBuffer(*commandBuffer);

        for (uint8_t i = 0; i < maxFrameInFlight; ++i)
            m_commandBufferPools.at(i) = m_device->newCommandBufferPool();

        m_textureStreamerThread = std::thread([this]()
        {
            std::mt19937 rng(std::random_device{}());
            std::uniform_int_distribution<int> channelDistribution(32, 255);

            std::shared_ptr<gfx::ParameterBlock> parameterBlock = m_device->newParameterBlockPool(gfx::ParameterBlockPool::Descriptor{
                .maxBindingCount = {
                    { gfx::BindingType::sampler, 1 },
                    { gfx::BindingType::sampledTexture, maxTextures },
                },
                .updateAfterBind = true
            })->get(m_textureArrayPBLayout);
            parameterBlock->setBinding(0, std::shared_ptr<gfx::Sampler>(m_device->newSampler(gfx::Sampler::Descriptor{})));

            std::atomic_store(&m_textureArrayPBlock, parameterBlock);

            std::unique_ptr<gfx::CommandBufferPool> cmdBufferPool = m_device->newCommandBufferPool();
            uint32_t textureIndex = 0;

            while (m_textureStreamerThreadContinue.load())
            {
                if (textureIndex >= maxTextures)
                    break;

                std::shared_ptr<gfx::Texture> texture = m_device->newTexture(gfx::Texture::Descriptor{
                    .type = gfx::TextureType::texture2d,
                    .width = 2,
                    .height = 2,
                    .pixelFormat = gfx::PixelFormat::RGBA8Unorm,
                    .usages = gfx::TextureUsage::copyDestination | gfx::TextureUsage::shaderRead,
                    .storageMode = gfx::ResourceStorageMode::deviceLocal
                });
                assert(texture);

                std::shared_ptr<gfx::Buffer> stagingBuffer = m_device->newBuffer(gfx::Buffer::Descriptor{
                    .size = static_cast<size_t>(4 * 4),
                    .usages = gfx::BufferUsage::copySource,
                    .storageMode = gfx::ResourceStorageMode::hostVisible
                });
                assert(stagingBuffer);

                const std::array<uint8_t, 4> rgba = {
                    static_cast<uint8_t>(channelDistribution(rng)),
                    static_cast<uint8_t>(channelDistribution(rng)),
                    static_cast<uint8_t>(channelDistribution(rng)),
                    255u
                };
                auto* bytes = stagingBuffer->content<uint8_t>();
                for (uint32_t texel = 0; texel < 4; ++texel)
                    std::memcpy(bytes + texel * rgba.size(), rgba.data(), rgba.size());

                std::shared_ptr<gfx::CommandBuffer> uploadCommandBuffer = cmdBufferPool->get();
                uploadCommandBuffer->beginBlitPass();
                uploadCommandBuffer->copyBufferToTexture(stagingBuffer, texture);
                uploadCommandBuffer->endBlitPass();
                m_device->submitCommandBuffers(uploadCommandBuffer);
                m_device->waitCommandBuffer(*uploadCommandBuffer);
                cmdBufferPool->reset();

                parameterBlock->setBinding(1, textureIndex, texture);
                {
                    std::lock_guard guard(m_availableTextureIndicesMtx);
                    m_availableTextureIndices.insert(textureIndex);
                }

                ++textureIndex;

                std::this_thread::sleep_for(100ms);
            }
        });
    }

    void loop()
    {
        while (true)
        {
            glfwPollEvents();
            if (glfwWindowShouldClose(m_window))
                break;

            if (m_swapchain == nullptr) {
                int width = 0;
                int height = 0;
                glfwGetFramebufferSize(m_window, &width, &height);
                m_swapchain = m_device->newSwapchain(gfx::Swapchain::Descriptor{
                    .surface = m_surface.get(),
                    .width = static_cast<uint32_t>(width),
                    .height = static_cast<uint32_t>(height),
                    .imageCount = 3,
                    .pixelFormat = gfx::PixelFormat::BGRA8Unorm,
                    .presentMode = gfx::PresentMode::fifo,
                });
                assert(m_swapchain);
                m_device->waitIdle();
            }

            if (m_lastCommandBuffers.at(m_frameIdx) != nullptr) {
                m_device->waitCommandBuffer(*m_lastCommandBuffers.at(m_frameIdx));
                m_lastCommandBuffers.at(m_frameIdx) = nullptr;
                m_commandBufferPools.at(m_frameIdx)->reset();
            }

            for (auto& sprite : m_sprites)
                sprite.size = std::max(0.0f, sprite.size - spriteShrinkPerFrame);
            std::erase_if(m_sprites, [](const auto& sprite) { return sprite.size <= 0.0f; });

            {
                std::lock_guard guard(m_availableTextureIndicesMtx);
                if (m_availableTextureIndices.empty() == false) {
                    int width = 0;
                    int height = 0;
                    glfwGetWindowSize(m_window, &width, &height);
                    if (width > 0 && height > 0) {
                        double mouseX = 0.0;
                        double mouseY = 0.0;
                        glfwGetCursorPos(m_window, &mouseX, &mouseY);

                        std::uniform_int_distribution<size_t> textureIndexDistribution(0, m_availableTextureIndices.size() - 1);
                        auto textureIndexIt = m_availableTextureIndices.begin();
                        std::advance(textureIndexIt, static_cast<std::ptrdiff_t>(textureIndexDistribution(m_rng)));
                        const glm::vec2 clipPos = {
                            static_cast<float>((mouseX / static_cast<double>(width)) * 2.0 - 1.0),
                            static_cast<float>(1.0 - (mouseY / static_cast<double>(height)) * 2.0)
                        };
                        std::uniform_real_distribution<float> rotationDistribution(0.0f, 2.0f * std::numbers::pi_v<float>);
                        m_sprites.push_back(Sprite{
                            .pos = clipPos,
                            .rotation = rotationDistribution(m_rng),
                            .size = initialSpriteSize,
                            .textureIdx = *textureIndexIt
                        });
                    }
                }
            }

            std::shared_ptr<gfx::Drawable> drawable = m_swapchain->nextDrawable();
            if (drawable == nullptr) {
                m_swapchain = nullptr;
                continue;
            }

            std::shared_ptr<gfx::CommandBuffer> commandBuffer = m_commandBufferPools.at(m_frameIdx)->get();

            gfx::Framebuffer framebuffer = {
                .colorAttachments = {
                    gfx::Framebuffer::Attachment{
                        .loadAction = gfx::LoadAction::clear,
                        .clearColor = { 0.08f, 0.08f, 0.10f, 1.0f },
                        .texture = drawable->texture()
                    }
                }
            };

            commandBuffer->beginRenderPass(framebuffer);
            {
                commandBuffer->usePipeline(m_graphicsPipeline);
                commandBuffer->useVertexBuffer(m_vertexBuffer);
                const auto textureArrayPBlock = std::atomic_load(&m_textureArrayPBlock);
                if (textureArrayPBlock != nullptr) {
                    commandBuffer->setParameterBlock(textureArrayPBlock, 0);

                    for (const auto& sprite : m_sprites) {
                        const PushConstants pushConstants{
                            .pos        = sprite.pos,
                            .rotation   = sprite.rotation,
                            .size       = sprite.size,
                            .textureIdx = sprite.textureIdx
                        };
                        commandBuffer->setPushConstants(&pushConstants);
                        commandBuffer->drawVertices(0, static_cast<uint32_t>(sprite_vertices.size()));
                    }
                }
            }
            commandBuffer->endRenderPass();

            commandBuffer->presentDrawable(drawable);

            m_device->submitCommandBuffers(commandBuffer);

            m_lastCommandBuffers.at(m_frameIdx) = commandBuffer.get();
            m_frameIdx = (m_frameIdx + 1) % maxFrameInFlight;
        }

        m_textureStreamerThreadContinue.store(false);
    }

    void clean()
    {
        if (m_textureStreamerThread.joinable())
            m_textureStreamerThread.join();
        glfwDestroyWindow(m_window);
        glfwTerminate();
    }

private:
    GLFWwindow* m_window = nullptr;

    std::unique_ptr<gfx::Instance> m_instance;
    std::unique_ptr<gfx::Surface> m_surface;
    std::unique_ptr<gfx::Device> m_device;
    std::unique_ptr<gfx::Swapchain> m_swapchain;

    std::shared_ptr<gfx::ParameterBlockLayout> m_textureArrayPBLayout;
    std::shared_ptr<gfx::GraphicsPipeline> m_graphicsPipeline;
    std::shared_ptr<gfx::ParameterBlock> m_textureArrayPBlock;

    std::shared_ptr<gfx::Buffer> m_vertexBuffer;

    std::thread m_textureStreamerThread;
    std::atomic<bool> m_textureStreamerThreadContinue = true;
    std::set<uint32_t> m_availableTextureIndices;
    std::mutex m_availableTextureIndicesMtx;

    std::vector<Sprite> m_sprites;
    std::mt19937 m_rng = std::mt19937(std::random_device{}());

    uint8_t m_frameIdx = 0;
    std::array<std::unique_ptr<gfx::CommandBufferPool>, maxFrameInFlight> m_commandBufferPools;
    std::array<gfx::CommandBuffer*, maxFrameInFlight> m_lastCommandBuffers{};
};

int main()
{
    try
    {
        Application app;
        app.init();
        app.loop();
        app.clean();
    }
    catch (...)
    {
        return -1;
    }
    return 0;
}
