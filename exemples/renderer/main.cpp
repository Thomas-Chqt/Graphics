/*
 * ---------------------------------------------------
 * main.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/20 18:38:01
 * ---------------------------------------------------
 */

#include "Entity.hpp"
#include "Graphics/KeyCodes.hpp"
#include "Graphics/Platform.hpp"
#include "MaterialLibrary.hpp"
#include "Math/Vector.hpp"
#include "RenderMethod.hpp"
#include "Renderer.hpp"
#include "TextureLibrary.hpp"
#include "UtilsCPP/Array.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "imgui/imgui.h"
#include "imguiWidget.hpp"
#include "LoadFile.hpp"

int main()
{
    gfx::Platform::init();

    bool running = true;
    gfx::Platform::shared().addEventCallBack([&](gfx::Event& event) { event.dispatch<gfx::KeyDownEvent>([&](gfx::KeyDownEvent& event) { if (event.keyCode() == ESC_KEY) running = false; }); });
    
    utils::SharedPtr<gfx::Window> window = gfx::Platform::shared().newDefaultWindow(800, 600);
    utils::SharedPtr<gfx::GraphicAPI> graphicAPI = gfx::Platform::shared().newDefaultGraphicAPI(window);

    #ifdef GFX_METAL_ENABLED
        graphicAPI->initMetalShaderLib(MTL_SHADER_LIB);
    #endif    
    graphicAPI->useForImGui(ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable);

    TextureLibrary::init(graphicAPI);
    MaterialLibrary::init(graphicAPI);

    Renderer renderer(window, graphicAPI);
    utils::Array<Entity*> entities;

    Camera camera;
    camera.name = "camera";
    camera.position = { 8, 2, -70 };
    entities.append(&camera);
    
    PointLight pointLight;
    pointLight.name = "Light 1";
    pointLight.position = { 20, 15, 7 };
    pointLight.color = WHITE3;
    pointLight.ambiantIntensity = 0.1f;
    pointLight.diffuseIntensity = 0.5f;
    pointLight.specularIntensity = 0.5f;
    entities.append(&pointLight);

    PointLight pointLight2;
    pointLight2.name = "Light 2";
    pointLight2.position = { 10, 15, -36 };
    pointLight2.color = WHITE3;
    pointLight2.ambiantIntensity = 0.1f;
    pointLight2.diffuseIntensity = 0.5f;
    pointLight2.specularIntensity = 0.5f;
    entities.append(&pointLight2);

    utils::Array<RenderableEntity> after_the_rain = loadMesh(*graphicAPI, RESSOURCES_DIR"/after_the_rain/scene.gltf");
    for (auto& entt : after_the_rain)
        entities.append(&entt);

    Entity* selectedEntt = nullptr;

    while (running)
    {
        gfx::Platform::shared().pollEvents();

     
        if (window->isKeyPress(W_KEY))     camera.position   += camera.forward() * 0.2;
        if (window->isKeyPress(A_KEY))     camera.position   -= camera.right()   * 0.2;
        if (window->isKeyPress(S_KEY))     camera.position   -= camera.forward() * 0.2;
        if (window->isKeyPress(D_KEY))     camera.position   += camera.right()   * 0.2;
        if (window->isKeyPress(UP_KEY))    camera.rotation.x -= 0.05;
        if (window->isKeyPress(LEFT_KEY))  camera.rotation.y -= 0.05;
        if (window->isKeyPress(DOWN_KEY))  camera.rotation.x += 0.05;
        if (window->isKeyPress(RIGHT_KEY)) camera.rotation.y += 0.05;
        
        renderer.beginScene();
        
        renderer.setImgui([&](){
            if (ImGui::Begin("Imgui", NULL, ImGuiWindowFlags_MenuBar))
            {
                if (ImGui::BeginMenuBar())
                {
                    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
                    ImGui::EndMenuBar();
                }

                ImGui::SeparatorText("Entities");
                {
                    if (ImGui::BeginChild("Entities", ImVec2(-FLT_MIN, ImGui::GetTextLineHeightWithSpacing() * 8), ImGuiChildFlags_Border | ImGuiChildFlags_ResizeY))
                    {
                        for (auto& entt : entities)
                        {
                            if (entt->parentEntity == nullptr)
                                enttSelect(*entt, selectedEntt);
                        }
                    }
                    ImGui::EndChild();
                }
                
                ImGui::SeparatorText("Selected Entity");
                {
                    if(selectedEntt)
                        editWidget(*selectedEntt);
                    else
                        ImGui::Text("No entity selected");
                }

                ImGui::End();
            }
        });

        renderer.setCamera(camera);

        for (Entity* entt : entities)
        {
            if(PointLight* pLight = dynamic_cast<PointLight*>(entt))
                renderer.addPointLight(*pLight); 
            else if (RenderableEntity* rEntt = dynamic_cast<RenderableEntity*>(entt))
            {
                if (rEntt->parentEntity == nullptr)
                    renderer.addRenderableEntity(*rEntt);
            }
        }

        renderer.endScene();
    }

    MaterialLibrary::terminate();
    TextureLibrary::terminate();
    
    gfx::Platform::terminate();
}