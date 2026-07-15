/*
 * ---------------------------------------------------
 * imgui_usage.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/07/29 07:30:35
 * ---------------------------------------------------
 */

#include "Graphics/CommandBuffer.hpp"
#include "Graphics/Drawable.hpp"
#include "Graphics/PassDescriptor.hpp"
#include "Graphics/Instance.hpp"
#include "Graphics/Device.hpp"
#include "Graphics/Surface.hpp"
#include "Graphics/Enums.hpp"
#include "Graphics/Swapchain.hpp"
#include "Graphics/Texture.hpp"

#include <GLFW/glfw3.h>
#include <gfx_glfw/gfx_glfw.hpp>
#include <imgui.h>
#include <gfx_imgui/gfx_imgui.hpp>
#include <backends/imgui_impl_glfw.h>
#include <stb_image/stb_image.h>

#include <memory>
#include <cassert>
#include <cstdint>
#include <cstring>

#if __XCODE__
    #include <unistd.h>
#endif

constexpr uint32_t WINDOW_WIDTH = 800;
constexpr uint32_t WINDOW_HEIGHT = 600;

constexpr uint8_t maxFrameInFlight = 3;

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
        m_window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "GLFW Window", nullptr, nullptr);
        assert(m_window);

        glfwSetWindowUserPointer(m_window, this);
        glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int, int){
            static_cast<Application*>(glfwGetWindowUserPointer(window))->m_swapchain = nullptr;
        });

        m_instance = gfx::Instance::newInstance(gfx::Instance::Descriptor{
            .instanceExtension = gfx::glfw::getInstanceExtension()
        });
        assert(m_instance);

        m_surface = gfx::glfw::createSurface(*m_instance, m_window);
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

        for (uint8_t i = 0; i < maxFrameInFlight; i++) {
            m_commandBufferPools.at(i) = m_device->newCommandBufferPool();
        }

        int width = 0;
        int height = 0;
        stbi_uc* textureBytes = stbi_load(RESOURCE_DIR"/MyImage01.jpg", &width, &height, nullptr, STBI_rgb_alpha);
        assert(textureBytes);

        m_texture = m_device->newTexture(gfx::Texture::Descriptor{
            .type = gfx::TextureType::texture2d,
            .width = static_cast<uint32_t>(width),
            .height = static_cast<uint32_t>(height),
            .pixelFormat = gfx::PixelFormat::RGBA8Unorm,
            .usages = gfx::TextureUsage::copyDestination | gfx::TextureUsage::shaderRead,
            .storageMode = gfx::ResourceStorageMode::deviceLocal
        });
        assert(m_texture);

        std::shared_ptr<gfx::Buffer> stagingBuffer = m_device->newBuffer(gfx::Buffer::Descriptor{
            .size = static_cast<size_t>(width) * static_cast<size_t>(height) * pixelFormatSize(gfx::PixelFormat::RGBA8Unorm),
            .usages = gfx::BufferUsage::copySource,
            .storageMode = gfx::ResourceStorageMode::hostVisible
        });
        assert(stagingBuffer);

        auto* bufferData = stagingBuffer->content<stbi_uc>();
        std::memcpy(bufferData, textureBytes, stagingBuffer->size());

        stbi_image_free(textureBytes);

        std::shared_ptr<gfx::CommandBuffer> commandBuffer = m_commandBufferPools.at(m_frameIdx)->get();
        auto blitPassDescriptor = m_device->newBlitPassDescriptor();
        commandBuffer->beginBlitPass(*blitPassDescriptor);
        {
            commandBuffer->copyBufferToTexture(stagingBuffer, 0, m_texture, 0);
        }
        commandBuffer->endBlitPass();
        m_device->submitCommandBuffers(commandBuffer);

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

        gfx::imgui::init(*m_device, {.colorAttachmentPixelFormats = {gfx::PixelFormat::BGRA8Unorm}});

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
                m_commandBufferPools.at(m_frameIdx)->reset();
            }

            gfx::imgui::newFrame(*m_device);
            ImGui_ImplGlfw_NewFrame();

            ImGui::NewFrame();
            {
                ImGui::ShowDemoWindow();

                ImGui::Begin("texture");
                if (gfx::imgui::textureId(*m_texture).has_value() == false)
                    gfx::imgui::initTextureId(*m_texture);
                ImGui::Image(*gfx::imgui::textureId(*m_texture), ImVec2((float)m_texture->width(), (float)m_texture->height()));
                ImGui::End();
            }
            ImGui::Render();

            std::shared_ptr<gfx::CommandBuffer> commandBuffer = m_commandBufferPools.at(m_frameIdx)->get();

            std::shared_ptr<gfx::Drawable> drawable = m_swapchain->nextDrawable();
            if (drawable == nullptr) {
                m_swapchain = nullptr;
                continue;
            }

            auto renderPassDescriptor = m_device->newRenderPassDescriptor();
            renderPassDescriptor->setColorAttachments({
                gfx::RenderPassDescriptor::Attachment{
                    .loadAction = gfx::LoadAction::clear,
                    .clearValue = gfx::ClearValue::color({0.0f, 0.0f, 0.0f, 0.0f}),
                    .texture = drawable->texture()
                }
            });

            commandBuffer->beginRenderPass(*renderPassDescriptor);
            {
                commandBuffer->addSampledTexture(m_texture);
                gfx::imgui::renderDrawData(*commandBuffer, ImGui::GetDrawData());
            }
            commandBuffer->endRenderPass();
            commandBuffer->presentDrawable(drawable);

            m_device->submitCommandBuffers(commandBuffer);
            m_lastCommandBuffers.at(m_frameIdx) = commandBuffer.get();

            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();

            m_frameIdx = (m_frameIdx + 1) % maxFrameInFlight;
        }
    }

    void clean()
    {
        m_texture.reset();

        m_device->waitIdle();
        for (auto& pool : m_commandBufferPools)
            pool->reset();
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

    std::shared_ptr<gfx::Texture> m_texture;

    uint8_t m_frameIdx = 0;
    std::array<std::unique_ptr<gfx::CommandBufferPool>, maxFrameInFlight> m_commandBufferPools;
    std::array<gfx::CommandBuffer*, maxFrameInFlight> m_lastCommandBuffers = {};
};

int main()
{
    Application app;
    app.init();
    app.loop();
    app.clean();
}
