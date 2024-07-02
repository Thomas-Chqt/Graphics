/*
 * ---------------------------------------------------
 * main.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/20 18:38:01
 * ---------------------------------------------------
 */

#include "AssetManager.hpp"
#include "Entity.hpp"
#include "Graphics/KeyCodes.hpp"
#include "Graphics/Platform.hpp"
#include "Math/Vector.hpp"
#include "RenderMethod.hpp"
#include "Renderer.hpp"
#include "UtilsCPP/Array.hpp"
#include "UtilsCPP/Func.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "imgui/imgui.h"

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

    Renderer renderer(window, graphicAPI);
    utils::Array<Entity*> entities;

    AssetManager::init(graphicAPI);

    Camera camera;
    camera.name = "camera";
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

    RenderableEntity city;
    city.name = "City";
    city.mesh = AssetManager::shared().mesh(RESSOURCES_DIR"/after_the_rain/scene.gltf");
    entities.append(&city);

    RenderableEntity cat;
    cat.name = "cat";
    cat.mesh = AssetManager::shared().mesh(RESSOURCES_DIR"/cat/cat.obj");
    entities.append(&cat);

    renderer.UI([&](){
        static Entity* selectedEntt = nullptr;
        static Mesh* selectedMesh = nullptr;

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
                    for (auto& entt : entities) {
                        if (ImGui::Selectable(entt->name.isEmpty() ? "No name" : entt->name, selectedEntt == entt))
                        {
                            if (selectedEntt != entt)
                                selectedMesh = nullptr;
                            selectedEntt = entt;
                        }
                    }
                }
                ImGui::EndChild();
            }
            
            ImGui::SeparatorText("Selected Entity");
            {
                if(selectedEntt == nullptr)
                    ImGui::Text("No entity selected");
                else
                {
                    ImGui::Text("Name: %s", (char*)selectedEntt->name);
                    ImGui::DragFloat3("position", (float*)&selectedEntt->position, 0.01);
                    ImGui::DragFloat3("rotation", (float*)&selectedEntt->rotation, 0.01);
                    ImGui::DragFloat3("scale",    (float*)&selectedEntt->scale,    0.01);
                    ImGui::Spacing();
                    if (PointLight* pointLight = dynamic_cast<PointLight*>(selectedEntt))
                    {
                        ImGui::ColorEdit3("color",            (float*)&pointLight->color);
                        ImGui::DragFloat("ambiantIntensity",  (float*)&pointLight->ambiantIntensity,  0.001, 0.0, 1.0);
                        ImGui::DragFloat("diffuseIntensity",  (float*)&pointLight->diffuseIntensity,  0.001, 0.0, 1.0);
                        ImGui::DragFloat("specularIntensity", (float*)&pointLight->specularIntensity, 0.001, 0.0, 1.0);
                    }
                    if (RenderableEntity* renderableEntt = dynamic_cast<RenderableEntity*>(selectedEntt))
                    {
                        utils::Func<void(Mesh&)> makeTreeNode = [&](Mesh& mesh){
                            ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
                                if (selectedMesh == &mesh)
                                    node_flags |= ImGuiTreeNodeFlags_Selected;
                                if (mesh.childs.isEmpty())
                                    node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
                                bool node_open = ImGui::TreeNodeEx(&mesh, node_flags, "%s", mesh.name.isEmpty() ? "No name" : (char*)mesh.name);
                                if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
                                    selectedMesh = &mesh;
                                if (mesh.childs.isEmpty() == false && node_open)
                                {
                                    for (auto& child : mesh.childs)
                                        makeTreeNode(child);
                                    ImGui::TreePop();
                                }
                        };
                        makeTreeNode(renderableEntt->mesh);
                    }
                }
            }

            ImGui::SeparatorText("Selected Mesh");
            {
                if(selectedMesh == nullptr)
                    ImGui::Text("No mesh selected");
                else
                {
                    if (selectedMesh->material)
                    {
                        ImGui::Text("renderMethod: %p",    (void*)selectedMesh->material->renderMethod);
                        ImGui::Text("baseTexture: %p",     (void*)selectedMesh->material->baseTexture);
                        ImGui::Text("specularTexture: %p", (void*)selectedMesh->material->specularTexture);
                        ImGui::Text("emissiveTexture: %p", (void*)selectedMesh->material->emissiveTexture);

                        ImGui::ColorEdit3("baseColor" ,    (float*)&selectedMesh->material->baseColor);
                        ImGui::ColorEdit3("specularColor", (float*)&selectedMesh->material->specularColor);
                        ImGui::ColorEdit3("emissiveColor", (float*)&selectedMesh->material->emissiveColor);
                        ImGui::DragFloat("shininess" ,     (float*)&selectedMesh->material->shininess, 1, 1);
                    }
                }
            }
        }
        ImGui::End();
    });
    
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
        
        renderer.setCamera(camera);

        for (Entity* entt : entities)
        {
            if(PointLight* pLight = dynamic_cast<PointLight*>(entt))
                renderer.addPointLight(*pLight); 
            else if (RenderableEntity* rEntt = dynamic_cast<RenderableEntity*>(entt))
                renderer.addMesh(rEntt->mesh, rEntt->modelMatrix());
        }

        renderer.endScene();
    }

    AssetManager::terminate();
    gfx::Platform::terminate();
}