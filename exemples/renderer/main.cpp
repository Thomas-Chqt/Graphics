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
#include "Math/Constants.hpp"
#include "Math/Vector.hpp"
#include "MeshLibrary.hpp"
#include "RenderMethod.hpp"
#include "Renderer.hpp"
#include "TextureLibrary.hpp"
#include "UtilsCPP/Array.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "imgui/imgui.h"
#include "imguiWidget.hpp"

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
    MeshLibrary::init(graphicAPI);

    Renderer renderer(window, graphicAPI);
    utils::Array<Entity*> entities;
    utils::Array<RenderableEntity*> renderableEntites;

    Camera camera;
    camera.name = "camera";
    entities.append(&camera);
    
    PointLight pointLight;
    pointLight.name = "Light 1";
    pointLight.position = { 3.5, 2.5, 7.0 };;
    pointLight.color = WHITE3;
    pointLight.ambiantIntensity = 0.1f;
    pointLight.diffuseIntensity = 0.5f;
    pointLight.specularIntensity = 0.5f;
    entities.append(&pointLight);

    PointLight pointLight2;
    pointLight2.name = "Light 2";
    pointLight2.position = { 0.0, 2.5, 7.0 };
    pointLight2.color = WHITE3;
    pointLight2.ambiantIntensity = 0.1f;
    pointLight2.diffuseIntensity = 0.5f;
    pointLight2.specularIntensity = 0.5f;
    entities.append(&pointLight2);

    RenderableEntity lightCube(MeshLibrary::shared().meshFromFile(RESSOURCES_DIR"/cube.obj"));
    lightCube.scale = { 0.1, 0.1, 0.1 };
    lightCube.mesh.subMeshes[0].material = MaterialLibrary::shared().newEmptyMaterial();
    lightCube.mesh.subMeshes[0].material->baseColor = BLACK3;
    lightCube.mesh.subMeshes[0].material->specularColor = BLACK3;
    lightCube.mesh.subMeshes[0].material->shininess = 0.0f;
    renderableEntites.append(&lightCube);

    RenderableEntity lightCube2(MeshLibrary::shared().meshFromFile(RESSOURCES_DIR"/cube.obj"));
    lightCube2.scale = { 0.1, 0.1, 0.1 };
    lightCube2.mesh.subMeshes[0].material = MaterialLibrary::shared().newEmptyMaterial();
    lightCube2.mesh.subMeshes[0].material->baseColor = BLACK3;
    lightCube2.mesh.subMeshes[0].material->specularColor = BLACK3;
    lightCube2.mesh.subMeshes[0].material->shininess = 0.0f;
    renderableEntites.append(&lightCube2);

    RenderableEntity cat(MeshLibrary::shared().meshFromFile(RESSOURCES_DIR"/cat.obj"));
    cat.name = "cat";
    cat.position = { 0.0, -1.5, 7.0 };
    cat.rotation = { 0.0, PI/2, 0.0 };
    entities.append(&cat);
    renderableEntites.append(&cat);

    RenderableEntity cup(MeshLibrary::shared().meshFromFile(RESSOURCES_DIR"/cup.obj"));
    cup.name = "cup";
    cup.position = { 3.5, -1.5, 7.0 };
    entities.append(&cup);
    renderableEntites.append(&cup);

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

        graphicAPI->beginFrame();
        graphicAPI->beginOnScreenRenderPass();

        ImGui::SeparatorText("Entities");
        {
            if (ImGui::BeginChild("Entities", ImVec2(-FLT_MIN, ImGui::GetTextLineHeightWithSpacing() * 8), ImGuiChildFlags_Border | ImGuiChildFlags_ResizeY))
            {
                for (auto& entt : entities)
                {
                    if (ImGui::Selectable(entt->name.isEmpty() ? "No name" : entt->name, selectedEntt == entt))
                        selectedEntt = entt;
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

        lightCube.position = pointLight.position;
        lightCube.mesh.subMeshes[0].material->emissiveColor = (pointLight.color * pointLight.ambiantIntensity) + (pointLight.color * pointLight.diffuseIntensity) + (pointLight.color * pointLight.specularIntensity);

        lightCube2.position = pointLight2.position;
        lightCube2.mesh.subMeshes[0].material->emissiveColor = (pointLight2.color * pointLight2.ambiantIntensity) + (pointLight2.color * pointLight2.diffuseIntensity) + (pointLight2.color * pointLight2.specularIntensity);

        renderer.beginScene(camera);

        renderer.addPointLight(pointLight);
        renderer.addPointLight(pointLight2);

        for (auto& entt : renderableEntites)
            renderer.render(*entt);

        renderer.endScene();

        graphicAPI->endOnScreenRenderPass();
        graphicAPI->endFrame();
    }

    MeshLibrary::terminate();
    MaterialLibrary::terminate();
    TextureLibrary::terminate();
    
    gfx::Platform::terminate();
}