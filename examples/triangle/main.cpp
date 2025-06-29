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

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    // glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "GLFW Window", nullptr, nullptr);

    ext::unique_ptr<gfx::Instance> instance = gfx::Instance::newInstance(gfx::Instance::Descriptor{});
    assert(instance);

    ext::unique_ptr<gfx::Surface> surface = instance->createSurface(window);
    assert(surface);

    gfx::Device::Descriptor deviceDescriptor = {
        .queueCaps = {
            .graphics = true,
            .compute = true,
            .transfer = true,
            .present = { surface.get() }
        }
    };
    ext::unique_ptr<gfx::Device> device = instance->newDevice(deviceDescriptor);
    assert(device);

    assert(surface->supportedPixelFormats(*device).contains(gfx::PixelFormat::BGRA8Unorm));
    assert(surface->supportedPresentModes(*device).contains(gfx::PresentMode::fifo));

    int width, height;
    ::glfwGetFramebufferSize(window, &width, &height);
    gfx::Swapchain::Descriptor swapchainDescriptor = {
        .surface = surface.get(),
        .width = (uint32_t)width, .height = (uint32_t)height,
        .imageCount = 3,
        .pixelFormat = gfx::PixelFormat::BGRA8Unorm,
        .presentMode = gfx::PresentMode::fifo,
    };
    ext::unique_ptr<gfx::Swapchain> swapchain = device->newSwapchain(swapchainDescriptor);

    ext::unique_ptr<gfx::ShaderLib> shaderLib = device->newShaderLib(SHADER_SLIB);

    gfx::GraphicsPipeline::Descriptor gfxPipelineDescriptor = {
        .vertexShader = &shaderLib->getFunction("vertexMain"),
        .fragmentShader = &shaderLib->getFunction("fragmentMain"),
        .colorAttachmentPxFormats = { gfx::PixelFormat::BGRA8Unorm },
    };
    ext::shared_ptr<gfx::GraphicsPipeline> graphicsPipeline = device->newGraphicsPipeline(gfxPipelineDescriptor);

    while (glfwWindowShouldClose(window) == false)
    {
        device->beginFrame();
        
        ext::unique_ptr<gfx::CommandBuffer> commandBuffer = device->commandBuffer();

        ext::shared_ptr<gfx::Drawable> drawable = swapchain->nextDrawable();
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
        
        commandBuffer->usePipeline(graphicsPipeline);

        commandBuffer->endRenderPass();

        device->submitCommandBuffer(std::move(commandBuffer));
        device->presentDrawable(drawable);
        
        device->endFrame();

        glfwPollEvents();
    }

    device->waitIdle();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
