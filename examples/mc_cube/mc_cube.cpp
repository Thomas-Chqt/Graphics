/*
 * ---------------------------------------------------
 * mc_cube.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/07/25 09:17:49
 * ---------------------------------------------------
 */

#include "Graphics/Buffer.hpp"
#include "Graphics/CommandBuffer.hpp"
#include "Graphics/Drawable.hpp"
#include "Graphics/Framebuffer.hpp"
#include "Graphics/GraphicsPipeline.hpp"
#include "Graphics/Instance.hpp"
#include "Graphics/Device.hpp"
#include "Graphics/ParameterBlock.hpp"
#include "Graphics/ShaderLib.hpp"
#include "Graphics/Surface.hpp"
#include "Graphics/Enums.hpp"
#include "Graphics/Swapchain.hpp"
#include "Graphics/VertexLayout.hpp"
#include "glm/fwd.hpp"

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <memory>
    #include <cassert>
    #include <set>
    #include <cstdint>
    #include <cstddef>
    #include <array>
    #include <numbers>
    namespace ext = std;
#endif

#if __XCODE__
    #include <unistd.h>
#endif

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

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

struct Vertex
{
    glm::vec3 pos;
};

const ext::array<Vertex, 24> cube_vertices = {
    // Front face (+Z)
    Vertex{ glm::vec3(-0.5f, -0.5f,  0.5f) },
    Vertex{ glm::vec3( 0.5f, -0.5f,  0.5f) },
    Vertex{ glm::vec3( 0.5f,  0.5f,  0.5f) },
    Vertex{ glm::vec3(-0.5f,  0.5f,  0.5f) },
    // Back face (-Z)
    Vertex{ glm::vec3( 0.5f, -0.5f, -0.5f) },
    Vertex{ glm::vec3(-0.5f, -0.5f, -0.5f) },
    Vertex{ glm::vec3(-0.5f,  0.5f, -0.5f) },
    Vertex{ glm::vec3( 0.5f,  0.5f, -0.5f) },
    // Left face (-X)
    Vertex{ glm::vec3(-0.5f, -0.5f, -0.5f) },
    Vertex{ glm::vec3(-0.5f, -0.5f,  0.5f) },
    Vertex{ glm::vec3(-0.5f,  0.5f,  0.5f) },
    Vertex{ glm::vec3(-0.5f,  0.5f, -0.5f) },
    // Right face (+X)
    Vertex{ glm::vec3( 0.5f, -0.5f,  0.5f) },
    Vertex{ glm::vec3( 0.5f, -0.5f, -0.5f) },
    Vertex{ glm::vec3( 0.5f,  0.5f, -0.5f) },
    Vertex{ glm::vec3( 0.5f,  0.5f,  0.5f) },
    // Top face (+Y)
    Vertex{ glm::vec3(-0.5f,  0.5f,  0.5f) },
    Vertex{ glm::vec3( 0.5f,  0.5f,  0.5f) },
    Vertex{ glm::vec3( 0.5f,  0.5f, -0.5f) },
    Vertex{ glm::vec3(-0.5f,  0.5f, -0.5f) },
    // Bottom face (-Y)
    Vertex{ glm::vec3(-0.5f, -0.5f, -0.5f) },
    Vertex{ glm::vec3( 0.5f, -0.5f, -0.5f) },
    Vertex{ glm::vec3( 0.5f, -0.5f,  0.5f) },
    Vertex{ glm::vec3(-0.5f, -0.5f,  0.5f) }
};

const ext::array<uint32_t, 36> cube_indices = {
    // Front face
    0, 1, 2, 0, 2, 3,
    // Back face
    4, 5, 6, 4, 6, 7,
    // Left face
    8, 9,10, 8,10,11,
    // Right face
   12,13,14,12,14,15,
    // Top face
   16,17,18,16,18,19,
    // Bottom face
   20,21,22,20,22,23
};

const gfx::ParameterBlock::Layout vpMatrixBpLayout = {
    .bindings = {
        gfx::ParameterBlock::Binding{ .type = gfx::BindingType::uniformBuffer, .usages = gfx::BindingUsage::vertexRead }
    }
};

