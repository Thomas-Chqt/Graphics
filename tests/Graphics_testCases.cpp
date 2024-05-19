/*
 * ---------------------------------------------------
 * Graphics_testCases.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/15 13:53:46
 * ---------------------------------------------------
 */

#include <gtest/gtest.h>

#include "Graphics/Event.hpp"
#include "Graphics/Window.hpp"
#include "Logger/Logger.hpp"
#include "Graphics/Platform.hpp"
#include "UtilsCPP/SharedPtr.hpp"

namespace gfx_test
{

using namespace gfx;
using namespace tlog;
using namespace utils;

TEST(GraphicsTest, window)
{
    SharedPtr<Window> window = Platform::shared().newWindow(800, 600);
}

TEST(GraphicsTest, event)
{
    bool running = true;

    SharedPtr<Window> window = Platform::shared().newWindow(800, 600);

    Platform::shared().setEventCallBack([&](Event& ev) { logDebug << ev << std::endl; });

    window->setEventCallBack([&](Event& ev)
    {
        logDebug << ev << std::endl;

        ev.dispatch<KeyDownEvent>([&](KeyDownEvent& e)
        {
            if (e.keyCode() == 53)
                running = false;
        });
    });

    while (running)
        Platform::shared().pollEvents();
}

}