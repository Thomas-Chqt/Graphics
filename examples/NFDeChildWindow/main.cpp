/*
 * ---------------------------------------------------
 * main.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2024/11/15 16:26:55
 * ---------------------------------------------------
 */

#include "Graphics/Event.hpp"
#include "Graphics/KeyCodes.hpp"
#include "Graphics/Platform.hpp"
#include "nfd.hpp"
// #include <future>

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
            event.dispatch<gfx::WindowRequestCloseEvent>([&](gfx::WindowRequestCloseEvent& event) {
                running = false;
            });
        });

        std::string textContent;
        // std::future<std::pair<nfdresult_t, NFD::UniquePath>> fut;
        // bool isPresented = false;

        while (running)
        {
            gfx::Platform::shared().pollEvents();

            graphicAPI->beginFrame();
            {   
                graphicAPI->beginRenderPass();
                graphicAPI->endRenderPass();
                
                graphicAPI->beginImguiRenderPass();
                {
                    ImGui::Text("%s", textContent.c_str());

                    if (ImGui::Button("select"))
                    {
                        // if (isPresented == false)
                        // {
                            nfdwindowhandle_t handle = window->getNFDwindowHandle();
                        //     fut = std::async([handle](){
                                std::pair<nfdresult_t, NFD::UniquePath> result;
                                result.first = NFD::OpenDialog(result.second, nullptr, 0, nullptr, handle);
                            //     return result;
                            // });
                            // isPresented = true;
                    //     }
                    // }

                    // if (isPresented)
                    // {
                        // if (fut.wait_for(std::chrono::nanoseconds(1)) == std::future_status::ready)
                        // {
                            // std::pair<nfdresult_t, NFD::UniquePath> result = fut.get();
                            if (result.first == NFD_OKAY)
                                textContent = result.second.get();
                        //     isPresented = false;   
                        // }
                    }
                }
                graphicAPI->endRenderPass();
            }
            graphicAPI->endFrame();
        }
    }
    gfx::Platform::terminate();
}