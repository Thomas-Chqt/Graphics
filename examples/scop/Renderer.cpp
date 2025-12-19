/*
 * ---------------------------------------------------
 * Renderer.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/09/18 09:26:08
 * ---------------------------------------------------
 */

#include "Renderer.hpp"
#include "Mesh.hpp"
#include "shaders/SceneData.slang"
#include "shaders/Light.slang"

#include <Graphics/Buffer.hpp>
#include <Graphics/Enums.hpp>

#include <GLFW/glfw3.h>
#if !defined (SCOP_MANDATORY)
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#else
#include "math/math.hpp"
#ifndef SCOP_MATH_GLM_ALIAS_DEFINED
#define SCOP_MATH_GLM_ALIAS_DEFINED
namespace glm = scop::math;
#endif
#endif

#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <cassert>
#include <memory>
#include <utility>

namespace scop
{

Renderer::Renderer(gfx::Device* device, GLFWwindow* window, gfx::Surface* surface)
    : m_device(device), m_window(window), m_surface(surface)
{
    glfwSetWindowUserPointer(m_window, this);
    glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int, int){
        static_cast<Renderer*>(glfwGetWindowUserPointer(window))->m_swapchain = nullptr;
    });

    for (auto& frameData : m_frameDatas)
    {
        frameData.commandBufferPool = m_device->newCommandBufferPool();
        assert(frameData.commandBufferPool);

        frameData.parameterBlockPool = m_device->newParameterBlockPool({
            .maxUniformBuffers = 2, .maxTextures = 0, .maxSamplers = 0
        });
        assert(frameData.parameterBlockPool);

        frameData.vpMatrix = m_device->newBuffer(gfx::Buffer::Descriptor{
            .size = sizeof(glm::mat4x4),
            .usages = gfx::BufferUsage::uniformBuffer,
            .storageMode = gfx::ResourceStorageMode::hostVisible});
        assert(frameData.vpMatrix);

        frameData.sceneDataBuffer = m_device->newBuffer(gfx::Buffer::Descriptor{
            .size = sizeof(shader::SceneData),
            .usages = gfx::BufferUsage::uniformBuffer,
            .storageMode = gfx::ResourceStorageMode::hostVisible});
    }

    m_vpMatrixBpLayout = m_device->newParameterBlockLayout(gfx::ParameterBlockLayout::Descriptor{
        .bindings = {
            gfx::ParameterBlockBinding{ .type = gfx::BindingType::uniformBuffer, .usages = gfx::BindingUsage::vertexRead }
        }
    });
    assert(m_vpMatrixBpLayout);
    s_vpMatrixBpLayout = m_vpMatrixBpLayout;

    m_sceneDataBpLayout = m_device->newParameterBlockLayout(gfx::ParameterBlockLayout::Descriptor{
        .bindings = {
            gfx::ParameterBlockBinding{ .type = gfx::BindingType::uniformBuffer, .usages = gfx::BindingUsage::fragmentRead }
        }
    });
    assert(m_sceneDataBpLayout);
    s_sceneDataBpLayout = m_sceneDataBpLayout;

#if !defined (SCOP_MANDATORY)
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

    m_device->imguiInit({gfx::PixelFormat::BGRA8Unorm}, gfx::PixelFormat::Depth32Float);
#endif
}

