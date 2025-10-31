/*
 * ---------------------------------------------------
 * scop.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/09/16 07:05:29
 * ---------------------------------------------------
 */

#include "Entity.hpp"
#include "Renderer.hpp"
#include "AssetLoader.hpp"
#include "Material.hpp"
#include "Light.hpp"

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
#include <numbers>
#include <print>
#include <cstdio>
#include <exception>
#include <numbers>
#include <bit>

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

extern "C"
{
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
        GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "scop", nullptr, nullptr);
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
        scop::AssetLoader assetLoader(device.get());

        scop::FlatColorMaterial::createPipeline(*device);
        scop::TexturedCubeMaterial::createPipeline(*device);
        scop::TexturedMaterial::createPipeline(*device);

        scop::FixCamera camera = scop::FixCamera();

        scop::PointLight pointLight;
        pointLight.setColor(glm::vec3(1.0f, 1.0f, 1.0f) * 0.8f);
        glm::vec3 pointLightPos = {0, 0, 3};

        scop::RenderableEntity afterTheRain = assetLoader.loadMesh(RESOURCE_DIR"/after_the_rain.glb");

        afterTheRain.setPosition({0.310f, 0.720f, 0.020f});
        afterTheRain.setRotation({-1.200f, std::numbers::pi_v<float>, 0.0f});
        afterTheRain.setScale(0.040f);

        glm::vec3 ambientLightColor = glm::vec3(1.0f, 1.0f, 1.0f) * 0.1f;

        while (true)
        {
            glfwPollEvents();
            if (glfwWindowShouldClose(window))
                break;

            renderer.beginFrame(camera);

            ImGui::Begin("settings");
            {
                {
                    glm::vec3 position = afterTheRain.position();
                    ImGui::DragFloat3("afterTheRain position", std::bit_cast<float*>(&position), 0.01f, -5.0f, 5.0f);
                    afterTheRain.setPosition(position);

                    glm::vec3 rotation = afterTheRain.rotation();
                    ImGui::DragFloat3("afterTheRain rotation", std::bit_cast<float*>(&rotation), 0.01f, -std::numbers::pi_v<float>, std::numbers::pi_v<float>);
                    afterTheRain.setRotation(rotation);

                    float scale = afterTheRain.scale();
                    ImGui::DragFloat("afterTheRain scale", std::bit_cast<float*>(&scale), 0.01f, 0.01f, 10);
                    afterTheRain.setScale(scale);
                }

                ImGui::Spacing();

                {
                    glm::vec3 lightColor = pointLight.color();
                    ImGui::ColorEdit3("light color", std::bit_cast<float*>(&lightColor));
                    pointLight.setColor(lightColor);

                    ImGui::DragFloat3("light position", std::bit_cast<float*>(&pointLightPos), 0.01f, -5.0f, 5.0f);
                }

                ImGui::Spacing();

                {
                    ImGui::ColorEdit3("ambient light color", std::bit_cast<float*>(&ambientLightColor));
                    renderer.setAmbientLightColor(ambientLightColor);
                }
            }
            ImGui::End();

            renderer.renderMesh(afterTheRain.mesh(), afterTheRain.modelMatrix());

            renderer.addLight(pointLight, pointLightPos);

            renderer.endFrame();
        }

        scop::TexturedMaterial::destroyPipeline();
        scop::TexturedCubeMaterial::destroyPipeline();
        scop::FlatColorMaterial::destroyPipeline();

        glfwDestroyWindow(window);
        glfwTerminate();
    }
    catch (const std::exception& e)
    {
        std::println(stderr, "{}", e.what());
        return 1;
    }
}
