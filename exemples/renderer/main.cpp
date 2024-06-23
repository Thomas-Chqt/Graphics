/*
 * ---------------------------------------------------
 * main.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/20 18:38:01
 * ---------------------------------------------------
 */

#include "Entity.hpp"
#include "Graphics/IndexBuffer.hpp"
#include "Graphics/KeyCodes.hpp"
#include "Graphics/Platform.hpp"
#include "Math/Constants.hpp"
#include "Math/Vector.hpp"
#include "RenderMethod.hpp"
#include "Renderer.hpp"
#include "ShaderDatas.hpp"
#include "UtilsCPP/Array.hpp"
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

    Camera camera;
    
    PointLight pointLight = {
        { 3.0, 2.5, 7.0 },  // position
        { 1.0, 1.0, 1.0 },  // color
        0.25,               // ambiantIntensity
        0.5,                // diffuseIntensity
        0.5,                // specularIntensity
    };

    RenderableEntity lightCube(*graphicAPI, RESSOURCES_DIR"/cube.obj");
    lightCube.scale = { 0.1, 0.1, 0.1 };
    lightCube.subMeshes[0].material = Material {
        { 0.0, 0.0, 0.0 },
        { 0.0, 0.0, 0.0 },
        { 0.0, 0.0, 0.0 },
        { 0.0, 0.0, 0.0 },
        0,
    };

    utils::Array<RenderableEntity> renderableEntites = {
        RenderableEntity(*graphicAPI, RESSOURCES_DIR"/cat.obj"),
        RenderableEntity(*graphicAPI, RESSOURCES_DIR"/cup.obj"),
    };

    renderableEntites[0].position = { 0.0, -1.5, 7.0 };
    renderableEntites[0].rotation = { 0.0, PI/2, 0.0 };

    renderableEntites[1].position = { 3.5, -1.5, 7.0 };

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

        if (ImGui::CollapsingHeader("Renderable Entities"))
        {
            for (auto& entt : renderableEntites)
            {
                if (ImGui::TreeNode(&entt, "Entity %p", &entt))
                {
                    ImGui::DragFloat3("position", (float*)&entt.position, 0.01);
                    ImGui::DragFloat3("rotation", (float*)&entt.rotation, 0.01);
                    if (ImGui::TreeNode("Sub meshes"))
                    {
                        for (auto& subMesh : entt.subMeshes)
                        {
                            if (ImGui::TreeNode(&subMesh, "Sub meshes %p", &subMesh))
                            {
                                ImGui::Text("vertex buffer: %p", (gfx::VertexBuffer*)subMesh.vertexBuffer);
                                ImGui::Text("index buffer: %p", (gfx::IndexBuffer*)subMesh.indexBuffer);
                                ImGui::Text("render method: %p", (RenderMethod*)subMesh.renderMethod);
                                if (ImGui::TreeNode("Material"))
                                {
                                    ImGui::ColorEdit3("ambiant", (float*)&subMesh.material.ambiant);
                                    ImGui::ColorEdit3("diffuse", (float*)&subMesh.material.diffuse);
                                    ImGui::ColorEdit3("specular", (float*)&subMesh.material.specular);
                                    ImGui::ColorEdit3("emissive", (float*)&subMesh.material.emissive);
                                    ImGui::DragFloat("shininess", (float*)&subMesh.material.shininess, 1, 1);
                                    ImGui::TreePop();
                                }
                                ImGui::Text("diffuse texture: %p", (gfx::Texture*)subMesh.diffuseTexture);
                                ImGui::TreePop();
                            }
                        }
                        ImGui::TreePop();
                    }
                    ImGui::TreePop();
                }
            }
            if (ImGui::TreeNode("light cube renderable"))
            {
                ImGui::Text("vertex buffer: %p", (gfx::VertexBuffer*)lightCube.subMeshes[0].vertexBuffer);
                ImGui::Text("index buffer: %p", (gfx::IndexBuffer*)lightCube.subMeshes[0].indexBuffer);
                ImGui::Text("render method: %p", (RenderMethod*)lightCube.subMeshes[0].renderMethod);
                if (ImGui::TreeNode("Material"))
                {
                    ImGui::ColorEdit3("ambiant", (float*)&lightCube.subMeshes[0].material.ambiant);
                    ImGui::ColorEdit3("diffuse", (float*)&lightCube.subMeshes[0].material.diffuse);
                    ImGui::ColorEdit3("specular", (float*)&lightCube.subMeshes[0].material.specular);
                    ImGui::ColorEdit3("emissive", (float*)&lightCube.subMeshes[0].material.emissive);
                    ImGui::DragFloat("shininess", (float*)&lightCube.subMeshes[0].material.shininess, 1, 1);
                    ImGui::TreePop();
                }
                ImGui::Text("diffuse texture: %p", (gfx::Texture*)lightCube.subMeshes[0].diffuseTexture);
                ImGui::TreePop();
            }

            ImGui::Spacing();
        }
        if (ImGui::CollapsingHeader("Lights"))
        {
            if (ImGui::TreeNode("Point light 1"))
            {
                ImGui::DragFloat3("position", (float*)&pointLight.position, 0.01);
                ImGui::ColorEdit3("color", (float*)&pointLight.color);
                ImGui::DragFloat("Light ambiant Intensity", (float*)&pointLight.ambiantIntensity, 0.001, 0.0, 1.0);
                ImGui::DragFloat("Light diffuse Intensity", (float*)&pointLight.diffuseIntensity, 0.001, 0.0, 1.0);
                ImGui::DragFloat("Light specular Intensity", (float*)&pointLight.specularIntensity, 0.001, 0.0, 1.0);
                ImGui::TreePop();
            }
        }

        lightCube.position = pointLight.position;
        lightCube.subMeshes[0].material.emissive = (pointLight.color * pointLight.ambiantIntensity) + (pointLight.color * pointLight.diffuseIntensity) + (pointLight.color * pointLight.specularIntensity);

        renderer.beginScene(camera);

        renderer.addPointLight(pointLight);

        for (auto& entt : renderableEntites)
            renderer.render(entt);
        renderer.render(lightCube);

        renderer.endScene();

        graphicAPI->endOnScreenRenderPass();
        graphicAPI->endFrame();
    }

    gfx::Platform::terminate();
}