/*
 * ---------------------------------------------------
 * main.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/07 13:34:53
 * ---------------------------------------------------
 */

#include "Graphics/Event.hpp"
#include "Graphics/GraphicAPI.hpp"
#include "Graphics/KeyCodes.hpp"
#include "Graphics/Platform.hpp"
#include "Graphics/Window.hpp"
#include "Math/Vector.hpp"
#include "UtilsCPP/SharedPtr.hpp"

int main()
{
    gfx::Platform::init();

    utils::SharedPtr<gfx::Window> window = gfx::Platform::shared().newDefaultWindow(800, 600);
    utils::SharedPtr<gfx::GraphicAPI> graphicAPI = gfx::Platform::shared().newDefaultGraphicAPI(window);

    bool running = true;
    math::rgba clearColor = BLACK;

    window->setEventCallBack([&](gfx::Event& event)
    {
        event.dispatch<gfx::KeyDownEvent>([&](gfx::KeyDownEvent& event)
        {
            switch (event.keyCode())
            {
            case ESC_KEY:
                running = false;
            }
        });

        event.dispatch<gfx::MouseDownEvent>([&](gfx::MouseDownEvent& event)
        {
            switch (event.mouseCode())
            {
            case MOUSE_L:
                clearColor = GREEN;
                break;
            case MOUSE_R:
                clearColor = RED;
                break;
            }
        });

        event.dispatch<gfx::MouseUpEvent>([&](gfx::MouseUpEvent& event)
        {
            clearColor = BLACK;
        });
    });

    while (running)
    {
        gfx::Platform::shared().pollEvents();

        gfx::RenderPassDescriptor renderPassDescriptor;
        renderPassDescriptor.clearColor = clearColor;

        graphicAPI->beginFrame(renderPassDescriptor);
        graphicAPI->endFrame();
    }

    gfx::Platform::terminate();
}