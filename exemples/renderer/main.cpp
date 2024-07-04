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
#include "Math/Constants.hpp"
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
    camera.position = { 0, 1, -4 };
    camera.rotation = { 0.2, 0, 0 };
    entities.append(&camera);
    
    PointLight pointLight;
    pointLight.name = "Light 1";
    pointLight.position = { -1.5, 1.5, -2.0 };
    pointLight.color = WHITE3;
    pointLight.ambiantIntensity = 0.1f;
    pointLight.diffuseIntensity = 0.5f;
    pointLight.specularIntensity = 0.5f;
    entities.append(&pointLight);

    PointLight pointLight2;
    pointLight2.name = "Light 2";
    pointLight2.position = { 1.5, 1.5, 0.5 };
    pointLight2.color = WHITE3;
    pointLight2.ambiantIntensity = 0.1f;
    pointLight2.diffuseIntensity = 0.5f;
    pointLight2.specularIntensity = 0.5f;
    entities.append(&pointLight2);

    RenderableEntity lightCube;
    lightCube.name = "lightCube1";
    lightCube.scale = { 0.1, 0.1, 0.1 };
    lightCube.mesh = AssetManager::shared().scene(RESSOURCES_DIR"/cube/cube.gltf")[0];
    lightCube.mesh.subMeshes[0].material = AssetManager::shared().material("lightCube1");
    lightCube.mesh.subMeshes[0].material->ambientColor = BLACK3;
    lightCube.mesh.subMeshes[0].material->diffuseColor = BLACK3;
    lightCube.mesh.subMeshes[0].material->specularColor = BLACK3;
    lightCube.mesh.subMeshes[0].material->shininess = 0.0f;
    entities.append(&lightCube);

    RenderableEntity lightCube2;
    lightCube2.name = "lightCube2";
    lightCube2.scale = { 0.1, 0.1, 0.1 };
    lightCube2.mesh = AssetManager::shared().scene(RESSOURCES_DIR"/cube/cube.gltf")[0];
    lightCube2.mesh.subMeshes[0].material = AssetManager::shared().material("lightCube2");
    lightCube2.mesh.subMeshes[0].material->ambientColor = BLACK3;
    lightCube2.mesh.subMeshes[0].material->diffuseColor = BLACK3;
    lightCube2.mesh.subMeshes[0].material->specularColor = BLACK3;
    lightCube2.mesh.subMeshes[0].material->shininess = 0.0f;
    entities.append(&lightCube2);

    RenderableEntity cat;
    cat.name = "cat";
    cat.position = { -0.7, 0, -0.7 };
    cat.rotation = { -PI/2, 2.5, 0.0 };
    cat.scale = {0.15, 0.15, 0.15};
    cat.mesh = AssetManager::shared().scene(RESSOURCES_DIR"/cat/cat.gltf")[0];
    entities.append(&cat);

    RenderableEntity cup;
    cup.name = "cup";
    cup.position = {0.7, -0.01, -0.6};
    cup.rotation = {-PI/2, 0, 0};
    cup.scale = {0.1, 0.1, 0.1};
    cup.mesh = AssetManager::shared().scene(RESSOURCES_DIR"/cup/cup.gltf")[0];
    entities.append(&cup);

    utils::Array<Mesh> potted_plant_scene = AssetManager::shared().scene(RESSOURCES_DIR"/potted_plant/potted_plant.gltf");
    utils::Array<RenderableEntity> potted_plant_entites;
    for (auto& mesh : potted_plant_scene) {
        RenderableEntity entt;
        entt.name = mesh.name;
        entt.mesh = mesh;
        potted_plant_entites.append(entt);
    }
    for (auto& entt : potted_plant_entites)
        entities.append(&entt);

    renderer.UI([&](){
        static Entity* selectedEntt = nullptr;
        static SubMesh* selectedSubMesh = nullptr;

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
                                selectedSubMesh = nullptr;
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
                        utils::Func<void(SubMesh&)> makeTreeNode = [&](SubMesh& subMesh){
                            ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
                                if (selectedSubMesh == &subMesh)
                                    node_flags |= ImGuiTreeNodeFlags_Selected;
                                if (subMesh.childs.isEmpty())
                                    node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
                                bool node_open = ImGui::TreeNodeEx(&subMesh, node_flags, "%s", subMesh.name.isEmpty() ? "No name" : (char*)subMesh.name);
                                if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
                                    selectedSubMesh = &subMesh;
                                if (subMesh.childs.isEmpty() == false && node_open)
                                {
                                    for (auto& child : subMesh.childs)
                                        makeTreeNode(child);
                                    ImGui::TreePop();
                                }
                        };
                        if(ImGui::TreeNode(renderableEntt->mesh.name))
                        {
                            for (auto& subMesh : renderableEntt->mesh.subMeshes)
                                makeTreeNode(subMesh);
                            ImGui::TreePop();
                        }
                    }
                }
            }

            ImGui::SeparatorText("Selected sub mesh");
            {
                if(selectedSubMesh == nullptr)
                    ImGui::Text("No sub mesh selected");
                else
                {
                    if (selectedSubMesh->material)
                    {
                        ImGui::Text("Matrial name: %s", (char*)selectedSubMesh->material->name);
                        ImGui::Text("ambientTexture: %p",  (void*)selectedSubMesh->material->ambientTexture);
                        ImGui::Text("diffuseTexture: %p",  (void*)selectedSubMesh->material->diffuseTexture);
                        ImGui::Text("specularTexture: %p", (void*)selectedSubMesh->material->specularTexture);
                        ImGui::Text("emissiveTexture: %p", (void*)selectedSubMesh->material->emissiveTexture);

                        ImGui::ColorEdit3("ambientColor" , (float*)&selectedSubMesh->material->ambientColor);
                        ImGui::ColorEdit3("diffuseColor" , (float*)&selectedSubMesh->material->diffuseColor);
                        ImGui::ColorEdit3("specularColor", (float*)&selectedSubMesh->material->specularColor);
                        ImGui::ColorEdit3("emissiveColor", (float*)&selectedSubMesh->material->emissiveColor);
                        
                        ImGui::DragFloat("shininess" ,     (float*)&selectedSubMesh->material->shininess, 1, 1);
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

        lightCube.position = pointLight.position;
        lightCube.mesh.subMeshes[0].material->emissiveColor  = pointLight.color  * pointLight.ambiantIntensity  + pointLight.color  * pointLight.diffuseIntensity  + pointLight.color  * pointLight.specularIntensity;
        
        lightCube2.position = pointLight2.position;
        lightCube2.mesh.subMeshes[0].material->emissiveColor = pointLight2.color * pointLight2.ambiantIntensity + pointLight2.color * pointLight2.diffuseIntensity + pointLight2.color * pointLight2.specularIntensity;
        
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