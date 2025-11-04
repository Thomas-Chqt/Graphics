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
#include "Graphics/ParameterBlockPool.hpp"
#include "Graphics/ShaderLib.hpp"
#include "Graphics/Surface.hpp"
#include "Graphics/Enums.hpp"
#include "Graphics/Swapchain.hpp"
#include "Graphics/VertexLayout.hpp"
#include "glm/fwd.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <cstring>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <stb_image/stb_image.h>

#include <memory>
#include <cassert>
#include <cstdint>
#include <cstddef>
#include <array>
#include <numbers>
#include <algorithm>
#include <bit>

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
    glm::vec2 uv;
};

constexpr std::array<Vertex, 24> cube_vertices = {
    // Front face (+Z)
    Vertex{ .pos=glm::vec3(-0.5f, -0.5f,  0.5f), .uv=glm::vec2(0.0f, 1.0f) },
    Vertex{ .pos=glm::vec3( 0.5f, -0.5f,  0.5f), .uv=glm::vec2(1.0f, 1.0f) },
    Vertex{ .pos=glm::vec3( 0.5f,  0.5f,  0.5f), .uv=glm::vec2(1.0f, 0.0f) },
    Vertex{ .pos=glm::vec3(-0.5f,  0.5f,  0.5f), .uv=glm::vec2(0.0f, 0.0f) },
    // Back face (-Z)
    Vertex{ .pos=glm::vec3( 0.5f, -0.5f, -0.5f), .uv=glm::vec2(0.0f, 1.0f) },
    Vertex{ .pos=glm::vec3(-0.5f, -0.5f, -0.5f), .uv=glm::vec2(1.0f, 1.0f) },
    Vertex{ .pos=glm::vec3(-0.5f,  0.5f, -0.5f), .uv=glm::vec2(1.0f, 0.0f) },
    Vertex{ .pos=glm::vec3( 0.5f,  0.5f, -0.5f), .uv=glm::vec2(0.0f, 0.0f) },
    // Left face (-X)
    Vertex{ .pos=glm::vec3(-0.5f, -0.5f, -0.5f), .uv=glm::vec2(0.0f, 1.0f) },
    Vertex{ .pos=glm::vec3(-0.5f, -0.5f,  0.5f), .uv=glm::vec2(1.0f, 1.0f) },
    Vertex{ .pos=glm::vec3(-0.5f,  0.5f,  0.5f), .uv=glm::vec2(1.0f, 0.0f) },
    Vertex{ .pos=glm::vec3(-0.5f,  0.5f, -0.5f), .uv=glm::vec2(0.0f, 0.0f) },
    // Right face (+X)
    Vertex{ .pos=glm::vec3( 0.5f, -0.5f,  0.5f), .uv=glm::vec2(0.0f, 1.0f) },
    Vertex{ .pos=glm::vec3( 0.5f, -0.5f, -0.5f), .uv=glm::vec2(1.0f, 1.0f) },
    Vertex{ .pos=glm::vec3( 0.5f,  0.5f, -0.5f), .uv=glm::vec2(1.0f, 0.0f) },
    Vertex{ .pos=glm::vec3( 0.5f,  0.5f,  0.5f), .uv=glm::vec2(0.0f, 0.0f) },
    // Top face (+Y)
    Vertex{ .pos=glm::vec3(-0.5f,  0.5f,  0.5f), .uv=glm::vec2(0.0f, 1.0f) },
    Vertex{ .pos=glm::vec3( 0.5f,  0.5f,  0.5f), .uv=glm::vec2(1.0f, 1.0f) },
    Vertex{ .pos=glm::vec3( 0.5f,  0.5f, -0.5f), .uv=glm::vec2(1.0f, 0.0f) },
    Vertex{ .pos=glm::vec3(-0.5f,  0.5f, -0.5f), .uv=glm::vec2(0.0f, 0.0f) },
    // Bottom face (-Y)
    Vertex{ .pos=glm::vec3(-0.5f, -0.5f, -0.5f), .uv=glm::vec2(0.0f, 1.0f) },
    Vertex{ .pos=glm::vec3( 0.5f, -0.5f, -0.5f), .uv=glm::vec2(1.0f, 1.0f) },
    Vertex{ .pos=glm::vec3( 0.5f, -0.5f,  0.5f), .uv=glm::vec2(1.0f, 0.0f) },
    Vertex{ .pos=glm::vec3(-0.5f, -0.5f,  0.5f), .uv=glm::vec2(0.0f, 0.0f) }
};

