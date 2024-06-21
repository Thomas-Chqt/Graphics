/*
 * ---------------------------------------------------
 * main.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/20 18:38:01
 * ---------------------------------------------------
 */

#include "AssetLoad.hpp"
#include "Entity.hpp"
#include "Graphics/KeyCodes.hpp"
#include "Graphics/Platform.hpp"
#include "Math/Constants.hpp"
#include "Math/Vector.hpp"
#include "Renderer.hpp"
#include "ShaderDatas.hpp"

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
    RenderableEntity cat = loadModel(graphicAPI, RESSOURCES_DIR"/cat.obj");
    DirectionalLight light;
    light.color = {1,1,1};
    light.ambiantIntensity = 0.25;
    light.diffuseIntensity = 1;
    light.specularIntensity = 1;

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

        ImGui::SliderFloat("Cat pos X", &cat.position.x, -2, 2);
        ImGui::SliderFloat("Cat pos Y", &cat.position.y, -2, 2);
        ImGui::SliderFloat("Cat pos Z", &cat.position.z, -2, 2);
        ImGui::SliderFloat("Cat rot X", &cat.rotation.x, 0, 2 * PI);
        ImGui::SliderFloat("Cat rot Y", &cat.rotation.y, 0, 2 * PI);
        ImGui::SliderFloat("Cat rot Z", &cat.rotation.z, 0, 2 * PI);

        ImGui::ColorPicker4("Light color", (float*)&light.color);
        ImGui::SliderFloat("Light direction X", &light.direction.x, 0, 2 * PI);
        ImGui::SliderFloat("Light direction Y", &light.direction.y, 0, 2 * PI);
        ImGui::SliderFloat("Light ambiantIntensity",  &light.ambiantIntensity,  0, 2);
        ImGui::SliderFloat("Light diffuseIntensity",  &light.diffuseIntensity,  0, 2);
        ImGui::SliderFloat("Light specularIntensity", &light.specularIntensity, 0, 2);

        renderer.beginScene(camera);
        renderer.setDirectionalLight(light);
        renderer.render(cat);
        renderer.endScene();

        graphicAPI->endOnScreenRenderPass();
        graphicAPI->endFrame();
    }

    gfx::Platform::terminate();
}