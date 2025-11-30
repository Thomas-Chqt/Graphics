/*
 * ---------------------------------------------------
 * multiBuffer.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/07/12 13:59:44
 * ---------------------------------------------------
 */

#include "Graphics/Buffer.hpp"
#include "Graphics/CommandBuffer.hpp"
#include "Graphics/Drawable.hpp"
#include "Graphics/Framebuffer.hpp"
#include "Graphics/GraphicsPipeline.hpp"
#include "Graphics/Instance.hpp"
#include "Graphics/Device.hpp"
#include "Graphics/ShaderLib.hpp"
#include "Graphics/Surface.hpp"
#include "Graphics/Enums.hpp"
#include "Graphics/Swapchain.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>

#include <memory>
#include <cassert>
#include <cstdint>
#include <array>

#if __XCODE__
    #include <unistd.h>
#endif

constexpr uint32_t WINDOW_WIDTH = 800;
constexpr uint32_t WINDOW_HEIGHT = 600;

constexpr uint8_t maxFrameInFlight = 3;

struct Vertex
{
    glm::vec2 pos;
    glm::vec4 color;
};

constexpr std::array<std::array<Vertex, 6>, 4> vertices = {
    std::array<Vertex, 6> {
        Vertex{ .pos=glm::vec2(-1.0f, -0.5f), .color=glm::vec4(1.0f, 0.0f, 0.0f, 0.0f) },
        Vertex{ .pos=glm::vec2(-1.0f,  1.0f), .color=glm::vec4(1.0f, 0.0f, 0.0f, 0.0f) },
        Vertex{ .pos=glm::vec2( 0.5f,  1.0f), .color=glm::vec4(1.0f, 0.0f, 0.0f, 0.0f) },

        Vertex{ .pos=glm::vec2(-1.0f, -0.5f), .color=glm::vec4(1.0f, 0.0f, 0.0f, 0.0f) },
        Vertex{ .pos=glm::vec2( 0.5f,  1.0f), .color=glm::vec4(1.0f, 0.0f, 0.0f, 0.0f) },
        Vertex{ .pos=glm::vec2( 0.5f, -0.5f), .color=glm::vec4(1.0f, 0.0f, 0.0f, 0.0f) },
    },
    std::array<Vertex, 6> {
        Vertex{ .pos=glm::vec2(-0.5f, -0.5f), .color=glm::vec4(0.0f, 1.0f, 0.0f, 0.0f) },
        Vertex{ .pos=glm::vec2(-0.5f,  1.0f), .color=glm::vec4(0.0f, 1.0f, 0.0f, 0.0f) },
        Vertex{ .pos=glm::vec2( 1.0f,  1.0f), .color=glm::vec4(0.0f, 1.0f, 0.0f, 0.0f) },

        Vertex{ .pos=glm::vec2(-0.5f, -0.5f), .color=glm::vec4(0.0f, 1.0f, 0.0f, 0.0f) },
        Vertex{ .pos=glm::vec2( 1.0f,  1.0f), .color=glm::vec4(0.0f, 1.0f, 0.0f, 0.0f) },
        Vertex{ .pos=glm::vec2( 1.0f, -0.5f), .color=glm::vec4(0.0f, 1.0f, 0.0f, 0.0f) },
    },
    std::array<Vertex, 6> {
        Vertex{ .pos=glm::vec2(-0.5f,  0.5f), .color=glm::vec4(0.0f, 0.0f, 1.0f, 0.0f) },
        Vertex{ .pos=glm::vec2( 1.0f,  0.5f), .color=glm::vec4(0.0f, 0.0f, 1.0f, 0.0f) },
        Vertex{ .pos=glm::vec2( 1.0f, -1.0f), .color=glm::vec4(0.0f, 0.0f, 1.0f, 0.0f) },

        Vertex{ .pos=glm::vec2(-0.5f,  0.5f), .color=glm::vec4(0.0f, 0.0f, 1.0f, 0.0f) },
        Vertex{ .pos=glm::vec2( 1.0f, -1.0f), .color=glm::vec4(0.0f, 0.0f, 1.0f, 0.0f) },
        Vertex{ .pos=glm::vec2(-0.5f, -1.0f), .color=glm::vec4(0.0f, 0.0f, 1.0f, 0.0f) },
    },
    std::array<Vertex, 6> {
        Vertex{ .pos=glm::vec2(-1.0f, -1.0f), .color=glm::vec4(1.0f, 0.0f, 0.0f, 0.0f) },
        Vertex{ .pos=glm::vec2(-1.0f,  0.5f), .color=glm::vec4(1.0f, 0.0f, 0.0f, 0.0f) },
        Vertex{ .pos=glm::vec2( 0.5f,  0.5f), .color=glm::vec4(1.0f, 0.0f, 0.0f, 0.0f) },

        Vertex{ .pos=glm::vec2(-1.0f, -1.0f), .color=glm::vec4(1.0f, 0.0f, 0.0f, 0.0f) },
        Vertex{ .pos=glm::vec2( 0.5f,  0.5f), .color=glm::vec4(1.0f, 0.0f, 0.0f, 0.0f) },
        Vertex{ .pos=glm::vec2( 0.5f, -1.0f), .color=glm::vec4(1.0f, 0.0f, 0.0f, 0.0f) }
    }
};

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
        // glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);
        m_window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "GLFW Window", nullptr, nullptr);
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

        gfx::GraphicsPipeline::Descriptor gfxPipelineDescriptor = {
            .vertexLayout = gfx::VertexLayout{
                .stride = sizeof(float) * 6,
                .attributes = {
                    gfx::VertexAttribute{
                        .format = gfx::VertexAttributeFormat::float2,
                        .offset = 0
                    },
                    gfx::VertexAttribute{
                        .format = gfx::VertexAttributeFormat::float3,
                        .offset = sizeof(float) * 2
                    }
                }
            },
            .vertexShader = &shaderLib->getFunction("vertexMain"),
            .fragmentShader = &shaderLib->getFunction("fragmentMain"),
            .colorAttachmentPxFormats = { gfx::PixelFormat::BGRA8Unorm },
        };
        m_graphicsPipeline = m_device->newGraphicsPipeline(gfxPipelineDescriptor);
        assert(m_graphicsPipeline);

        for (uint8_t i = 0; i < maxFrameInFlight; i++) {
            m_commandBufferPools.at(i) = m_device->newCommandBufferPool();
        }

        std::shared_ptr<gfx::Buffer> stagingBuffer = m_device->newBuffer(gfx::Buffer::Descriptor{
            .size = sizeof(Vertex) * 6,
            .usages = gfx::BufferUsage::copySource,
            .storageMode = gfx::ResourceStorageMode::hostVisible
        });
        assert(stagingBuffer);

        for (uint64_t i = 0; i < vertices.size(); i++)
        {
            m_vertexBuffers.at(i) = m_device->newBuffer(gfx::Buffer::Descriptor{
                .size = sizeof(Vertex) * 6,
                .usages = gfx::BufferUsage::vertexBuffer | gfx::BufferUsage::copyDestination,
                .storageMode = gfx::ResourceStorageMode::deviceLocal
            });
            assert(m_vertexBuffers.at(i));

            std::ranges::copy(vertices.at(i), stagingBuffer->content<Vertex>());
            std::shared_ptr<gfx::CommandBuffer> commandBuffer = m_commandBufferPools.at(m_frameIdx)->get();
            commandBuffer->beginBlitPass();
            commandBuffer->copyBufferToBuffer(stagingBuffer, m_vertexBuffers.at(i), stagingBuffer->size());
            commandBuffer->endBlitPass();
            m_device->submitCommandBuffers(commandBuffer);
            m_device->waitCommandBuffer(*commandBuffer);
        }
    }

    void loop()
    {
        while (true)
        {
            glfwPollEvents();
            if (glfwWindowShouldClose(m_window))
                break;

            if (m_swapchain == nullptr) {
                int width = 0, height = 0;
                ::glfwGetFramebufferSize(m_window, &width, &height);
                gfx::Swapchain::Descriptor swapchainDescriptor = {
                    .surface = m_surface.get(),
                    .width = (uint32_t)width,
                    .height = (uint32_t)height,
                    .imageCount = 3,
                    .pixelFormat = gfx::PixelFormat::BGRA8Unorm,
                    .presentMode = gfx::PresentMode::fifo,
                };
                m_swapchain = m_device->newSwapchain(swapchainDescriptor);
                assert(m_swapchain);
                m_device->waitIdle();
            }

            if (m_lastCommandBuffers.at(m_frameIdx) != nullptr) {
                m_device->waitCommandBuffer(*m_lastCommandBuffers.at(m_frameIdx));
                m_lastCommandBuffers.at(m_frameIdx).reset();
                m_commandBufferPools.at(m_frameIdx)->reset();
            }

            std::vector<std::shared_ptr<gfx::CommandBuffer>> commandBuffers(2);
            commandBuffers.at(0) = m_commandBufferPools.at(m_frameIdx)->get();
            commandBuffers.at(1) = m_commandBufferPools.at(m_frameIdx)->get();

            std::shared_ptr<gfx::Drawable> drawable = m_swapchain->nextDrawable();
            if (drawable == nullptr) {
                m_swapchain = nullptr;
                continue;
            }

            gfx::Framebuffer framebuffer = {
                .colorAttachments = {
                    gfx::Framebuffer::Attachment{
                        .loadAction = gfx::LoadAction::clear,
                        .clearColor = {0.0f, 0.0f, 0.0f, 0.0f },
                        .texture = drawable->texture()
                    }
                }
            };

            commandBuffers.at(0)->beginRenderPass(framebuffer);
            {
                commandBuffers.at(0)->usePipeline(m_graphicsPipeline);
                commandBuffers.at(0)->useVertexBuffer(m_vertexBuffers[0]);
                commandBuffers.at(0)->drawVertices(0, 6);
            }
            commandBuffers.at(0)->endRenderPass();

            framebuffer.colorAttachments[0].loadAction = gfx::LoadAction::load;

            commandBuffers.at(0)->beginRenderPass(framebuffer);
            {
                commandBuffers.at(0)->usePipeline(m_graphicsPipeline);
                commandBuffers.at(0)->useVertexBuffer(m_vertexBuffers[1]);
                commandBuffers.at(0)->drawVertices(0, 6);
            }
            commandBuffers.at(0)->endRenderPass();

            commandBuffers.at(1)->beginRenderPass(framebuffer);
            {
                commandBuffers.at(1)->usePipeline(m_graphicsPipeline);
                commandBuffers.at(1)->useVertexBuffer(m_vertexBuffers[2]);
                commandBuffers.at(1)->drawVertices(0, 6);
            }
            commandBuffers.at(1)->endRenderPass();

            commandBuffers.at(1)->beginRenderPass(framebuffer);
            {
                commandBuffers.at(1)->usePipeline(m_graphicsPipeline);
                commandBuffers.at(1)->useVertexBuffer(m_vertexBuffers[3]);
                commandBuffers.at(1)->drawVertices(0, 6);
            }
            commandBuffers.at(1)->endRenderPass();

            commandBuffers.at(1)->presentDrawable(drawable);

            m_lastCommandBuffers.at(m_frameIdx) = commandBuffers.at(1);
            m_device->submitCommandBuffers(commandBuffers);

            m_frameIdx = (m_frameIdx + 1) % maxFrameInFlight;
        }
    }

    void clean()
    {
        glfwDestroyWindow(m_window);
        glfwTerminate();
    }

private:
    GLFWwindow* m_window = nullptr;
    std::unique_ptr<gfx::Instance> m_instance;
    std::unique_ptr<gfx::Surface> m_surface;
    std::unique_ptr<gfx::Device> m_device;
    std::unique_ptr<gfx::Swapchain> m_swapchain;
    std::shared_ptr<gfx::GraphicsPipeline> m_graphicsPipeline;
    std::array<std::shared_ptr<gfx::Buffer>, 4> m_vertexBuffers;

    uint8_t m_frameIdx = 0;
    std::array<std::unique_ptr<gfx::CommandBufferPool>, maxFrameInFlight> m_commandBufferPools;
    std::array<std::shared_ptr<gfx::CommandBuffer>, maxFrameInFlight> m_lastCommandBuffers = {};
};

int main()
{
    Application app;
    app.init();
    app.loop();
    app.clean();
}
