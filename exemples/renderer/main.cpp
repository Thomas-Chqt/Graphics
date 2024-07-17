/*
 * ---------------------------------------------------
 * main.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/07/17 16:01:05
 * ---------------------------------------------------
 */

#include "Graphics/Event.hpp"
#include "Graphics/GraphicAPI.hpp"
#include "Graphics/KeyCodes.hpp"
#include "Graphics/Platform.hpp"
#include "Graphics/Window.hpp"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

int main()
{
    gfx::Platform::init();

    utils::SharedPtr<gfx::Window> window = gfx::Platform::shared().newWindow(WINDOW_WIDTH, WINDOW_HEIGHT);
    utils::SharedPtr<gfx::GraphicAPI> graphicAPI = gfx::Platform::shared().newGraphicAPI(window);
    graphicAPI->initImgui();

    bool running = true;
    window->addEventCallBack([&](gfx::Event& event) {
        event.dispatch<gfx::KeyDownEvent>([&](gfx::KeyDownEvent& keyDownEvent)
        {
            switch (keyDownEvent.keyCode())
            {
            case ESC_KEY:
                running = false;
                break;
            }
        });
    });
    
    while (running)
    {
        gfx::Platform::shared().pollEvents();
        graphicAPI->beginFrame();
        {
            graphicAPI->beginImguiRenderPass();
            {
            }
            graphicAPI->endRenderPass();
        }
        graphicAPI->endFrame();
    }

    gfx::Platform::terminate();
    return 0;
}