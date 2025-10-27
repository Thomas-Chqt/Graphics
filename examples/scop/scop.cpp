/*
 * ---------------------------------------------------
 * scop.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/09/16 07:05:29
 * ---------------------------------------------------
 */

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

        scop::FixCamera camera = scop::FixCamera();

        scop::PointLight pointLight;
        pointLight.setColor(glm::vec3(1.0f, 1.0f, 1.0f) * 0.8f);
        glm::vec3 pointLightPos = {0, 0, 3};

        auto flatColorWhiteMaterial = std::make_shared<scop::FlatColorMaterial>(*device);
        flatColorWhiteMaterial->setColor(glm::vec3(1.0f, 1.0f, 1.0f));
        flatColorWhiteMaterial->setShininess(32.0f);
        flatColorWhiteMaterial->setSpecular(0.5f);

        auto mcCubeMaterial = std::make_shared<scop::TexturedCubeMaterial>(*device);
        mcCubeMaterial->setTexture(assetLoader.loadCubeTexture(RESOURCE_DIR"/mc_grass_side.jpg", RESOURCE_DIR"/mc_grass_side.jpg", RESOURCE_DIR"/mc_grass_top.jpg", RESOURCE_DIR"/mc_grass_bottom.jpg", RESOURCE_DIR"/mc_grass_side.jpg", RESOURCE_DIR"/mc_grass_side.jpg"));

        scop::Mesh cube = assetLoader.builtinCube([&]() { return mcCubeMaterial; });
        glm::vec3 cubePosition = {0.5, 0, 0};
        glm::vec3 cubeRotation = {0, 0, 0};
        float cubeScale = 1.0f;

        scop::Mesh chess = assetLoader.loadMesh(RESOURCE_DIR"/chess_set/chess_set.gltf", [&]() { return flatColorWhiteMaterial; });
        glm::vec3 chessPosition = {-0.5, 0, 0};
        glm::vec3 chessRotation = {0.5, 0, 0};
        float chessScale = 1.5f;

        glm::vec3 ambientLightColor = glm::vec3(1.0f, 1.0f, 1.0f) * 0.1f;

        while (true)
        {
            glfwPollEvents();
            if (glfwWindowShouldClose(window))
                break;

            renderer.beginFrame(camera);

            auto cubeModelMatrix = glm::mat4x4(1.0f);
            auto chessModelMatrix = glm::mat4x4(1.0f);

            ImGui::Begin("settings");
            {
                {
                    ImGui::DragFloat3("cube position", std::bit_cast<float*>(&cubePosition), 0.01f, -5.0f, 5.0f);
                    cubeModelMatrix = glm::translate(cubeModelMatrix, cubePosition);

                    ImGui::DragFloat3("cube rotation", std::bit_cast<float*>(&cubeRotation), 0.01f, -std::numbers::pi_v<float>, std::numbers::pi_v<float>);
                    cubeModelMatrix = glm::rotate(cubeModelMatrix, cubeRotation.x, glm::vec3(1, 0, 0));
                    cubeModelMatrix = glm::rotate(cubeModelMatrix, cubeRotation.y, glm::vec3(0, 1, 0));
                    cubeModelMatrix = glm::rotate(cubeModelMatrix, cubeRotation.z, glm::vec3(0, 0, 1));

                    ImGui::DragFloat("cube scale", std::bit_cast<float*>(&cubeScale), 0.01f, 0.01f, 10);
                    cubeModelMatrix = glm::scale(cubeModelMatrix, glm::vec3(1.0f, 1.0f, 1.0f) * cubeScale);
                }

                {
                    ImGui::DragFloat3("chess position", std::bit_cast<float*>(&chessPosition), 0.01f, -5.0f, 5.0f);
                    chessModelMatrix = glm::translate(chessModelMatrix, chessPosition);

                    ImGui::DragFloat3("chess rotation", std::bit_cast<float*>(&chessRotation), 0.01f, -std::numbers::pi_v<float>, std::numbers::pi_v<float>);
                    chessModelMatrix = glm::rotate(chessModelMatrix, chessRotation.x, glm::vec3(1, 0, 0));
                    chessModelMatrix = glm::rotate(chessModelMatrix, chessRotation.y, glm::vec3(0, 1, 0));
                    chessModelMatrix = glm::rotate(chessModelMatrix, chessRotation.z, glm::vec3(0, 0, 1));

                    ImGui::DragFloat("chess scale", std::bit_cast<float*>(&chessScale), 0.01f, 0.01f, 10);
                    chessModelMatrix = glm::scale(chessModelMatrix, glm::vec3(1.0f, 1.0f, 1.0f) * chessScale);
                }

                ImGui::Spacing();

                {
                    glm::vec3 color = flatColorWhiteMaterial->color();
                    ImGui::ColorEdit3("color", std::bit_cast<float*>(&color));
                    flatColorWhiteMaterial->setColor(color);

                    float shininess = flatColorWhiteMaterial->shininess();
                    ImGui::SliderFloat("shininess", &shininess, 0.1f, 32.0f);
                    flatColorWhiteMaterial->setShininess(shininess);

                    float specular = flatColorWhiteMaterial->specular();
                    ImGui::SliderFloat("specular", &specular, 0, 1.0f);
                    flatColorWhiteMaterial->setSpecular(specular);
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

            renderer.renderMesh(cube, cubeModelMatrix);
            renderer.renderMesh(chess, chessModelMatrix);

            renderer.addLight(pointLight,  pointLightPos);

            renderer.endFrame();
        }

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
