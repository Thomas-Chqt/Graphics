/*
 * ---------------------------------------------------
 * scop.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/09/16 07:05:29
 * ---------------------------------------------------
 */

#include "Material.hpp"
#include "Mesh.hpp"
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
#if !defined(SCOP_MANDATORY)
    #include <imgui.h>
    #include <glm/glm.hpp>
#else
    #include "math/math.hpp"
    #ifndef SCOP_MATH_GLM_ALIAS_DEFINED
    #define SCOP_MATH_GLM_ALIAS_DEFINED
    namespace glm = scop::math;
    #endif
#endif

#include <cassert>
#include <algorithm> // IWYU pragma: keep
#include <filesystem>
#include <memory>
#include <numbers> // IWYU pragma: keep
#include <print>
#include <cstdio>
#include <exception>
#include <numbers>
#include <bit> // IWYU pragma: keep
#include <set>
#include <vector>
#include <future> // IWYU pragma: keep
#include <string_view>
#include <stdexcept>
#include <string>
#include <functional>

#if __XCODE__
    #include <unistd.h>
#endif

#if !defined (SCOP_MANDATORY)
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
#endif

constexpr uint32_t WINDOW_WIDTH = 800;
constexpr uint32_t WINDOW_HEIGHT = 600;

#if defined (SCOP_MANDATORY)
constexpr double TEXTURE_TRANSITION_DURATION = 0.5;
#endif

namespace fs = std::filesystem;

int main(int argc, char** argv)
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

#if defined (SCOP_MANDATORY)
        std::shared_ptr<scop::Camera> camera = std::make_shared<scop::FixedCamera>();
#else
        std::shared_ptr<scop::Camera> camera = std::make_shared<scop::FlightCamera>();
#endif

#if defined (SCOP_MANDATORY)
        auto parameterBlockPool = device->newParameterBlockPool(gfx::ParameterBlockPool::Descriptor{ .maxUniformBuffers=1, .maxTextures=0, .maxSamplers=0 });
        auto commandBufferPool = device->newCommandBufferPool();
        auto scopMaterial = std::make_shared<scop::ScopMaterial>(*device);
        auto commandBuffer = commandBufferPool->get();
        commandBuffer->beginBlitPass();
        scopMaterial->setDiffuseTexture(assetLoader.loadTexture(RESOURCE_DIR"/kittens.png", *commandBuffer));
        commandBuffer->endBlitPass();
        device->submitCommandBuffers(commandBuffer);
        scopMaterial->makeParameterBlock(*parameterBlockPool);
#endif

        std::vector<std::shared_ptr<scop::Entity>> entities;

        auto light = std::make_shared<scop::PointLight>();
        light->setName("Point Light");
        light->setColor(glm::vec3(1.0f, 1.0f, 1.0f) * 0.8f);
        entities.push_back(light);

        fs::path meshPath;
        if (argc >= 2 && argv[1] != nullptr && std::string_view(argv[1]).empty() == false)
        {
            if (fs::exists(argv[1]))
                meshPath = argv[1];
            else if (fs::exists(fs::path(RESOURCE_DIR) / argv[1]))
                meshPath = fs::path(RESOURCE_DIR) / argv[1];
        }
        else {
#if defined (SCOP_MANDATORY)
            throw std::invalid_argument("please provide a 3D file (ex: 42.obj)");
#else
            throw std::invalid_argument("please provide a 3D file (ex: bistro.glb)");
#endif
        }

#if defined (SCOP_MANDATORY)
        auto object = std::make_shared<scop::RenderableEntity>(std::async(std::launch::async, [&assetLoader, scopMaterial, meshPath]() {
            return assetLoader.loadMesh(meshPath, scopMaterial);
        }));
#else
        auto object = std::make_shared<scop::RenderableEntity>(std::async(std::launch::async, [&assetLoader, meshPath]() {
            return assetLoader.loadMesh(meshPath);
        }));
