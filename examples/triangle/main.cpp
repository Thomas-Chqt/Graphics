/*
 * ---------------------------------------------------
 * main.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/06 16:59:24
 * ---------------------------------------------------
 */

#include "Graphics/CommandBuffer.hpp"
#include "Graphics/Framebuffer.hpp"
#include "Graphics/Instance.hpp"
#include "Graphics/Device.hpp"
#include "Graphics/RenderPass.hpp"
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
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "GLFW Window", nullptr, nullptr);

    ext::unique_ptr<gfx::Instance> instance = gfx::Instance::newInstance(gfx::Instance::Descriptor{});
    assert(instance);

    ext::unique_ptr<gfx::Surface> surface = instance->createSurface(window);
    assert(surface);

    ext::unique_ptr<gfx::Device> device = instance->newDevice(gfx::Device::Descriptor::singleQueuePatern(surface.get()));
    assert(device);

    assert(surface->supportedPixelFormats(device->physicalDevice()).contains(gfx::PixelFormat::BGRA8Unorm));
    assert(surface->supportedPresentModes(device->physicalDevice()).contains(gfx::PresentMode::fifo));

    gfx::RenderPass::Descriptor renderPassDescriptor = {
        .colorAttachments = {
            gfx::AttachmentDescriptor{ .format = gfx::PixelFormat::BGRA8Unorm, .loadAction = gfx::LoadAction::clear }
        }
    };
    ext::unique_ptr<gfx::RenderPass> renderPass = device->newRenderPass(renderPassDescriptor);
    assert(renderPass);
    
    int width, height;
    ::glfwGetFramebufferSize(window, &width, &height);
    gfx::Swapchain::Descriptor swapchainDescriptor = {
        .surface = surface.get(),
        .width = (uint32_t)width, .height = (uint32_t)height,
        .renderPass = renderPass.get()
    };
    ext::unique_ptr<gfx::Swapchain> swapchain = device->newSwapchain(swapchainDescriptor);

    glfwPollEvents();

    ext::unique_ptr<gfx::CommandBuffer> commandBuffer = device->newCommandBuffer();
    assert(commandBuffer);

    const gfx::Framebuffer& framebuffer = swapchain->nextFrameBuffer();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
