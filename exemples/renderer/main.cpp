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
    
    RenderableEntity cat(*graphicAPI, RESSOURCES_DIR"/cat.obj");
    cat.position = { 0.0, -1.5, 7.0 };
    cat.rotation = { 0.0, PI/2, 0.0 };

    RenderableEntity cup(*graphicAPI, RESSOURCES_DIR"/cup.obj");
    cup.position = { 3.5, -1.5, 7.0 };

    PointLight pointLight = {
        { 3.0, 2.5, 7.0 },  // position
        { 1.0, 1.0, 1.0 },  // color
        0.25,               // ambiantIntensity
        0.5,                // diffuseIntensity
        0.5,                // specularIntensity
    };

    RenderableEntity lightCube(*graphicAPI, RESSOURCES_DIR"/cube.obj");
    lightCube.scale = { 0.1, 0.1, 0.1 };
    lightCube.subMeshes[0].renderMethod = utils::SharedPtr<RenderMethod>(new LightCubeRenderMethod(*graphicAPI));

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
            if (ImGui::TreeNode("Cat"))
            {
                ImGui::DragFloat3("position", (float*)&cat.position, 0.01);
                ImGui::DragFloat3("rotation", (float*)&cat.rotation, 0.01);
                if (ImGui::TreeNode("Sub meshes"))
                {
                    int i = 0;
                    for (auto& subMesh : cat.subMeshes)
                    {
                        if (ImGui::TreeNode(&subMesh, "Sub meshes %d", i))
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
                        i++;
                    }
                    ImGui::TreePop();
                }
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("Cup"))
            {
                ImGui::DragFloat3("position", (float*)&cup.position, 0.01);
                ImGui::DragFloat3("rotation", (float*)&cup.rotation, 0.01);
                if (ImGui::TreeNode("Sub meshes"))
                {
                    int i = 0;
                    for (auto& subMesh : cup.subMeshes)
                    {
                        if (ImGui::TreeNode(&subMesh, "Sub meshes %d", i))
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
                        i++;
                    }
                    ImGui::TreePop();
                }
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

        renderer.render(cat);
        renderer.render(cup);
        renderer.render(lightCube);

        renderer.endScene();

        graphicAPI->endOnScreenRenderPass();
        graphicAPI->endFrame();
    }

    gfx::Platform::terminate();
}