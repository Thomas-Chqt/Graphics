/*
 * ---------------------------------------------------
 * scop.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/09/16 07:05:29
 * ---------------------------------------------------
 */

#include "Renderer.hpp"
#include "MeshLoader.hpp"

#include <Graphics/Instance.hpp>
#include <Graphics/Surface.hpp>
#include <Graphics/Device.hpp>
#include <Graphics/GraphicsPipeline.hpp>
#include <Graphics/Buffer.hpp>
#include <Graphics/Enums.hpp>

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <glm/glm.hpp>

#include <cassert>
#include <memory>
#include <print>
#include <cstdio>
#include <exception>
#include <numbers>

#if __XCODE__
    #include <unistd.h>
#endif

#if (defined(__GNUC__) || defined(__clang__))
    #define SCOP_EXPORT __attribute__((visibility("default")))
#elif defined(_MSC_VER)
    #define SCOP_EXPORT __declspec(dllexport)
#else
    #error "unknown compiler"
#endif

extern "C" {
SCOP_EXPORT ImGuiContext* GetCurrentContext() { return ImGui::GetCurrentContext(); }
SCOP_EXPORT ImGuiIO* GetIO() { return &ImGui::GetIO(); }
SCOP_EXPORT ImGuiPlatformIO* GetPlatformIO() { return &ImGui::GetPlatformIO(); }
SCOP_EXPORT ImGuiViewport* GetMainViewport() { return ImGui::GetMainViewport(); }
SCOP_EXPORT bool DebugCheckVersionAndDataLayout(const char* version_str, size_t sz_io, size_t sz_style, size_t sz_vec2, size_t sz_vec4, size_t sz_drawvert, size_t sz_drawidx) { return ImGui::DebugCheckVersionAndDataLayout(version_str, sz_io, sz_style, sz_vec2, sz_vec4, sz_drawvert, sz_drawidx); }
SCOP_EXPORT void* MemAlloc(size_t size) { return ImGui::MemAlloc(size); }
SCOP_EXPORT void MemFree(void* ptr) { return ImGui::MemFree(ptr); }
SCOP_EXPORT void DestroyPlatformWindows() { return ImGui::DestroyPlatformWindows(); }
}

constexpr uint32_t WINDOW_WIDTH = 800;
constexpr uint32_t WINDOW_HEIGHT = 600;

const gfx::ParameterBlock::Layout flatColorMaterialBpLayout = {
    .bindings = {
        gfx::ParameterBlock::Binding{.type = gfx::BindingType::uniformBuffer, .usages = gfx::BindingUsage::fragmentRead},
    }};

class FlatColorMaterial : public scop::Material
{
public:
    FlatColorMaterial(const gfx::Device& device)
    {
        m_color = device.newBuffer(gfx::Buffer::Descriptor{
            .size = sizeof(glm::vec3),
            .usages = gfx::BufferUsage::uniformBuffer,
            .storageMode = gfx::ResourceStorageMode::hostVisible});
    }

    FlatColorMaterial(const FlatColorMaterial&) = delete;
    FlatColorMaterial(FlatColorMaterial&&) = delete;

    static void createPipeline(gfx::Device& device)
    {
        if (s_graphicsPipeline != nullptr)
            return;
        ext::unique_ptr<gfx::ShaderLib> shaderLib = device.newShaderLib(SHADER_DIR "/flat_color.slib");
        assert(shaderLib);

        gfx::GraphicsPipeline::Descriptor gfxPipelineDescriptor = {
            .vertexLayout = gfx::VertexLayout{
                .stride = sizeof(scop::Vertex),
                .attributes = {
                    gfx::VertexAttribute{
                        .format = gfx::VertexAttributeFormat::float3,
                        .offset = offsetof(scop::Vertex, pos)}}},
            .vertexShader = &shaderLib->getFunction("vertexMain"),
            .fragmentShader = &shaderLib->getFunction("fragmentMain"),
            .colorAttachmentPxFormats = {gfx::PixelFormat::BGRA8Unorm},
            .depthAttachmentPxFormat = gfx::PixelFormat::Depth32Float,
            .parameterBlockLayouts = {scop::vpMatrixBpLayout, scop::modelMatrixBpLayout, flatColorMaterialBpLayout}};

        s_graphicsPipeline = device.newGraphicsPipeline(gfxPipelineDescriptor);
    }

    inline static std::shared_ptr<gfx::GraphicsPipeline> getPipeline() { return s_graphicsPipeline; }

    inline static void destroyPipeline() { s_graphicsPipeline.reset(); }

