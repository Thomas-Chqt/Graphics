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
#include "Entity/Entity.hpp"
#include "Entity/Camera.hpp"
#include "Entity/Light.hpp"
#include "Entity/RenderableEntity.hpp"

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
#include <set>
#include <vector>

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

        static std::set<int> pressedKeys;
        glfwSetKeyCallback(window, [](GLFWwindow*, int key, int, int action, int) {
            if (action == GLFW_PRESS)
                pressedKeys.insert(key);
            if (action == GLFW_RELEASE && pressedKeys.contains(key))
                pressedKeys.erase(key);
        });

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

        std::vector<std::shared_ptr<scop::Entity>> entities;

        auto camera = std::make_shared<scop::FlightCamera>();
        camera->setName("Camera");
        entities.push_back(camera);

        auto light = std::make_shared<scop::PointLight>();
        light->setName("Point Light");
        light->setColor(glm::vec3(1.0f, 1.0f, 1.0f) * 0.8f);
        entities.push_back(light);

        //auto afterTheRain = std::make_shared<scop::RenderableEntity>(assetLoader.loadMesh(RESOURCE_DIR"/after_the_rain.glb"));
        //afterTheRain->setName("after_the_rain");
        //afterTheRain->setRotation({-std::numbers::pi_v<float> / 2, 0.0f, 0.0f});
        //entities.push_back(afterTheRain);

        //auto neighbourhood_city = std::make_shared<scop::RenderableEntity>(assetLoader.loadMesh(RESOURCE_DIR"/neighbourhood_city.glb"));
        //neighbourhood_city->setName("neighbourhood_city");
        //neighbourhood_city->setRotation({-std::numbers::pi_v<float> / 2, 0.0f, 0.0f});
        //entities.push_back(neighbourhood_city);

        //auto sponza = std::make_shared<scop::RenderableEntity>(assetLoader.loadMesh(RESOURCE_DIR"/sponza.glb"));
        //sponza->setName("sponza");
        //sponza->setRotation({-std::numbers::pi_v<float> / 2, 0.0f, 0.0f});
        //entities.push_back(sponza);

        auto bistro = std::make_shared<scop::RenderableEntity>(assetLoader.loadMesh(RESOURCE_DIR"/Bistro.glb"));
        bistro->setName("bistro");
        bistro->setRotation({std::numbers::pi_v<float> / 2, 0.0f, 0.0f});
        entities.push_back(bistro);

        glm::vec3 ambientLightColor = glm::vec3(1.0f, 1.0f, 1.0f) * 0.1f;
        scop::Entity* selectedEntity = nullptr;

        double lastFrameTime = glfwGetTime();

        while (true)
        {
            glfwPollEvents();
            if (glfwWindowShouldClose(window))
                break;

            double currentFrameTime = glfwGetTime();
            double deltaTime = currentFrameTime - lastFrameTime;
            lastFrameTime = currentFrameTime;

            glm::vec3 rotationInput = glm::vec3{
                (pressedKeys.contains(GLFW_KEY_UP)   ? 1.0f : 0.0f) - (pressedKeys.contains(GLFW_KEY_DOWN)  ? 1.0f : 0.0f),
                (pressedKeys.contains(GLFW_KEY_LEFT) ? 1.0f : 0.0f) - (pressedKeys.contains(GLFW_KEY_RIGHT) ? 1.0f : 0.0f),
                0
            };
            if (glm::length(rotationInput) > 0.0f)
                camera->setRotation(camera->rotation() + glm::normalize(rotationInput) * (float)deltaTime * 2.0f);

            auto rotationMat = glm::mat4x4(1.0f);
            rotationMat = glm::rotate(rotationMat, camera->rotation().y, glm::vec3(0, 1, 0));
            rotationMat = glm::rotate(rotationMat, camera->rotation().x, glm::vec3(1, 0, 0));

            glm::vec3 movementInput = glm::vec3{
                (pressedKeys.contains(GLFW_KEY_D) ? 1.0f : 0.0f) - (pressedKeys.contains(GLFW_KEY_A) ? 1.0f : 0.0f),
                0,
                (pressedKeys.contains(GLFW_KEY_S) ? 1.0f : 0.0f) - (pressedKeys.contains(GLFW_KEY_W) ? 1.0f : 0.0f)
            };
            if (glm::length(movementInput) > 0.0f)
                camera->setPosition(camera->position() + glm::vec3(rotationMat * glm::vec4(glm::normalize(movementInput), 0)) * (float)deltaTime * 4.0f);

            light->setPosition(camera->position());

            renderer.beginFrame(camera->viewMatrix());

            ImGui::Begin("entities");
            {
                for (auto& entity : entities) {
                    if (ImGui::Selectable(entity->name().c_str(), selectedEntity == entity.get()))
                        selectedEntity = entity.get();
                }
            }
            ImGui::End();

            ImGui::Begin("selected entity");
            {
                if (selectedEntity != nullptr)
                {
                    glm::vec3 position = selectedEntity->position();
                    ImGui::DragFloat3("position", std::bit_cast<float*>(&position), 0.01f, -5000.0f, 5000.0f);
                    selectedEntity->setPosition(position);

                    glm::vec3 rotation = selectedEntity->rotation();
                    ImGui::DragFloat3("rotation", std::bit_cast<float*>(&rotation), 0.01f, -std::numbers::pi_v<float>, std::numbers::pi_v<float>);
                    selectedEntity->setRotation(rotation);

                    if (auto* renderableEntity = dynamic_cast<scop::RenderableEntity*>(selectedEntity))
                    {
                        float scale = renderableEntity->scale();
                        ImGui::DragFloat("scale", std::bit_cast<float*>(&scale), 0.01f, 0.01f, 10);
                        renderableEntity->setScale(scale);
                    }

                    if (auto* light = dynamic_cast<scop::Light*>(selectedEntity))
                    {
                        glm::vec3 lightColor = light->color();
                        ImGui::ColorEdit3("light color", std::bit_cast<float*>(&lightColor));
                        light->setColor(lightColor);
                    }
                }
                else
                {
                    ImGui::Text("no entity selected"); // NOLINT(cppcoreguidelines-pro-type-vararg)
                }
            }
            ImGui::End();

            renderer.setAmbientLightColor(ambientLightColor);

            for (auto& entity : entities)
            {
                if (auto* renderableEntity = dynamic_cast<scop::RenderableEntity*>(entity.get()))
                    renderer.addMesh(renderableEntity->mesh(), renderableEntity->modelMatrix());

                if (auto* pointLight = dynamic_cast<scop::PointLight*>(entity.get()))
                    renderer.addPointLight(pointLight->position(), pointLight->color());
            }

            renderer.endFrame();
        }

        glfwDestroyWindow(window);
        glfwTerminate();
    }
    catch (const std::exception& e)
    {
        std::println(stderr, "{}", e.what());
        return 1;
    }
}
