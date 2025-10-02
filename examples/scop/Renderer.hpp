/*
 * ---------------------------------------------------
 * Renderer.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/09/16 07:34:15
 * ---------------------------------------------------
 */

#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "Camera.hpp"
#include "AssetLoader.hpp"
#include "Material.hpp"
#include "Light.hpp"

#include <Graphics/Surface.hpp>
#include <Graphics/Device.hpp>
#include <Graphics/GraphicsPipeline.hpp>

#include <GLFW/glfw3.h>
#include <cstddef>
#include <glm/glm.hpp>

#include <array>
#include <map>
#include <memory>
#include <vector>
#include <deque>

#define cfd m_frameDatas.at(m_frameIdx)
#define cfsd (*cfd.sceneDataBuffer->content<SceneData>())

namespace scop
{

constexpr uint8_t maxFrameInFlight = 3;

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

const gfx::ParameterBlock::Layout sceneDataBpLayout = {
    .bindings = {
        gfx::ParameterBlock::Binding{ .type = gfx::BindingType::uniformBuffer, .usages = gfx::BindingUsage::fragmentRead }
    }
};

struct SceneData
{
    alignas(16) glm::vec3 cameraPosition;
    alignas(16) glm::vec3 ambientLightColor;

    alignas(16) int directionalLightCount;
    std::array<GPUDirectionalLight, 8> directionalLights;

    alignas(16) int pointLightCount;
    std::array<GPUPointLight, 8> pointLights;
};

static_assert(offsetof(SceneData, ambientLightColor) == 16);
static_assert(offsetof(SceneData, directionalLightCount) == 32);

class Renderer
{
public:
    Renderer() = delete;
    Renderer(const Renderer&) = delete;
    Renderer(Renderer&&) = delete;

    Renderer(gfx::Device*, GLFWwindow*, gfx::Surface*);

    void beginFrame(const Camera&);

    inline void setAmbientLightColor(glm::vec3 c) { cfsd.ambientLightColor = c; }

    void renderMesh(const Mesh&, glm::mat4x4 transform);
    void addLight(const Light&, const glm::vec3& position);

    void endFrame();

    ~Renderer();

private:
    struct FrameData
    {
        std::unique_ptr<gfx::CommandBufferPool> commandBufferPool;
        std::unique_ptr<gfx::ParameterBlockPool> parameterBlockPool;

        std::shared_ptr<gfx::Buffer> vpMatrix;

        ext::shared_ptr<gfx::Texture> depthTexture;

        ext::deque<std::shared_ptr<gfx::Buffer>> availableModelMatrixBuffers;
        ext::deque<std::shared_ptr<gfx::Buffer>> usedModelMatrixBuffers;

        std::shared_ptr<gfx::Buffer> sceneDataBuffer;

        std::map<
            std::shared_ptr<gfx::GraphicsPipeline>,
            std::map<
                std::shared_ptr<Material>,
                std::map<
                    std::pair<std::shared_ptr<gfx::Buffer>, std::shared_ptr<gfx::Buffer>>, // vertex buffer / index buffer
                    std::vector<std::shared_ptr<gfx::Buffer>> // model matrix
        >>> renderables;

        gfx::CommandBuffer* lastCommandBuffer = nullptr;
    };

    gfx::Device* m_device;
    GLFWwindow* m_window;
    gfx::Surface* m_surface;

    std::unique_ptr<gfx::Swapchain> m_swapchain;

    uint8_t m_frameIdx = 0;
    std::array<FrameData, maxFrameInFlight> m_frameDatas;

public:
    Renderer& operator=(const Renderer&) = delete;
    Renderer& operator=(Renderer&&) = delete;
};

} // namespace scop

#endif // RENDERER_HPP
