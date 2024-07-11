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

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

int main()
{
    gfx::Platform::init();
    {

        utils::SharedPtr<gfx::Window> window = gfx::Platform::shared().newWindow(WINDOW_WIDTH, WINDOW_HEIGHT);
        utils::SharedPtr<gfx::GraphicAPI> graphicAPI = gfx::Platform::shared().newGraphicAPI(window);

        graphicAPI->initImgui();

        bool running = true;

        window->addEventCallBack([&](gfx::Event& event) {
            event.dispatch<gfx::KeyDownEvent>([&](gfx::KeyDownEvent& event) {
                if (event.keyCode() == ESC_KEY)
                    running = false;
            });
        });

        while (running)
        {
            gfx::Platform::shared().pollEvents();

            graphicAPI->beginFrame();
            {
                graphicAPI->setClearColor(BLUE);
                
                graphicAPI->beginRenderPass();
                graphicAPI->endRenderPass();
                
                graphicAPI->beginImguiRenderPass();
                {
                    ImGui::ShowDemoWindow();
                }
                graphicAPI->endRenderPass();
            }
            graphicAPI->endFrame();
        }

        graphicAPI->terminateImGui();
    }
    gfx::Platform::terminate();
}
