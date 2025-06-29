/*
 * ---------------------------------------------------
 * main.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/06 16:59:24
 * ---------------------------------------------------
 */

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

#include <GLFW/glfw3.h>

#if defined(GFX_USE_UTILSCPP)
    #include "UtilsCPP/memory.hpp"
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
        glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int, int){ static_cast<Application*>(glfwGetWindowUserPointer(window))->createSwapchain(); });

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
        
        createSwapchain();

        ext::unique_ptr<gfx::ShaderLib> shaderLib = m_device->newShaderLib(SHADER_SLIB);
        assert(shaderLib);

        gfx::GraphicsPipeline::Descriptor gfxPipelineDescriptor = {
            .vertexShader = &shaderLib->getFunction("vertexMain"),
            .fragmentShader = &shaderLib->getFunction("fragmentMain"),
            .colorAttachmentPxFormats = { gfx::PixelFormat::BGRA8Unorm },
        };
        m_graphicsPipeline = m_device->newGraphicsPipeline(gfxPipelineDescriptor);
        assert(m_graphicsPipeline);
    }

    void loop()
    {
        while (glfwWindowShouldClose(m_window) == false)
        {
            m_device->beginFrame();

            ext::unique_ptr<gfx::CommandBuffer> commandBuffer = m_device->commandBuffer();
            assert(commandBuffer);

            ext::shared_ptr<gfx::Drawable> drawable = m_swapchain->nextDrawable();
            assert(drawable);

            gfx::Framebuffer framebuffer = {
                .colorAttachments = {
                    gfx::Framebuffer::Attachment{
                        .loadAction = gfx::LoadAction::clear,
                        .clearColor = {0.0f, 0.0f, 0.0f, 0.0f },
                        .texture = drawable->texture()
                    } 
                }
            };

            commandBuffer->beginRenderPass(framebuffer);

            commandBuffer->usePipeline(m_graphicsPipeline);

            commandBuffer->drawVertices(0, 3);

            commandBuffer->endRenderPass();

            m_device->submitCommandBuffer(std::move(commandBuffer));
            m_device->presentDrawable(drawable);

            m_device->endFrame();

            glfwPollEvents();
        }
    }

    void clean()
    {
        m_device->waitIdle();

        glfwDestroyWindow(m_window);
        glfwTerminate();
    }

private:
    GLFWwindow* m_window;
    ext::unique_ptr<gfx::Instance> m_instance;
    ext::unique_ptr<gfx::Surface> m_surface;
    ext::unique_ptr<gfx::Device> m_device;
    ext::unique_ptr<gfx::Swapchain> m_swapchain;
    ext::shared_ptr<gfx::GraphicsPipeline> m_graphicsPipeline;

    void createSwapchain()
    {
        int width, height;
        ::glfwGetFramebufferSize(m_window, &width, &height);
        gfx::Swapchain::Descriptor swapchainDescriptor = {
            .surface = m_surface.get(),
            .width = (uint32_t)width, .height = (uint32_t)height,
            .imageCount = 3,
            .pixelFormat = gfx::PixelFormat::BGRA8Unorm,
            .presentMode = gfx::PresentMode::fifo,
        };
        m_swapchain = m_device->newSwapchain(swapchainDescriptor);
        assert(m_swapchain);
    }
};

int main()
{
    Application app;
    app.init();
    app.loop();
    app.clean();
}
