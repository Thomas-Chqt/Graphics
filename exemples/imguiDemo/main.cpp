/*
 * ---------------------------------------------------
 * main.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/07 15:01:58
 * ---------------------------------------------------
 */

#include "Graphics/Event.hpp"
#include "Graphics/KeyCodes.hpp"
#include "Graphics/Platform.hpp"
#include "Graphics/Window.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "imgui/imgui.h"

int main()
{
    gfx::Platform::init();

    utils::SharedPtr<gfx::Window> window = gfx::Platform::shared().newDefaultWindow(800, 600);
    utils::SharedPtr<gfx::GraphicAPI> graphicAPI = gfx::Platform::shared().newDefaultGraphicAPI(window);

    graphicAPI->useForImGui(
        ImGuiConfigFlags_NavEnableKeyboard |
        ImGuiConfigFlags_DockingEnable     |
        ImGuiConfigFlags_ViewportsEnable
    );

    bool running = true;

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
    });

    while (running)
    {
        gfx::Platform::shared().pollEvents();

        graphicAPI->beginFrame();

        ImGui::ShowDemoWindow();

        graphicAPI->endFrame();
    }

    gfx::Platform::terminate();
}