/*
 * ---------------------------------------------------
 * main.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/07/06 17:45:26
 * ---------------------------------------------------
 */

#include "Graphics/Event.hpp"
#include "Graphics/GraphicAPI.hpp"
#include "Graphics/Platform.hpp"
#include "Graphics/Window.hpp"
#include "Mesh.hpp"
#include "Renderer.hpp"
#include "UtilsCPP/Array.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "Graphics/KeyCodes.hpp"
#include "DirectionalLight.hpp"

int main()
{
    gfx::Platform::init();

    utils::SharedPtr<gfx::Window> window = gfx::Platform::shared().newDefaultWindow(800, 600);
    utils::SharedPtr<gfx::GraphicAPI> graphicAPI = gfx::Platform::shared().newDefaultGraphicAPI(window);
    Renderer renderer(graphicAPI, window);

    bool running = true;
    window->addEventCallBack([&](gfx::Event& event){ event.dispatch<gfx::KeyDownEvent>([&](gfx::KeyDownEvent& keyDownEvent){ if (keyDownEvent.keyCode() == ESC_KEY) running = false; }); });

    Mesh sphere = loadMeshes(*graphicAPI, RESSOURCES_DIR"/sphere.glb")[0];
    DirectionalLight light;
    Material material;

    while (running)
    {
        gfx::Platform::shared().pollEvents();

        graphicAPI->beginFrame();
        graphicAPI->beginOnScreenRenderPass();

        renderer.render(sphere, light, material);

        graphicAPI->endOnScreenRenderPass();
        graphicAPI->endFrame();
    }

    gfx::Platform::terminate();
}