void Renderer::beginFrame(const glm::mat4x4& viewMatrix, float fov, float near, float far)
{
    if (m_swapchain == nullptr) {
        int width = 0, height = 0;
        ::glfwGetFramebufferSize(m_window, &width, &height);
        gfx::Swapchain::Descriptor swapchainDescriptor = {
            .surface = m_surface,
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
        for (auto& frameData : m_frameDatas)
            frameData.depthTexture = m_device->newTexture(depthTextureDescriptor);
        m_device->waitIdle();
    }

    if (cfd.lastCommandBuffer != nullptr) {
        m_device->waitCommandBuffer(*cfd.lastCommandBuffer);
        cfd.lastCommandBuffer.reset();
        cfd.commandBufferPool->reset();
        cfd.parameterBlockPool->reset();
    }

    cfd.renderables.clear();
    cfsd = shader::SceneData{
        .cameraPosition = -viewMatrix[3],
        .ambientLightColor = glm::vec3(0),
        .directionalLightCount = 0,
        .directionalLights = {},
        .pointLightCount = 0,
        .pointLights = {}
    };

    int width = 0, height = 0;
    ::glfwGetFramebufferSize(m_window, &width, &height);
    const float aspectRatio = static_cast<float>(width) / static_cast<float>(height == 0 ? 1 : height);
    glm::mat4 projectionMatrix = glm::perspective(glm::radians(fov), aspectRatio, near, far);
    *cfd.vpMatrix->content<glm::mat4x4>() = projectionMatrix * viewMatrix;

#if !defined (SCOP_MANDATORY)
    m_device->imguiNewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
#endif
}

void Renderer::addMesh(const Mesh& mesh, const glm::mat4x4& worlTransform)
{
    std::function<void(const SubMesh&, glm::mat4x4)> addSubmesh = [&](const SubMesh& submesh, const glm::mat4x4& transform) {
        glm::mat4x4 modelMatrix = transform * submesh.transform;

        for (auto& childSubmesh : submesh.subMeshes)
            addSubmesh(childSubmesh, modelMatrix);

        cfd.renderables[submesh.material->graphicsPipleine()][submesh.material][std::make_pair(submesh.vertexBuffer, submesh.indexBuffer)].push_back(modelMatrix);
    };

    for (auto& submesh : mesh.subMeshes)
        addSubmesh(submesh, worlTransform);
}

void Renderer::addPointLight(const glm::vec3& position, const glm::vec3& color)
{
    if (static_cast<size_t>(cfsd.pointLightCount) >= sizeof(cfsd.pointLights) / sizeof(cfsd.pointLights[0]))
        return;
    cfsd.pointLights[cfsd.pointLightCount++] = shader::PointLight{ // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
        .position = position,
        .color = color
    };
}

void Renderer::endFrame()
{
#if !defined (SCOP_MANDATORY)
    ImGui::Render();
#endif

    std::shared_ptr<gfx::CommandBuffer> commandBuffer = cfd.commandBufferPool->get();

    std::shared_ptr<gfx::Drawable> drawable = m_swapchain->nextDrawable();
    if (drawable == nullptr) {
#if !defined (SCOP_MANDATORY)
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
#endif
        m_swapchain = nullptr;
        return;
    }

    gfx::Framebuffer framebuffer = {
        .colorAttachments = {
            gfx::Framebuffer::Attachment{
                .loadAction = gfx::LoadAction::clear,
                .clearColor = {0.0f, 0.0f, 0.0f, 0.0f},
                .texture = drawable->texture()
            }
        },
        .depthAttachment = {
            gfx::Framebuffer::Attachment{
                .loadAction = gfx::LoadAction::clear,
                .clearDepth = 1.0f,
                .texture = cfd.depthTexture
            }
        }
    };

    commandBuffer->beginRenderPass(framebuffer);
    {
        std::shared_ptr<gfx::ParameterBlock> vpMatrixPBlock = cfd.parameterBlockPool->get(vpMatrixBpLayout());
        vpMatrixPBlock->setBinding(0, cfd.vpMatrix);

        std::shared_ptr<gfx::ParameterBlock> sceneDataPBlock = cfd.parameterBlockPool->get(sceneDataBpLayout());
        sceneDataPBlock->setBinding(0, cfd.sceneDataBuffer);

        for (auto& [pipeline, renderables] : cfd.renderables)
        {
            commandBuffer->usePipeline(pipeline);
            commandBuffer->setParameterBlock(vpMatrixPBlock, 0);
            commandBuffer->setParameterBlock(sceneDataPBlock, 1);

            for (auto& [material, buffers] : renderables)
            {
                commandBuffer->setParameterBlock(material->getParameterBlock(), 2);
                for (auto& [vtxIdxBuffer, modelMatrices] : buffers)
                {
                    auto& [vertexBuffer, indexBuffer] = vtxIdxBuffer;
                    commandBuffer->useVertexBuffer(vertexBuffer);
                    for (auto& modelMatrix : modelMatrices)
                    {
                        commandBuffer->setPushConstants(&modelMatrix);
                        commandBuffer->drawIndexedVertices(indexBuffer);
                    }
                }
            }
        }

#if !defined (SCOP_MANDATORY)
        commandBuffer->imGuiRenderDrawData(ImGui::GetDrawData());
#endif
    }
    commandBuffer->endRenderPass();
    commandBuffer->presentDrawable(drawable);

    cfd.lastCommandBuffer = commandBuffer;
    m_device->submitCommandBuffers(commandBuffer);

#if !defined (SCOP_MANDATORY)
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
#endif

    m_frameIdx = (m_frameIdx + 1) % maxFrameInFlight;
}

Renderer::~Renderer()
{
#if !defined (SCOP_MANDATORY)
    m_device->imguiShutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
#endif
}

} // namespace scop