const gfx::ParameterBlock::Layout modelMatrixBpLayout = {
    .bindings = {
        gfx::ParameterBlock::Binding{ .type = gfx::BindingType::uniformBuffer, .usages = gfx::BindingUsage::vertexRead }
    }
};

const gfx::ParameterBlock::Layout materialBpLayout = {
    .bindings = {
        gfx::ParameterBlock::Binding{ .type = gfx::BindingType::uniformBuffer, .usages = gfx::BindingUsage::fragmentRead }
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
            },
            .maxFrameInFlight = 3
        };
        m_device = m_instance->newDevice(deviceDescriptor);
        assert(m_device);

        assert(m_surface->supportedPixelFormats(*m_device).contains(gfx::PixelFormat::BGRA8Unorm));
        assert(m_surface->supportedPresentModes(*m_device).contains(gfx::PresentMode::fifo));

        ext::unique_ptr<gfx::ShaderLib> shaderLib = m_device->newShaderLib(SHADER_SLIB);
        assert(shaderLib);

        gfx::GraphicsPipeline::Descriptor gfxPipelineDescriptor = {
            .vertexLayout = gfx::VertexLayout{
                .stride = sizeof(Vertex),
                .attributes = {
                    gfx::VertexAttribute{
                        .format = gfx::VertexAttributeFormat::float3,
                        .offset = offsetof(Vertex, pos)
                    }
                }
            },
            .vertexShader = &shaderLib->getFunction("vertexMain"),
            .fragmentShader = &shaderLib->getFunction("fragmentMain"),
            .colorAttachmentPxFormats = { gfx::PixelFormat::BGRA8Unorm },
            .parameterBlockLayouts = { vpMatrixBpLayout, modelMatrixBpLayout, materialBpLayout }
        };
        m_graphicsPipeline = m_device->newGraphicsPipeline(gfxPipelineDescriptor);
        assert(m_graphicsPipeline);

        m_vertexBuffer = m_device->newBuffer(gfx::Buffer::Descriptor{
            .size = sizeof(cube_vertices), .data = cube_vertices.data(), .usage = gfx::BufferUsage::vertexBuffer
        });
        assert(m_vertexBuffer);

        m_indexBuffer = m_device->newBuffer(gfx::Buffer::Descriptor{
            .size = sizeof(cube_indices), .data = cube_indices.data(), .usage = gfx::BufferUsage::indexBuffer
        });
        assert(m_indexBuffer);

        m_vpMatrix = m_device->newBuffer(gfx::Buffer::Descriptor{.size=sizeof(glm::mat4x4), .usage=gfx::BufferUsage::uniformBuffer });
        assert(m_vpMatrix);

        m_modelMatrix = m_device->newBuffer(gfx::Buffer::Descriptor{.size=sizeof(glm::mat4x4), .usage=gfx::BufferUsage::uniformBuffer | gfx::BufferUsage::perFrameData});
        assert(m_modelMatrix);

        m_color = m_device->newBuffer(gfx::Buffer::Descriptor{.size=sizeof(glm::vec4), .usage=gfx::BufferUsage::uniformBuffer | gfx::BufferUsage::perFrameData});
        assert(m_color);

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

                constexpr glm::vec3 camPos = glm::vec3(0.0f, 0.0f,  3.0f);
                constexpr glm::vec3 camDir = glm::vec3(0.0f, 0.0f, -1.0f);
                constexpr glm::vec3 camUp  = glm::vec3(0.0f, 1.0f,  0.0f);
                const glm::mat4 viewMatrix = lookAt(camPos, camPos + camDir, camUp);
                glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 10.0f);
                *m_vpMatrix->content<glm::mat4x4>() = projectionMatrix * viewMatrix;
            }

            m_device->beginFrame();
            {
                m_device->imguiNewFrame();
                ImGui_ImplGlfw_NewFrame();
                ImGui::NewFrame();
                {
                    ImGui::Begin("mc_cube");
                    {
                        *m_modelMatrix->content<glm::mat4x4>() = glm::mat4(1.0f);

                        ImGui::DragFloat3("position", (float*)&m_cubePos, 0.01f, -5.0, 5.0);
                        *m_modelMatrix->content<glm::mat4x4>() = glm::translate(*m_modelMatrix->content<glm::mat4x4>(), m_cubePos);

                        ImGui::DragFloat3("rotation", (float*)&m_cubeRot, 0.01f, -ext::numbers::pi_v<float>, ext::numbers::pi_v<float>);
                        *m_modelMatrix->content<glm::mat4x4>() = glm::rotate(*m_modelMatrix->content<glm::mat4x4>(), m_cubeRot.x, glm::vec3(1, 0, 0));
                        *m_modelMatrix->content<glm::mat4x4>() = glm::rotate(*m_modelMatrix->content<glm::mat4x4>(), m_cubeRot.y, glm::vec3(0, 1, 0));
                        *m_modelMatrix->content<glm::mat4x4>() = glm::rotate(*m_modelMatrix->content<glm::mat4x4>(), m_cubeRot.z, glm::vec3(0, 0, 1));

                        ImGui::DragFloat3("scale", (float*)&m_cubeSca, 0.01f, -2, 2);
                        *m_modelMatrix->content<glm::mat4x4>() = glm::scale(*m_modelMatrix->content<glm::mat4x4>(), m_cubeSca);

                        ImGui::Spacing();

                        static glm::vec3 color = { 1, 1, 1 };
                        ImGui::ColorEdit3("color", (float*)&color);
                        *m_color->content<glm::vec3>() = color;
                    }
                    ImGui::End();
                }
                ImGui::Render();

                gfx::ParameterBlock& vpMatrixPbEncoder = m_device->parameterBlock(vpMatrixBpLayout);
                vpMatrixPbEncoder.setBinding(0, m_vpMatrix);

                gfx::ParameterBlock& modelMatrixPbEncoder = m_device->parameterBlock(modelMatrixBpLayout);
                modelMatrixPbEncoder.setBinding(0, m_modelMatrix);

                gfx::ParameterBlock& materialPbEncoder = m_device->parameterBlock(materialBpLayout);
                materialPbEncoder.setBinding(0, m_color);

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
                    commandBuffer.usePipeline(m_graphicsPipeline);
                    commandBuffer.setParameterBlock(vpMatrixPbEncoder, 0);
                    commandBuffer.setParameterBlock(modelMatrixPbEncoder, 1);
                    commandBuffer.setParameterBlock(materialPbEncoder, 2);
                    commandBuffer.useVertexBuffer(m_vertexBuffer);
                    commandBuffer.drawIndexedVertices(m_indexBuffer);

                    commandBuffer.imGuiRenderDrawData(ImGui::GetDrawData());
                }
                commandBuffer.endRenderPass();

                m_device->submitCommandBuffer(commandBuffer);
                m_device->presentDrawable(drawable);

                if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
                {
                    ImGui::UpdatePlatformWindows();
                    ImGui::RenderPlatformWindowsDefault();
                }
            }
            m_device->endFrame();
        }
    }

    void clean()
    {
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
    ext::shared_ptr<gfx::GraphicsPipeline> m_graphicsPipeline;
    ext::unique_ptr<gfx::Swapchain> m_swapchain;

    ext::shared_ptr<gfx::Buffer> m_vertexBuffer;
    ext::shared_ptr<gfx::Buffer> m_indexBuffer;
    ext::shared_ptr<gfx::Buffer> m_vpMatrix;

    glm::vec3 m_cubePos = {0, 0, 0};
    glm::vec3 m_cubeRot = {0, 0, 0};
    glm::vec3 m_cubeSca = {1, 1, 1};
    ext::shared_ptr<gfx::Buffer> m_modelMatrix;

    ext::shared_ptr<gfx::Buffer> m_color;
};

int main()
{
    Application app;
    app.init();
    app.loop();
    app.clean();
}
