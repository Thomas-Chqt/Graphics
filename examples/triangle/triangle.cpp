/*
 * ---------------------------------------------------
 * triangle.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/06 16:59:24
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
#include "Graphics/VertexLayout.hpp"

#include <GLFW/glfw3.h>

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <memory>
    #include <cassert>
    #include <set>
    #include <cstdint>
    namespace ext = std;
#endif

#if __XCODE__
    #include <unistd.h>
#endif


#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

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
        
        ext::unique_ptr<gfx::ShaderLib> shaderLib = m_device->newShaderLib(SHADER_SLIB);
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

        float vertices[] = {
            /*.pos=*/ 0.0f,  0.5f, /*.color=*/1.0f, 0.0f, 0.0f, 0.0f,
            /*.pos=*/ 0.5f, -0.5f, /*.color=*/0.0f, 1.0f, 0.0f, 0.0f,
            /*.pos=*/-0.5f, -0.5f, /*.color=*/0.0f, 0.0f, 1.0f, 0.0f 
        };

        m_vertexBuffer = m_device->newBuffer(gfx::Buffer::Descriptor{
            .size = sizeof(vertices), .data = vertices, .usages = gfx::BufferUsage::vertexBuffer
        });
        assert(m_vertexBuffer);
    }

    void loop()
    {
        while (1)
        {
            glfwPollEvents();
            if (glfwWindowShouldClose(m_window))
                break;

            if (m_swapchain == nullptr) {
                int width, height;
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

            m_device->beginFrame();
            {
                gfx::CommandBuffer& commandBuffer = m_device->commandBuffer();

                /*
                 * pre passes
                 */

                ext::shared_ptr<gfx::Drawable> drawable = m_swapchain->nextDrawable();
                if (drawable == nullptr) {
                    m_swapchain = nullptr;
                    continue;
                }

                gfx::Framebuffer framebuffer = {
                    .colorAttachments = {
                        gfx::Framebuffer::Attachment{
                            .loadAction = gfx::LoadAction::clear,
                            .clearColor = { 0.0f, 0.0f, 0.0f, 0.0f },
                            .texture = drawable->texture()
                        } 
                    }
                };

                commandBuffer.beginRenderPass(framebuffer);
                {
                    commandBuffer.usePipeline(m_graphicsPipeline);
                    commandBuffer.useVertexBuffer(m_vertexBuffer);
                    commandBuffer.drawVertices(0, 3);
                }
                commandBuffer.endRenderPass();

                m_device->submitCommandBuffer(commandBuffer);
                m_device->presentDrawable(drawable);
            }
            m_device->endFrame();
        }
    }

    void clean()
    {
        glfwDestroyWindow(m_window);
        glfwTerminate();
    }

private:
    GLFWwindow* m_window;
    ext::unique_ptr<gfx::Instance> m_instance;
    ext::unique_ptr<gfx::Surface> m_surface;
    ext::unique_ptr<gfx::Device> m_device;
    ext::shared_ptr<gfx::GraphicsPipeline> m_graphicsPipeline;
    ext::unique_ptr<gfx::Swapchain> m_swapchain;
    ext::shared_ptr<gfx::Buffer> m_vertexBuffer;
};

int main()
{
    Application app;
    app.init();
    app.loop();
    app.clean();
}