#endif
        object->setName("mesh");
        object->setPosition(glm::vec3{0, 0, -3});
        if (meshPath.filename() == "bistro.glb")
            object->setRotation({std::numbers::pi_v<float> / 2, 0.0f, 0.0f});
        if (meshPath.filename() == "after_the_rain.glb")
            object->setRotation({-std::numbers::pi_v<float> / 2, 0.0f, 0.0f});
        if (meshPath.filename() == "neighbourhood_city.glb")
            object->setRotation({-std::numbers::pi_v<float> / 2, 0.0f, 0.0f});
        if (meshPath.filename() == "sponza.glb")
            object->setRotation({-std::numbers::pi_v<float> / 2, 0.0f, 0.0f});
        entities.push_back(object);

        while (true)
        {
            glfwPollEvents();
            if (glfwWindowShouldClose(window))
                break;

            static double lastFrameTime = glfwGetTime();
            double currentFrameTime = glfwGetTime();
            double deltaTime = currentFrameTime - lastFrameTime;
            lastFrameTime = currentFrameTime;

            camera->update(pressedKeys, deltaTime);

#if defined (SCOP_MANDATORY)
            static bool objectScaled = false;
            if (object->mesh().has_value() && objectScaled == false)
            {
                const scop::Mesh& mesh = *object->mesh();
                const glm::vec3 size = mesh.bBoxMax - mesh.bBoxMin;
                const float longestSide = std::max({size.x, size.y, size.z});
                if (longestSide > std::numeric_limits<float>::epsilon())
                    object->setScale((1.0f / longestSide) * 2.0f);
                objectScaled = true;
            }

            object->setRotation(object->rotation() + glm::vec3(0.0f, 1.0f, 0.0f) * float(deltaTime) * 0.75f);

            static float textureStrengthStart = scopMaterial->textureStrength();
            static float textureStrengthTarget = scopMaterial->textureStrength();
            static double textureTransitionElapsed = TEXTURE_TRANSITION_DURATION;

            static bool spacePressed = false;
            if (pressedKeys.contains(GLFW_KEY_SPACE)) {
                if (spacePressed == false) {
                    static bool textureEnabled = scopMaterial->textureStrength() >= 0.5f;
                    textureEnabled = !textureEnabled;
                    textureStrengthStart = scopMaterial->textureStrength();
                    textureStrengthTarget = textureEnabled ? 1.0f : 0.0f;
                    textureTransitionElapsed = 0.0;
                }
                spacePressed = true;
            }
            else {
                spacePressed = false;
            }

            if (textureTransitionElapsed < TEXTURE_TRANSITION_DURATION)
            {
                textureTransitionElapsed += deltaTime;
                const float t = static_cast<float>(std::clamp(textureTransitionElapsed / TEXTURE_TRANSITION_DURATION, 0.0, 1.0));
                scopMaterial->setTextureStrength(textureStrengthStart + (textureStrengthTarget - textureStrengthStart) * t);
            }
            else if (scopMaterial->textureStrength() != textureStrengthTarget)
            {
                scopMaterial->setTextureStrength(textureStrengthTarget);
            }
#else
            static scop::Light* lightAttachedToCamera = nullptr;
            if (lightAttachedToCamera != nullptr)
                lightAttachedToCamera->setPosition(camera->position());
#endif

            renderer.beginFrame(camera->viewMatrix(), camera->fov(), camera->nearPlane(), camera->farPlane());

#if !defined (SCOP_MANDATORY)
            static scop::Entity* selectedEntity = nullptr;
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
                        if (renderableEntity->mesh().has_value())
                        {
                            std::function<void(const scop::SubMesh& mesh)> bfs = [&](const scop::SubMesh& mesh){
                                if (std::shared_ptr<scop::ScopMaterial> scopMaterial = std::dynamic_pointer_cast<scop::ScopMaterial>(mesh.material)) {
                                    ImGui::PushID(&mesh);
                                    for (int i = 0; i < 6; i++) {
                                        ImGui::PushID(i);
                                        glm::vec4 color = scopMaterial->paletteColor(i);
                                        ImGui::ColorEdit4("##colorPalette", std::bit_cast<float*>(&color), ImGuiColorEditFlags_NoInputs);
                                        if (i != 5)
                                            ImGui::SameLine();
                                        scopMaterial->setPaletteColor(i, color);
                                        ImGui::PopID();
                                    }
                                    float textureStrength = scopMaterial->textureStrength();
                                    ImGui::SliderFloat("texture strength", &textureStrength, 0, 1);
                                    scopMaterial->setTextureStrength(textureStrength);
                                    ImGui::PopID();
                                }
                                for (const auto& subMesh : mesh.subMeshes)
                                    bfs(subMesh);
                            };
                            for (const auto& subMesh : renderableEntity->mesh()->subMeshes)
                                bfs(subMesh);
                        }
                    }

                    if (auto* light = dynamic_cast<scop::Light*>(selectedEntity))
                    {
                        glm::vec3 lightColor = light->color();
                        ImGui::ColorEdit3("light color", std::bit_cast<float*>(&lightColor));
                        light->setColor(lightColor);
                        bool attachedToCamera = lightAttachedToCamera == light;
                        ImGui::Checkbox("attach to camera", &attachedToCamera);
                        if(attachedToCamera)
                            lightAttachedToCamera = light;
                        else
                            lightAttachedToCamera = nullptr;
                    }
                }
                else
                {
                    ImGui::Text("no entity selected"); // NOLINT(cppcoreguidelines-pro-type-vararg)
                }
            }
            ImGui::End();
#endif

            renderer.setAmbientLightColor(glm::vec3(1.0f, 1.0f, 1.0f) * 0.1f);

            for (auto& entity : entities)
            {
                if (auto* renderableEntity = dynamic_cast<scop::RenderableEntity*>(entity.get())) {
                    if (renderableEntity->mesh().has_value())
                        renderer.addMesh(*renderableEntity->mesh(), renderableEntity->modelMatrix());
                }

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
