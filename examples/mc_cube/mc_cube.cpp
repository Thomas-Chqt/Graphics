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
#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <memory>
    #include <cassert>
    #include <cstdint>
    #include <cstddef>
    #include <array>
    #include <numbers>
    #include <algorithm>
    #include <bit>
    namespace ext = std;
#endif

#if __XCODE__
    #include <unistd.h>
#endif

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

constexpr uint32_t WINDOW_WIDTH = 800;
constexpr uint32_t WINDOW_HEIGHT = 600;

constexpr uint8_t maxFrameInFlight = 3;

struct Vertex
{
    glm::vec3 pos;
};

constexpr ext::array<Vertex, 24> cube_vertices = {
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

constexpr ext::array<uint32_t, 36> cube_indices = {
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
        (void)res;

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
            .depthAttachmentPxFormat = gfx::PixelFormat::Depth32Float,
            .parameterBlockLayouts = { vpMatrixBpLayout, modelMatrixBpLayout, materialBpLayout }
        };
        m_graphicsPipeline = m_device->newGraphicsPipeline(gfxPipelineDescriptor);
        assert(m_graphicsPipeline);

        for (uint8_t i = 0; i < maxFrameInFlight; i++) {
            m_commandBufferPools.at(i) = m_device->newCommandBufferPool();
            m_parameterBlockPools.at(i) = m_device->newParameterBlockPool();
        }

        ext::shared_ptr<gfx::Buffer> stagingBuffer = m_device->newBuffer(gfx::Buffer::Descriptor{
            .size = ext::max(sizeof(Vertex) * cube_vertices.size(), sizeof(uint32_t) * cube_indices.size()),
            .usages = gfx::BufferUsage::copySource,
            .storageMode = gfx::ResourceStorageMode::hostVisible
        });
        assert(stagingBuffer);

        m_vertexBuffer = m_device->newBuffer(gfx::Buffer::Descriptor{
            .size = sizeof(Vertex) * cube_vertices.size(),
            .usages = gfx::BufferUsage::vertexBuffer | gfx::BufferUsage::copyDestination,
            .storageMode = gfx::ResourceStorageMode::deviceLocal
        });
        assert(m_vertexBuffer);

        m_indexBuffer = m_device->newBuffer(gfx::Buffer::Descriptor{
            .size = sizeof(uint32_t) * cube_indices.size(),
            .usages = gfx::BufferUsage::indexBuffer | gfx::BufferUsage::copyDestination,
            .storageMode = gfx::ResourceStorageMode::deviceLocal
        });
        assert(m_indexBuffer);

        ext::ranges::copy(cube_vertices, stagingBuffer->content<Vertex>());
        gfx::CommandBuffer* commandBuffer = m_commandBufferPools.at(m_frameIdx)->get().release();
        commandBuffer->beginBlitPass();
        commandBuffer->copyBufferToBuffer(stagingBuffer, m_vertexBuffer, m_vertexBuffer->size());
        commandBuffer->endBlitPass();
        m_device->submitCommandBuffers(ext::unique_ptr<gfx::CommandBuffer>(commandBuffer));

        m_device->waitCommandBuffer(commandBuffer);

        ext::ranges::copy(cube_indices, stagingBuffer->content<uint32_t>());
        commandBuffer = m_commandBufferPools.at(m_frameIdx)->get().release();
        commandBuffer->beginBlitPass();
        commandBuffer->copyBufferToBuffer(stagingBuffer, m_indexBuffer, m_indexBuffer->size());
        commandBuffer->endBlitPass();
        m_device->submitCommandBuffers(ext::unique_ptr<gfx::CommandBuffer>(commandBuffer));

        for (auto i = 0; i < maxFrameInFlight; i++)
        {
            m_vpMatrix.at(i) = m_device->newBuffer(gfx::Buffer::Descriptor{
                .size=sizeof(glm::mat4x4),
                .usages=gfx::BufferUsage::uniformBuffer,
                .storageMode = gfx::ResourceStorageMode::hostVisible
            });
            assert(m_vpMatrix.at(i));

            m_modelMatrix.at(i) = m_device->newBuffer(gfx::Buffer::Descriptor{
                .size=sizeof(glm::mat4x4),
                .usages=gfx::BufferUsage::uniformBuffer,
                .storageMode = gfx::ResourceStorageMode::hostVisible
            });
            assert(m_modelMatrix.at(i));

            m_color.at(i) = m_device->newBuffer(gfx::Buffer::Descriptor{
                .size=sizeof(glm::vec3),
                .usages=gfx::BufferUsage::uniformBuffer,
                .storageMode = gfx::ResourceStorageMode::hostVisible
            });
            assert(m_color.at(i));
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

        m_device->imguiInit({ gfx::PixelFormat::BGRA8Unorm }, gfx::PixelFormat::Depth32Float);
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

                for (auto i = 0; i < maxFrameInFlight; i++) {
                    gfx::Texture::Descriptor depthTextureDescriptor = {
                        .width = (uint32_t)width, .height = (uint32_t)height,
                        .pixelFormat = gfx::PixelFormat::Depth32Float,
                        .usages = gfx::TextureUsage::depthStencilAttachment,
                        .storageMode = gfx::ResourceStorageMode::deviceLocal
                    };

                    m_depthTexture.at(i) = m_device->newTexture(depthTextureDescriptor);
                }
            }

            if (m_lastCommandBuffers.at(m_frameIdx) != nullptr) {
                m_device->waitCommandBuffer(m_lastCommandBuffers.at(m_frameIdx));
                m_lastCommandBuffers.at(m_frameIdx) = nullptr;
            }

            int width = 0, height = 0;
            ::glfwGetFramebufferSize(m_window, &width, &height);
            constexpr glm::vec3 camPos = glm::vec3(0.0f, 0.0f,  3.0f);
            constexpr glm::vec3 camDir = glm::vec3(0.0f, 0.0f, -1.0f);
            constexpr glm::vec3 camUp  = glm::vec3(0.0f, 1.0f,  0.0f);
            const glm::mat4 viewMatrix = glm::lookAt(camPos, camPos + camDir, camUp);
            glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 10.0f);
            *m_vpMatrix.at(m_frameIdx)->content<glm::mat4x4>() = projectionMatrix * viewMatrix;

            m_device->imguiNewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            {
                ImGui::Begin("mc_cube");
                {
                    *m_modelMatrix.at(m_frameIdx)->content<glm::mat4x4>() = glm::mat4(1.0f);

                    ImGui::DragFloat3("position", ext::bit_cast<float*>(&m_cubePos), 0.01f, -5.0, 5.0);
                    *m_modelMatrix.at(m_frameIdx)->content<glm::mat4x4>() = glm::translate(*m_modelMatrix.at(m_frameIdx)->content<glm::mat4x4>(), m_cubePos);

                    ImGui::DragFloat3("rotation", ext::bit_cast<float*>(&m_cubeRot), 0.01f, -ext::numbers::pi_v<float>, ext::numbers::pi_v<float>);
                    *m_modelMatrix.at(m_frameIdx)->content<glm::mat4x4>() = glm::rotate(*m_modelMatrix.at(m_frameIdx)->content<glm::mat4x4>(), m_cubeRot.x, glm::vec3(1, 0, 0));
                    *m_modelMatrix.at(m_frameIdx)->content<glm::mat4x4>() = glm::rotate(*m_modelMatrix.at(m_frameIdx)->content<glm::mat4x4>(), m_cubeRot.y, glm::vec3(0, 1, 0));
                    *m_modelMatrix.at(m_frameIdx)->content<glm::mat4x4>() = glm::rotate(*m_modelMatrix.at(m_frameIdx)->content<glm::mat4x4>(), m_cubeRot.z, glm::vec3(0, 0, 1));

                    ImGui::DragFloat3("scale", ext::bit_cast<float*>(&m_cubeSca), 0.01f, -2, 2);
                    *m_modelMatrix.at(m_frameIdx)->content<glm::mat4x4>() = glm::scale(*m_modelMatrix.at(m_frameIdx)->content<glm::mat4x4>(), m_cubeSca);

                    ImGui::Spacing();

                    static glm::vec3 color = { 1, 1, 1 };
                    ImGui::ColorEdit3("color", ext::bit_cast<float*>(&color));
                    *m_color.at(m_frameIdx)->content<glm::vec3>() = color;
                }
                ImGui::End();
            }
            ImGui::Render();

            ext::shared_ptr<gfx::ParameterBlock> vpMatrixPBlock = m_parameterBlockPools.at(m_frameIdx)->get(vpMatrixBpLayout);
            vpMatrixPBlock->setBinding(0, m_vpMatrix.at(m_frameIdx));

            ext::shared_ptr<gfx::ParameterBlock> modelMatrixPBlock = m_parameterBlockPools.at(m_frameIdx)->get(modelMatrixBpLayout);
            modelMatrixPBlock->setBinding(0, m_modelMatrix.at(m_frameIdx));

            ext::shared_ptr<gfx::ParameterBlock> materialPBlock = m_parameterBlockPools.at(m_frameIdx)->get(materialBpLayout);
            materialPBlock->setBinding(0, m_color.at(m_frameIdx));

            ext::unique_ptr<gfx::CommandBuffer> commandBuffer = m_commandBufferPools.at(m_frameIdx)->get();

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
                },
                .depthAttachment = {
                    gfx::Framebuffer::Attachment{
                        .loadAction = gfx::LoadAction::clear,
                        .clearDepth = 1.0f,
                        .texture = m_depthTexture.at(m_frameIdx)
                    }
                }
            };

            commandBuffer->beginRenderPass(framebuffer);
            {
                commandBuffer->usePipeline(m_graphicsPipeline);
                commandBuffer->setParameterBlock(vpMatrixPBlock, 0);
                commandBuffer->setParameterBlock(modelMatrixPBlock, 1);
                commandBuffer->setParameterBlock(materialPBlock, 2);
                commandBuffer->useVertexBuffer(m_vertexBuffer);
                commandBuffer->drawIndexedVertices(m_indexBuffer);

                commandBuffer->imGuiRenderDrawData(ImGui::GetDrawData());
            }
            commandBuffer->endRenderPass();
            commandBuffer->presentDrawable(drawable);

            m_lastCommandBuffers.at(m_frameIdx) = commandBuffer.get();
            m_device->submitCommandBuffers(ext::move(commandBuffer));

            if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
            {
                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault();
            }

            m_frameIdx = (m_frameIdx + 1) % maxFrameInFlight;
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
    GLFWwindow* m_window = nullptr;
    ext::unique_ptr<gfx::Instance> m_instance;
    ext::unique_ptr<gfx::Surface> m_surface;
    ext::unique_ptr<gfx::Device> m_device;
    ext::shared_ptr<gfx::GraphicsPipeline> m_graphicsPipeline;
    ext::unique_ptr<gfx::Swapchain> m_swapchain;

    ext::shared_ptr<gfx::Buffer> m_vertexBuffer;
    ext::shared_ptr<gfx::Buffer> m_indexBuffer;

    uint8_t m_frameIdx = 0;
    ext::array<ext::unique_ptr<gfx::CommandBufferPool>, maxFrameInFlight> m_commandBufferPools;
    ext::array<ext::unique_ptr<gfx::ParameterBlockPool>, maxFrameInFlight> m_parameterBlockPools;
    ext::array<gfx::CommandBuffer*, maxFrameInFlight> m_lastCommandBuffers = {};

    ext::array<ext::shared_ptr<gfx::Buffer>, maxFrameInFlight> m_vpMatrix;

    glm::vec3 m_cubePos = {0, 0, 0};
    glm::vec3 m_cubeRot = {0, 0, 0};
    glm::vec3 m_cubeSca = {1, 1, 1};
    ext::array<ext::shared_ptr<gfx::Buffer>, maxFrameInFlight> m_modelMatrix;

    ext::array<ext::shared_ptr<gfx::Buffer>, maxFrameInFlight> m_color;

    ext::array<ext::shared_ptr<gfx::Texture>, maxFrameInFlight> m_depthTexture;

};

int main()
{
    Application app;
    app.init();
    app.loop();
    app.clean();
}
