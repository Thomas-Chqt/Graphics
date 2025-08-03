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
#include "Graphics/Framebuffer.hpp"
#include "Graphics/Instance.hpp"
#include "Graphics/Device.hpp"
#include "Graphics/Surface.hpp"
#include "Graphics/Enums.hpp"
#include "Graphics/Swapchain.hpp"
#include "imgui.h"

#include <GLFW/glfw3.h>
#include <backends/imgui_impl_glfw.h>

#if (defined(__GNUC__) || defined(__clang__))
    #define GFX_EXPORT __attribute__((visibility("default")))
#elif defined(_MSC_VER)
    #define GFX_EXPORT __declspec(dllexport)
#else
    #error "unknown compiler"
#endif

extern "C"
{

GFX_EXPORT ImGuiContext* GetCurrentContext() { return ImGui::GetCurrentContext(); }
GFX_EXPORT ImGuiIO* GetIO() { return &ImGui::GetIO(); }
GFX_EXPORT ImGuiPlatformIO* GetPlatformIO() { return &ImGui::GetPlatformIO(); }
GFX_EXPORT ImGuiViewport* GetMainViewport() { return ImGui::GetMainViewport(); }
GFX_EXPORT bool DebugCheckVersionAndDataLayout(const char* version_str, size_t sz_io, size_t sz_style, size_t sz_vec2, size_t sz_vec4, size_t sz_drawvert, size_t sz_drawidx) { return ImGui::DebugCheckVersionAndDataLayout(version_str, sz_io, sz_style, sz_vec2, sz_vec4, sz_drawvert, sz_drawidx); }
GFX_EXPORT void* MemAlloc(size_t size) { return ImGui::MemAlloc(size); }
GFX_EXPORT void MemFree(void* ptr) { return ImGui::MemFree(ptr); }
GFX_EXPORT void DestroyPlatformWindows() { return ImGui::DestroyPlatformWindows(); }

}

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
            },
            .maxFrameInFlight = 3
        };
        m_device = m_instance->newDevice(deviceDescriptor);
        assert(m_device);

        assert(m_surface->supportedPixelFormats(*m_device).contains(gfx::PixelFormat::BGRA8Unorm));
        assert(m_surface->supportedPresentModes(*m_device).contains(gfx::PresentMode::fifo));

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

        m_device->imguiInit(3, {gfx::PixelFormat::BGRA8Unorm});
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

            m_device->imguiNewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            static bool showDemoWindow = true;
            ImGui::ShowDemoWindow(&showDemoWindow);
            ImGui::Render();

            m_device->beginFrame();
            {
                gfx::CommandBuffer& commandBuffer = m_device->commandBuffer();

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
                    commandBuffer.imGuiRenderDrawData(ImGui::GetDrawData());
                }
                commandBuffer.endRenderPass();
                
                m_device->submitCommandBuffer(commandBuffer);
                m_device->presentDrawable(drawable);
            }
            m_device->endFrame();

            if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
            {
                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault();
            }
        }
    }

    void clean()
    {
        m_device->waitIdle();
        m_device->imguiShutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        glfwDestroyWindow(m_window);
        glfwTerminate();
    }

private:
    GLFWwindow* m_window;
    ext::unique_ptr<gfx::Instance> m_instance;
    ext::unique_ptr<gfx::Surface> m_surface;
    ext::unique_ptr<gfx::Device> m_device;
    ext::unique_ptr<gfx::Swapchain> m_swapchain;
};

int main()
{
    Application app;
    app.init();
    app.loop();
    app.clean();
}
