/*
 * ---------------------------------------------------
 * main.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/07/13 10:34:44
 * ---------------------------------------------------
 */

#include "Graphics/KeyCodes.hpp"
#include "Graphics/Platform.hpp"
#include "Graphics/Window.hpp"
#include "Renderer.hpp"
#include "UtilsCPP/SharedPtr.hpp"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

int main()
{
    gfx::Platform::init();
    {
        utils::SharedPtr<gfx::Window> window = gfx::Platform::shared().newWindow(WINDOW_WIDTH, WINDOW_HEIGHT);
        utils::SharedPtr<gfx::GraphicAPI> graphicAPI = gfx::Platform::shared().newGraphicAPI(window);
        graphicAPI->initImgui();

        Renderer renderer(graphicAPI, window);

        Mesh sphere = loadMeshes(*graphicAPI, RESSOURCES_DIR"/sphere.glb")[0];

        bool running = true;
        window->addEventCallBack([&](gfx::Event& event) {
            event.dispatch<gfx::KeyDownEvent>([&](gfx::KeyDownEvent& keyDownEvent) {
                if (keyDownEvent.keyCode() == ESC_KEY) running = false;
            });
        });

        while (running)
        {
            gfx::Platform::shared().pollEvents();
            graphicAPI->beginFrame();
            {
                graphicAPI->beginRenderPass();
                {
                    renderer.render(sphere);
                }
                graphicAPI->endRenderPass();
            }
            graphicAPI->endFrame();
        }
    }
    gfx::Platform::terminate();
    return 0;
}