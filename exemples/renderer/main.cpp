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
    MaterialLibrary::init();
    MeshLibrary::init(graphicAPI);

    Renderer renderer(window, graphicAPI);
    utils::Array<Entity*> entities;
    utils::Array<RenderableEntity*> renderableEntites;

    Camera camera;
    entities.append(&camera);
    
    PointLight pointLight;
    pointLight.position = { 3.0, 2.5, 7.0 };
    pointLight.color = WHITE3;
    pointLight.ambiantIntensity = 0.25f;
    pointLight.diffuseIntensity = 0.5f;
    pointLight.specularIntensity = 0.5f;
    entities.append(&pointLight);

    RenderableEntity lightCube(MeshLibrary::shared().meshFromFile(RESSOURCES_DIR"/cube.obj"));
    lightCube.scale = { 0.1, 0.1, 0.1 };
    lightCube.mesh.subMeshes[0].renderMethod = utils::SharedPtr<IRenderMethod>(new RenderMethod<Shader::universal3D, Shader::baseColor>(graphicAPI));
    lightCube.mesh.subMeshes[0].material = MaterialLibrary::shared().newEmptyMaterial();
    lightCube.mesh.subMeshes[0].material->baseColor = BLACK3;
    lightCube.mesh.subMeshes[0].material->specularColor = BLACK3;
    lightCube.mesh.subMeshes[0].material->emissiveColor = WHITE3;
    lightCube.mesh.subMeshes[0].material->shininess = 0.0f;
    renderableEntites.append(&lightCube);

    RenderableEntity cat(MeshLibrary::shared().meshFromFile(RESSOURCES_DIR"/cat.obj"));
    cat.position = { 0.0, -1.5, 7.0 };
    cat.rotation = { 0.0, PI/2, 0.0 };
    entities.append(&cat);
    renderableEntites.append(&cat);

    RenderableEntity cup(MeshLibrary::shared().meshFromFile(RESSOURCES_DIR"/cup.obj"));
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
                    char buf[32];
                    sprintf(buf, "Entity %p", entt);
                    if (ImGui::Selectable(buf, selectedEntt == entt))
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

        renderer.beginScene(camera);

        renderer.addPointLight(pointLight);

        for (auto& entt : renderableEntites)
            renderer.render(*entt);
        renderer.render(lightCube);

        renderer.endScene();

        graphicAPI->endOnScreenRenderPass();
        graphicAPI->endFrame();
    }

    MeshLibrary::terminate();
    MaterialLibrary::terminate();
    TextureLibrary::terminate();
    
    gfx::Platform::terminate();
}