    inline const std::shared_ptr<gfx::GraphicsPipeline>& graphicsPipleine() const override { return s_graphicsPipeline; }

    std::unique_ptr<gfx::ParameterBlock> makeParameterBlock(gfx::ParameterBlockPool& pool) const override
    {
        ext::unique_ptr<gfx::ParameterBlock> parameterBlock = pool.get(flatColorMaterialBpLayout);
        parameterBlock->setBinding(0, m_color);
        return parameterBlock;
    }

    inline const char* name() const override { return "flat_color"; };

    inline glm::vec3 color() const { return *m_color->content<glm::vec3>(); }
    inline void setColor(const glm::vec3& c) { *m_color->content<glm::vec3>() = c; }

    ~FlatColorMaterial() override = default;

private:
    inline static std::shared_ptr<gfx::GraphicsPipeline> s_graphicsPipeline;

    ext::shared_ptr<gfx::Buffer> m_color;

public:
    FlatColorMaterial& operator=(const FlatColorMaterial&) = delete;
    FlatColorMaterial& operator=(FlatColorMaterial&&) = delete;
};

int main()
{
    try
    {

#if __XCODE__
        sleep(1); // XCODE BUG https://github.com/glfw/glfw/issues/2634
#endif

        auto res = glfwInit();
        assert(res == GLFW_TRUE);
        (void)res;

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "GLFW Window", nullptr, nullptr);
        assert(window);

        std::unique_ptr<gfx::Instance> instance = gfx::Instance::newInstance(gfx::Instance::Descriptor{});
        assert(instance);

        std::unique_ptr<gfx::Surface> surface = instance->createSurface(window);
        assert(surface);

        gfx::Device::Descriptor deviceDescriptor = {
            .queueCaps = {
                .graphics = true,
                .compute = true,
                .transfer = true,
                .present = {surface.get()}}};
        std::unique_ptr<gfx::Device> device = instance->newDevice(deviceDescriptor);
        assert(device);

        if (surface->supportedPixelFormats(*device).contains(gfx::PixelFormat::BGRA8Unorm) == false)
            throw std::runtime_error("surface does not support the BGRA8Unorm pixel format");

        if (surface->supportedPresentModes(*device).contains(gfx::PresentMode::fifo) == false)
            throw std::runtime_error("surface does not support the fifo present mode");

        scop::Renderer renderer(device.get(), window, surface.get());
        scop::MeshLoader meshLoader(device.get());
        
        FlatColorMaterial::createPipeline(*device);

        scop::FixCamera camera = scop::FixCamera();

        std::shared_ptr<FlatColorMaterial> cubeMaterial = std::make_shared<FlatColorMaterial>(*device);
        cubeMaterial->setColor({1, 1, 1});

        scop::Mesh cube = meshLoader.builtinCube([&]() { return cubeMaterial; });

        glm::vec3 cubePos = {0, 0, 0};
        glm::vec3 cubeRot = {0, 0, 0};
        glm::vec3 cubeSca = {1, 1, 1};

        while (true)
        {
            glfwPollEvents();
            if (glfwWindowShouldClose(window))
                break;

            renderer.beginFrame(camera);

            auto modelMatrix = glm::mat4x4(1.0f);

            ImGui::Begin("mc_cube");
            {
                ImGui::DragFloat3("position", ext::bit_cast<float*>(&cubePos), 0.01f, -5.0, 5.0);
                modelMatrix = glm::translate(modelMatrix, cubePos);

                ImGui::DragFloat3("rotation", ext::bit_cast<float*>(&cubeRot), 0.01f, -std::numbers::pi_v<float>, std::numbers::pi_v<float>);
                modelMatrix = glm::rotate(modelMatrix, cubeRot.x, glm::vec3(1, 0, 0));
                modelMatrix = glm::rotate(modelMatrix, cubeRot.y, glm::vec3(0, 1, 0));
                modelMatrix = glm::rotate(modelMatrix, cubeRot.z, glm::vec3(0, 0, 1));

                ImGui::DragFloat3("scale", ext::bit_cast<float*>(&cubeSca), 0.01f, -2, 2);
                modelMatrix = glm::scale(modelMatrix, cubeSca);
            }
            ImGui::End();

            renderer.renderMesh(cube, modelMatrix);

            renderer.endFrame();
        }

        FlatColorMaterial::destroyPipeline();

        glfwDestroyWindow(window);
        glfwTerminate();
    }
    catch (const std::exception& e)
    {
        std::println(stderr, "{}", e.what());
    }
}
