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
#include "Renderer.hpp"
#include "ShaderDatas.hpp"
#include "TextureLibrary.hpp"
#include "UtilsCPP/Array.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "imgui/imgui.h"

void showImguiEdit(Material& material)
{
    // ImGui::ColorEdit3("diffuse", (float*)&subMesh.material.diffuse);

    math::rgb specularColor = material.specularColor();
    ImGui::ColorEdit3("specular", (float*)&specularColor);
    material.setSpecularColor(specularColor);

    math::rgb emissiveColor = material.emissiveColor();
    ImGui::ColorEdit3("emissive", (float*)&emissiveColor);
    material.setEmissiveColor(emissiveColor);

    float shininess = material.shininess();
    ImGui::DragFloat("shininess", (float*)&shininess, 1, 1);
    material.setShininess(shininess);
}

void showImguiEdit(SubMesh& subMesh)
{
    showImguiEdit(*subMesh.material);
}

void showImguiEdit(Mesh& mesh)
{    
    if (ImGui::TreeNode("Sub meshes"))
    {
        for (auto& subMesh : mesh.subMeshes)
        {
            showImguiEdit(subMesh);
        }
        ImGui::TreePop();
    }
}

void showImguiEdit(RenderableEntity& entt)
{
    ImGui::DragFloat3("position", (float*)&entt.position, 0.01);
    ImGui::DragFloat3("rotation", (float*)&entt.rotation, 0.01);
    showImguiEdit(entt.mesh);
}

void showImguiEdit(shaderData::PointLight& entt)
{
    ImGui::DragFloat3("position", (float*)&entt.position, 0.01);
}

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

    Camera camera;
    
    shaderData::PointLight pointLight = {
        { 3.0, 2.5, 7.0 },  // position
        { 1.0, 1.0, 1.0 },  // color
        0.25,               // ambiantIntensity
        0.5,                // diffuseIntensity
        0.5,                // specularIntensity
    };

    RenderableEntity lightCube(MeshLibrary::shared().meshFromFile(RESSOURCES_DIR"/cube.obj"));
    lightCube.scale = { 0.1, 0.1, 0.1 };
    lightCube.mesh.subMeshes[0].material = MaterialLibrary::shared().newEmptyMaterial();
    lightCube.mesh.subMeshes[0].material->setDiffuseColor(BLACK3);
    lightCube.mesh.subMeshes[0].material->setSpecularColor(BLACK3);
    lightCube.mesh.subMeshes[0].material->setEmissiveColor(WHITE3);
    lightCube.mesh.subMeshes[0].material->setShininess(0);
    

    utils::Array<RenderableEntity> renderableEntites;

    renderableEntites.append((RenderableEntity)(MeshLibrary::shared().meshFromFile(RESSOURCES_DIR"/cat.obj")));
    renderableEntites.last().position = { 0.0, -1.5, 7.0 };
    renderableEntites.last().rotation = { 0.0, PI/2, 0.0 };

    renderableEntites.append((RenderableEntity)(MeshLibrary::shared().meshFromFile(RESSOURCES_DIR"/cup.obj")));
    renderableEntites.last().position = { 3.5, -1.5, 7.0 };

    RenderableEntity* selectedEntt = nullptr;
    shaderData::PointLight* selectedPointLight = nullptr;
    
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
                for (auto& entt : renderableEntites)
                {
                    char buf[32];
                    sprintf(buf, "Entity %p", &entt);
                    if (ImGui::Selectable(buf, selectedEntt == &entt))
                        selectedEntt = &entt;
                }
            }
            ImGui::EndChild();
        }
        ImGui::SeparatorText("Point Lights");
        {
            if (ImGui::BeginChild("Point Lights", ImVec2(-FLT_MIN, ImGui::GetTextLineHeightWithSpacing() * 8), ImGuiChildFlags_Border | ImGuiChildFlags_ResizeY))
            {
                char buf[32];
                sprintf(buf, "Light %p", &pointLight);
                if (ImGui::Selectable(buf, selectedPointLight == &pointLight))
                    selectedPointLight = &pointLight;
            }
            ImGui::EndChild();
        }
        ImGui::SeparatorText("Selected Entity");
        {
            if(selectedEntt)
                showImguiEdit(*selectedEntt);
            else
                ImGui::Text("No entity selected");
        }
        ImGui::SeparatorText("Selected Point Light");
        {
            if(selectedPointLight)
                showImguiEdit(*selectedPointLight);
            else
                ImGui::Text("No light selected");
        }

        lightCube.position = pointLight.position;
        lightCube.mesh.subMeshes[0].material->setEmissiveColor((pointLight.color * pointLight.ambiantIntensity) + (pointLight.color * pointLight.diffuseIntensity) + (pointLight.color * pointLight.specularIntensity));

        renderer.beginScene(camera);

        renderer.addPointLight(pointLight);

        for (auto& entt : renderableEntites)
            renderer.render(entt);
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