constexpr std::array<uint32_t, 36> cube_indices = {
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
        gfx::ParameterBlock::Binding{ .type = gfx::BindingType::sampledTexture, .usages = gfx::BindingUsage::fragmentRead },
        gfx::ParameterBlock::Binding{ .type = gfx::BindingType::sampler, .usages = gfx::BindingUsage::fragmentRead }
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

        std::unique_ptr<gfx::ShaderLib> shaderLib = m_device->newShaderLib(SHADER_SLIB);
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
            m_parameterBlockPools.at(i) = m_device->newParameterBlockPool(gfx::ParameterBlockPool::Descriptor{});
        }

        {
            m_vertexBuffer = m_device->newBuffer(gfx::Buffer::Descriptor{
                .size = sizeof(Vertex) * cube_vertices.size(),
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

            std::ranges::copy(cube_vertices, stagingBuffer->content<Vertex>());

            std::unique_ptr<gfx::CommandBuffer> commandBuffer = m_commandBufferPools.at(m_frameIdx)->get();
            commandBuffer->beginBlitPass();
            commandBuffer->copyBufferToBuffer(stagingBuffer, m_vertexBuffer, m_vertexBuffer->size());
            commandBuffer->endBlitPass();
            m_device->submitCommandBuffers(std::move(commandBuffer));
        }

        {
            m_indexBuffer = m_device->newBuffer(gfx::Buffer::Descriptor{
                .size = sizeof(uint32_t) * cube_indices.size(),
                .usages = gfx::BufferUsage::indexBuffer | gfx::BufferUsage::copyDestination,
                .storageMode = gfx::ResourceStorageMode::deviceLocal
            });
            assert(m_indexBuffer);

            std::shared_ptr<gfx::Buffer> stagingBuffer = m_device->newBuffer(gfx::Buffer::Descriptor{
                .size = m_indexBuffer->size(),
                .usages = gfx::BufferUsage::copySource,
                .storageMode = gfx::ResourceStorageMode::hostVisible
            });
            assert(stagingBuffer);

            std::ranges::copy(cube_indices, stagingBuffer->content<uint32_t>());

            std::unique_ptr<gfx::CommandBuffer> commandBuffer = m_commandBufferPools.at(m_frameIdx)->get();
            commandBuffer->beginBlitPass();
            commandBuffer->copyBufferToBuffer(stagingBuffer, m_indexBuffer, m_indexBuffer->size());
            commandBuffer->endBlitPass();
            m_device->submitCommandBuffers(std::move(commandBuffer));
        }

        {
            int width = 0;
            int height = 0;
            stbi_uc* sideBytes = stbi_load(RESOURCE_DIR"/mc_grass_side.jpg", &width, &height, nullptr, STBI_rgb_alpha);
            stbi_uc* topBytes = stbi_load(RESOURCE_DIR"/mc_grass_top.jpg", &width, &height, nullptr, STBI_rgb_alpha);
            stbi_uc* bottomBytes = stbi_load(RESOURCE_DIR"/mc_grass_bottom.jpg", &width, &height, nullptr, STBI_rgb_alpha);

            m_grassTexture = m_device->newTexture(gfx::Texture::Descriptor{
                .type = gfx::TextureType::textureCube,
                .width = static_cast<uint32_t>(width),
                .height = static_cast<uint32_t>(height),
                .pixelFormat = gfx::PixelFormat::RGBA8Unorm,
                .usages = gfx::TextureUsage::copyDestination | gfx::TextureUsage::shaderRead,
                .storageMode = gfx::ResourceStorageMode::deviceLocal
            });

            size_t faceSize = static_cast<size_t>(width) * static_cast<size_t>(height) * pixelFormatSize(gfx::PixelFormat::RGBA8Unorm);
            std::shared_ptr<gfx::Buffer> stagingBuffer = m_device->newBuffer(gfx::Buffer::Descriptor{
                .size = faceSize * 6, // 6 faces
                .usages = gfx::BufferUsage::copySource,
                .storageMode = gfx::ResourceStorageMode::hostVisible
            });
            assert(stagingBuffer);

            auto* bufferData = stagingBuffer->content<stbi_uc>();
            std::memcpy(bufferData + 0 * faceSize, sideBytes, faceSize);   // +X (right)
            std::memcpy(bufferData + 1 * faceSize, sideBytes, faceSize);   // -X (left)
            std::memcpy(bufferData + 2 * faceSize, topBytes, faceSize);    // +Y (top)
            std::memcpy(bufferData + 3 * faceSize, bottomBytes, faceSize); // -Y (bottom)
            std::memcpy(bufferData + 4 * faceSize, sideBytes, faceSize);   // +Z (front)
            std::memcpy(bufferData + 5 * faceSize, sideBytes, faceSize);   // -Z (back)

            stbi_image_free(sideBytes);
            stbi_image_free(topBytes);
            stbi_image_free(bottomBytes);

            std::unique_ptr<gfx::CommandBuffer> commandBuffer = m_commandBufferPools.at(m_frameIdx)->get();
            commandBuffer->beginBlitPass();
            {
                for (int face = 0; face < 6; ++face)
                    commandBuffer->copyBufferToTexture(stagingBuffer, face * faceSize, m_grassTexture, face);
            }
            commandBuffer->endBlitPass();
            m_device->submitCommandBuffers(std::move(commandBuffer));
        }

        m_sampler = m_device->newSampler(gfx::Sampler::Descriptor{});

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
                    .drawableCount = maxFrameInFlight,
                    .pixelFormat = gfx::PixelFormat::BGRA8Unorm,
                    .presentMode = gfx::PresentMode::fifo,
                };
                m_swapchain = m_device->newSwapchain(swapchainDescriptor);
                assert(m_swapchain);

                gfx::Texture::Descriptor depthTextureDescriptor = {
                    .width = (uint32_t)width, .height = (uint32_t)height,
                    .pixelFormat = gfx::PixelFormat::Depth32Float,
                    .usages = gfx::TextureUsage::depthStencilAttachment,
                    .storageMode = gfx::ResourceStorageMode::deviceLocal
                };
                for (auto i = 0; i < maxFrameInFlight; i++)
                    m_depthTexture.at(i) = m_device->newTexture(depthTextureDescriptor);
                m_device->waitIdle();
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

                    ImGui::DragFloat3("position", std::bit_cast<float*>(&m_cubePos), 0.01f, -5.0, 5.0);
                    *m_modelMatrix.at(m_frameIdx)->content<glm::mat4x4>() = glm::translate(*m_modelMatrix.at(m_frameIdx)->content<glm::mat4x4>(), m_cubePos);

                    ImGui::DragFloat3("rotation", std::bit_cast<float*>(&m_cubeRot), 0.01f, -std::numbers::pi_v<float>, std::numbers::pi_v<float>);
                    *m_modelMatrix.at(m_frameIdx)->content<glm::mat4x4>() = glm::rotate(*m_modelMatrix.at(m_frameIdx)->content<glm::mat4x4>(), m_cubeRot.x, glm::vec3(1, 0, 0));
                    *m_modelMatrix.at(m_frameIdx)->content<glm::mat4x4>() = glm::rotate(*m_modelMatrix.at(m_frameIdx)->content<glm::mat4x4>(), m_cubeRot.y, glm::vec3(0, 1, 0));
                    *m_modelMatrix.at(m_frameIdx)->content<glm::mat4x4>() = glm::rotate(*m_modelMatrix.at(m_frameIdx)->content<glm::mat4x4>(), m_cubeRot.z, glm::vec3(0, 0, 1));

                    ImGui::DragFloat3("scale", std::bit_cast<float*>(&m_cubeSca), 0.01f, -2, 2);
                    *m_modelMatrix.at(m_frameIdx)->content<glm::mat4x4>() = glm::scale(*m_modelMatrix.at(m_frameIdx)->content<glm::mat4x4>(), m_cubeSca);
                }
                ImGui::End();
            }
            ImGui::Render();

            std::shared_ptr<gfx::ParameterBlock> vpMatrixPBlock = m_parameterBlockPools.at(m_frameIdx)->get(vpMatrixBpLayout);
            vpMatrixPBlock->setBinding(0, m_vpMatrix.at(m_frameIdx));

            std::shared_ptr<gfx::ParameterBlock> modelMatrixPBlock = m_parameterBlockPools.at(m_frameIdx)->get(modelMatrixBpLayout);
            modelMatrixPBlock->setBinding(0, m_modelMatrix.at(m_frameIdx));

            std::shared_ptr<gfx::ParameterBlock> materialPBlock = m_parameterBlockPools.at(m_frameIdx)->get(materialBpLayout);
            materialPBlock->setBinding(0, m_grassTexture);
            materialPBlock->setBinding(1, m_sampler);

            std::unique_ptr<gfx::CommandBuffer> commandBuffer = m_commandBufferPools.at(m_frameIdx)->get();

            std::shared_ptr<gfx::Drawable> drawable = m_swapchain->nextDrawable();
            if (drawable == nullptr) {
                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault();
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
            m_device->submitCommandBuffers(std::move(commandBuffer));

            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();

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
    std::unique_ptr<gfx::Instance> m_instance;
    std::unique_ptr<gfx::Surface> m_surface;
    std::unique_ptr<gfx::Device> m_device;
    std::shared_ptr<gfx::GraphicsPipeline> m_graphicsPipeline;
    std::unique_ptr<gfx::Swapchain> m_swapchain;

    std::shared_ptr<gfx::Buffer> m_vertexBuffer;
    std::shared_ptr<gfx::Buffer> m_indexBuffer;

    uint8_t m_frameIdx = 0;
    std::array<std::unique_ptr<gfx::CommandBufferPool>, maxFrameInFlight> m_commandBufferPools;
    std::array<std::unique_ptr<gfx::ParameterBlockPool>, maxFrameInFlight> m_parameterBlockPools;
    std::array<gfx::CommandBuffer*, maxFrameInFlight> m_lastCommandBuffers = {};

    std::array<std::shared_ptr<gfx::Buffer>, maxFrameInFlight> m_vpMatrix;

    glm::vec3 m_cubePos = {0, 0, 0};
    glm::vec3 m_cubeRot = {0, 0, 0};
    glm::vec3 m_cubeSca = {1, 1, 1};
    std::array<std::shared_ptr<gfx::Buffer>, maxFrameInFlight> m_modelMatrix;

    std::array<std::shared_ptr<gfx::Texture>, maxFrameInFlight> m_depthTexture;

    std::shared_ptr<gfx::Texture> m_grassTexture;
    std::shared_ptr<gfx::Sampler> m_sampler;
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
