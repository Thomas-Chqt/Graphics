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
#include "UtilsCPP/Func.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "Graphics/GraphicAPI.hpp"
#include "Graphics/KeyCodes.hpp"

namespace gfx_test
{

using namespace gfx;
using namespace tlog;
using namespace utils;

class GraphicsWindowedTest : public ::testing::Test
{
public:
    GraphicsWindowedTest() : m_window(Platform::shared().newWindow(800, 600))
    {
    }
    
    ~GraphicsWindowedTest()
    {
    }

protected:
    SharedPtr<Window> m_window;
};

TEST(GraphicsTest, window)
{
    SharedPtr<Window> window = Platform::shared().newWindow(800, 600);
}

TEST(GraphicsTest, event)
{
    bool running = true;

    Platform::shared().setEventCallBack([&](Event& ev)
    {
        logDebug << ev << std::endl;

        ev.dispatch<KeyDownEvent>([&](KeyDownEvent& e)
        {
            if (e.keyCode() == ESC_KEY)
                running = false;
        });
    });

    SharedPtr<Window> window = Platform::shared().newWindow(800, 600);

    while (running)
        Platform::shared().pollEvents();
}

#ifdef USING_OPENGL
TEST_F(GraphicsWindowedTest, openglAPI)
{
    SharedPtr<GraphicAPI> graphicAPI = GraphicAPI::newOpenGLGraphicAPI(m_window);

    graphicAPI->beginFrame();
    graphicAPI->endFrame();
}
#endif

#ifdef USING_METAL
TEST_F(GraphicsWindowedTest, metalAPI)
{
    SharedPtr<GraphicAPI> graphicAPI = GraphicAPI::newMetalGraphicAPI(m_window);

    graphicAPI->beginFrame();
    graphicAPI->endFrame();
}
#endif

#ifdef USING_OPENGL
TEST_F(GraphicsWindowedTest, setClearColorOpenGL)
{
    bool running = true;
    SharedPtr<GraphicAPI> graphicAPI = GraphicAPI::newOpenGLGraphicAPI(m_window);

    Func<void(Event& ev)> callback = [&](Event& ev)
    {
        ev.dispatch<KeyDownEvent>([&](KeyDownEvent& e)
        {
            if (e.keyCode() == ESC_KEY)
                running = false;
        });

        ev.dispatch<MouseDownEvent>([&](MouseDownEvent& e)
        {
            if (e.mouseCode() == MOUSE_L)
                graphicAPI->setClearColor(1, 0, 0, 1);
            if (e.mouseCode() == MOUSE_R)
                graphicAPI->setClearColor(0, 1, 0, 1);
        });

        ev.dispatch<MouseUpEvent>([&](MouseUpEvent& e)
        {
            graphicAPI->setClearColor(0, 0, 0, 0);
        });
    };

    m_window->setEventCallBack(callback);

    while (running)
    {
        Platform::shared().pollEvents();
        graphicAPI->beginFrame();
        graphicAPI->endFrame();
    }
}
#endif

#ifdef USING_METAL
TEST_F(GraphicsWindowedTest, setClearColorMetal)
{
    bool running = true;
    SharedPtr<GraphicAPI> graphicAPI = GraphicAPI::newMetalGraphicAPI(m_window);

    Func<void(Event& ev)> callback = [&](Event& ev)
    {
        ev.dispatch<KeyDownEvent>([&](KeyDownEvent& e)
        {
            if (e.keyCode() == ESC_KEY)
                running = false;
        });

        ev.dispatch<MouseDownEvent>([&](MouseDownEvent& e)
        {
            if (e.mouseCode() == MOUSE_L)
                graphicAPI->setClearColor(1, 0, 0, 1);
            if (e.mouseCode() == MOUSE_R)
                graphicAPI->setClearColor(0, 1, 0, 1);
        });

        ev.dispatch<MouseUpEvent>([&](MouseUpEvent& e)
        {
            graphicAPI->setClearColor(0, 0, 0, 0);
        });
    };

    m_window->setEventCallBack(callback);

    while (running)
    {
        Platform::shared().pollEvents();
        graphicAPI->beginFrame();
        graphicAPI->endFrame();
    }
}
#endif

TEST_F(GraphicsWindowedTest, windowRecreation)
{
    bool running = true;
    SharedPtr<GraphicAPI> graphicAPI = GraphicAPI::newDefaultGraphicAPI(m_window);

    Platform::shared().setEventCallBack([&](Event& ev)
    {
        ev.dispatch<KeyDownEvent>([&](KeyDownEvent& e)
        {
            if (e.keyCode() == ESC_KEY)
                running = false;
            if (e.keyCode() == SPACE_KEY)
            {
               m_window = Platform::shared().newWindow(800, 600);
               graphicAPI->setRenderTarget(m_window);
            }
        });

        ev.dispatch<MouseDownEvent>([&](MouseDownEvent& e)
        {
            if (e.mouseCode() == MOUSE_L)
                graphicAPI->setClearColor(1, 0, 0, 1);
            if (e.mouseCode() == MOUSE_R)
                graphicAPI->setClearColor(0, 1, 0, 1);
        });

        ev.dispatch<MouseUpEvent>([&](MouseUpEvent& e)
        {
            graphicAPI->setClearColor(0, 0, 0, 0);
        });
    });

    while (running)
    {
        Platform::shared().pollEvents();
        graphicAPI->beginFrame();
        graphicAPI->endFrame();
    }
}

TEST(GraphicsTests, doubleWindowSameAPI)
{
    bool running = true;

    SharedPtr<GraphicAPI> graphicAPI1 = GraphicAPI::newDefaultGraphicAPI(Platform::shared().newWindow(800, 600));
    SharedPtr<GraphicAPI> graphicAPI2 = GraphicAPI::newDefaultGraphicAPI(Platform::shared().newWindow(800, 600));

    Platform::shared().setEventCallBack([&](Event& ev)
    {
        ev.dispatch<KeyDownEvent>([&](KeyDownEvent& e)
        {
            if (e.keyCode() == ESC_KEY)
                running = false;
        });

        ev.dispatch<MouseDownEvent>([&](MouseDownEvent& e)
        {
            if (e.mouseCode() == MOUSE_L)
            {
                graphicAPI1->setClearColor(1, 0, 0, 1);
                graphicAPI2->setClearColor(1, 0, 0, 1);
            }
            if (e.mouseCode() == MOUSE_R)
            {
                graphicAPI1->setClearColor(0, 1, 0, 1);
                graphicAPI2->setClearColor(0, 1, 0, 1);
            }
        });

        ev.dispatch<MouseUpEvent>([&](MouseUpEvent& e)
        {
            graphicAPI1->setClearColor(0, 0, 0, 0);
            graphicAPI2->setClearColor(0, 0, 0, 0);
        });
    });

    while (running)
    {
        Platform::shared().pollEvents();
        
        graphicAPI1->beginFrame();
        graphicAPI1->endFrame();

        graphicAPI2->beginFrame();
        graphicAPI2->endFrame();
    }
}

#if defined (USING_OPENGL) && defined (USING_METAL)
TEST(GraphicsTests, doubleWindowDifferentAPI)
{
    bool running = true;

    SharedPtr<GraphicAPI> graphicAPI1 = GraphicAPI::newOpenGLGraphicAPI(Platform::shared().newWindow(800, 600));
    SharedPtr<GraphicAPI> graphicAPI2 = GraphicAPI::newMetalGraphicAPI(Platform::shared().newWindow(800, 600));

    Platform::shared().setEventCallBack([&](Event& ev)
    {
        ev.dispatch<KeyDownEvent>([&](KeyDownEvent& e)
        {
            if (e.keyCode() == ESC_KEY)
                running = false;
        });

        ev.dispatch<MouseDownEvent>([&](MouseDownEvent& e)
        {
            if (e.mouseCode() == MOUSE_L)
            {
                graphicAPI1->setClearColor(1, 0, 0, 1);
                graphicAPI2->setClearColor(1, 0, 0, 1);
            }
            if (e.mouseCode() == MOUSE_R)
            {
                graphicAPI1->setClearColor(0, 1, 0, 1);
                graphicAPI2->setClearColor(0, 1, 0, 1);
            }
        });

        ev.dispatch<MouseUpEvent>([&](MouseUpEvent& e)
        {
            graphicAPI1->setClearColor(0, 0, 0, 0);
            graphicAPI2->setClearColor(0, 0, 0, 0);
        });
    });

    while (running)
    {
        Platform::shared().pollEvents();
        
        graphicAPI1->beginFrame();
        graphicAPI1->endFrame();

        graphicAPI2->beginFrame();
        graphicAPI2->endFrame();
    }
}
#endif

}