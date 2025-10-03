/*
 * ---------------------------------------------------
 * Renderer.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/09/18 09:26:08
 * ---------------------------------------------------
 */

#include "Renderer.hpp"
#include "AssetLoader.hpp"
#include "Light.hpp"

#include <Graphics/Buffer.hpp>
#include <Graphics/Enums.hpp>

#include <GLFW/glfw3.h>
#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>

#include <cassert>
#include <memory>
#include <utility>

namespace scop
{

Renderer::Renderer(gfx::Device* device, GLFWwindow* window, gfx::Surface* surface)
    : m_device(device), m_window(window), m_surface(surface)
{
    for (auto& frameData : m_frameDatas)
    {
        frameData.commandBufferPool = m_device->newCommandBufferPool();
        assert(frameData.commandBufferPool);

        frameData.parameterBlockPool = m_device->newParameterBlockPool();
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
}

void Renderer::beginFrame(const Camera& camera)
{
    if (cfd.lastCommandBuffer != nullptr)
    {
        m_device->waitCommandBuffer(cfd.lastCommandBuffer);
        cfd.lastCommandBuffer = nullptr;
        cfd.renderables.clear();
        cfd.availableModelMatrixBuffers.append_range(cfd.usedModelMatrixBuffers);
        cfd.usedModelMatrixBuffers.clear();
    }

    if (m_swapchain == nullptr)
    {
        m_device->waitIdle();

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
            .width = (uint32_t)width, .height = (uint32_t)height, .pixelFormat = gfx::PixelFormat::Depth32Float, .usages = gfx::TextureUsage::depthStencilAttachment, .storageMode = gfx::ResourceStorageMode::deviceLocal};
        for (auto& frameData : m_frameDatas)
            frameData.depthTexture = m_device->newTexture(depthTextureDescriptor);
    }

    int width = 0, height = 0;
    ::glfwGetFramebufferSize(m_window, &width, &height);
    glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 10.0f);
    *cfd.vpMatrix->content<glm::mat4x4>() = projectionMatrix * camera.viewMatrix();

    cfsd = shader::SceneData{
        .cameraPosition = -camera.viewMatrix()[3],
        .ambientLightColor = glm::vec3(0),
        .directionalLightCount = 0,
        .directionalLights = {},
        .pointLightCount = 0,
        .pointLights = {}
    };

    m_device->imguiNewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void Renderer::renderMesh(const Mesh& mesh, glm::mat4x4 worlTransform)
{
    std::function<void(const SubMesh&, glm::mat4x4)> addSubmesh = [&](const SubMesh& submesh, glm::mat4x4 transform) {
        std::shared_ptr<gfx::Buffer> modelMatrix;
        if (cfd.availableModelMatrixBuffers.empty() == false)
        {
            modelMatrix = cfd.availableModelMatrixBuffers.front();
            cfd.availableModelMatrixBuffers.pop_front();
        }
        else
        {
            modelMatrix = m_device->newBuffer(gfx::Buffer::Descriptor{
                .size = sizeof(glm::mat4x4),
                .usages = gfx::BufferUsage::uniformBuffer,
                .storageMode = gfx::ResourceStorageMode::hostVisible});
        }
        cfd.usedModelMatrixBuffers.push_back(modelMatrix);
        *modelMatrix->content<glm::mat4x4>() = transform * submesh.transform;

        for (auto& submesh : submesh.subMeshes)
            addSubmesh(submesh, *modelMatrix->content<glm::mat4x4>());

        cfd.renderables[submesh.material->graphicsPipleine()][submesh.material][ext::make_pair(submesh.vertexBuffer, submesh.indexBuffer)].push_back(modelMatrix);
    };

    for (auto& submesh : mesh.subMeshes)
        addSubmesh(submesh, worlTransform);
}

void Renderer::addLight(const Light& light, const glm::vec3& position)
{
    if (const auto* directionalLight = dynamic_cast<const DirectionalLight*>(&light))
    {
        if (static_cast<size_t>(cfsd.directionalLightCount) >= sizeof(cfsd.directionalLights) / sizeof(cfsd.directionalLights[0]))
            return;
        cfsd.directionalLights[cfsd.directionalLightCount++] = shader::DirectionalLight{ // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
            .position = position,
            .color = directionalLight->color()
        };
    }
    else if (const auto* pointLight = dynamic_cast<const PointLight*>(&light))
    {
        if (static_cast<size_t>(cfsd.pointLightCount) >= sizeof(cfsd.pointLights) / sizeof(cfsd.pointLights[0]))
            return;
        cfsd.pointLights[cfsd.pointLightCount++] = shader::PointLight{ // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
            .position = position,
            .color = pointLight->color()
        };
    }
}

void Renderer::endFrame()
{
    ImGui::Render();

    std::unique_ptr<gfx::CommandBuffer> commandBuffer = cfd.commandBufferPool->get();

    std::shared_ptr<gfx::Drawable> drawable = m_swapchain->nextDrawable();
    if (drawable == nullptr)
    {
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
        std::shared_ptr<gfx::ParameterBlock> vpMatrixPBlock = cfd.parameterBlockPool->get(vpMatrixBpLayout);
        vpMatrixPBlock->setBinding(0, cfd.vpMatrix);

        std::shared_ptr<gfx::ParameterBlock> sceneDataPBlock = cfd.parameterBlockPool->get(sceneDataBpLayout);
        sceneDataPBlock->setBinding(0, cfd.sceneDataBuffer);

        for (auto& [pipeline, renderables] : cfd.renderables)
        {
            commandBuffer->usePipeline(pipeline);
            commandBuffer->setParameterBlock(vpMatrixPBlock, 0);
            commandBuffer->setParameterBlock(sceneDataPBlock, 2);

            for (auto& [material, buffers] : renderables)
            {
                std::shared_ptr<gfx::ParameterBlock> materialPBlock = material->makeParameterBlock(*cfd.parameterBlockPool);
                commandBuffer->setParameterBlock(materialPBlock, 3);
                for (auto& [vtxIdxBuffer, modelMatrices] : buffers)
                {
                    auto& [vertexBuffer, indexBuffer] = vtxIdxBuffer;
                    commandBuffer->useVertexBuffer(vertexBuffer);
                    for (auto& modelMatrix : modelMatrices)
                    {
                        std::shared_ptr<gfx::ParameterBlock> modelMatrixPBlock = cfd.parameterBlockPool->get(modelMatrixBpLayout);
                        modelMatrixPBlock->setBinding(0, modelMatrix);
                        commandBuffer->setParameterBlock(modelMatrixPBlock, 1);
                        commandBuffer->drawIndexedVertices(indexBuffer);
                    }
                }
            }
        }

        commandBuffer->imGuiRenderDrawData(ImGui::GetDrawData());
    }
    commandBuffer->endRenderPass();

    commandBuffer->presentDrawable(drawable);

    cfd.lastCommandBuffer = commandBuffer.get();
    m_device->submitCommandBuffers(ext::move(commandBuffer));

    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }

    m_frameIdx = (m_frameIdx + 1) % maxFrameInFlight;
}

Renderer::~Renderer()
{
    m_device->imguiShutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

} // namespace scop
