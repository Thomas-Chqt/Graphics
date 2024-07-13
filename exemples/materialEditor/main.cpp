/*
 * ---------------------------------------------------
 * main.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/07/13 10:34:44
 * ---------------------------------------------------
 */

#include "DirectionalLight.hpp"
#include "Graphics/KeyCodes.hpp"
#include "Graphics/Platform.hpp"
#include "Graphics/Window.hpp"
#include "Material.hpp"
#include "Renderer.hpp"
#include "UtilsCPP/SharedPtr.hpp"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

int main()
{
    gfx::Platform::init();
    {
        utils::SharedPtr<gfx::Window> window = gfx::Platform::shared().newWindow(WINDOW_WIDTH, WINDOW_HEIGHT);
        utils::SharedPtr<gfx::GraphicAPI> graphicAPI = gfx::Platform::shared().newGraphicAPI(window);
        graphicAPI->initImgui();

        Renderer renderer(graphicAPI, window);

        Mesh sphere = loadMeshes(*graphicAPI, RESSOURCES_DIR"/sphere.glb")[0];
        Material material;
        DirectionalLight directionalLight;

        bool running = true;
        window->addEventCallBack([&](gfx::Event& event) {
            event.dispatch<gfx::KeyDownEvent>([&](gfx::KeyDownEvent& keyDownEvent) {
                if (keyDownEvent.keyCode() == ESC_KEY) running = false;
            });
        });

        while (running)
        {
            gfx::Platform::shared().pollEvents();
            graphicAPI->beginFrame();
            {
                graphicAPI->beginImguiRenderPass();
                {
                    if (ImGui::Begin("Imgui", nullptr, ImGuiWindowFlags_MenuBar))
                    {
                        if (ImGui::BeginMenuBar())
                        {
                            ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
                            ImGui::EndMenuBar();
                        }
                        ImGui::SeparatorText("Light");
                        {
                            ImGui::ColorEdit3("Color",            (float*)&directionalLight.color);
                            ImGui::DragFloat("ambiantIntensity",  (float*)&directionalLight.ambiantIntensity,  0.001, 0.0, 1.0);
                            ImGui::DragFloat("diffuseIntensity",  (float*)&directionalLight.diffuseIntensity,  0.001, 0.0, 1.0);
                            ImGui::DragFloat("specularIntensity", (float*)&directionalLight.specularIntensity, 0.001, 0.0, 1.0);
                        }
                        ImGui::SeparatorText("Material");
                        {
                            ImGui::ColorEdit3("Base color", (float*)&material.baseColor);
                        }
                    }
                    ImGui::End();

                    renderer.render(sphere, material, directionalLight);
                }
                graphicAPI->endRenderPass();
            }
            graphicAPI->endFrame();
        }
    }
    gfx::Platform::terminate();
    return 0;
}