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
#include "Graphics/GraphicPipeline.hpp"
#include "Graphics/VertexBuffer.hpp"
#include "Graphics/Window.hpp"
#include "Logger/Logger.hpp"
#include "Graphics/Platform.hpp"
#include "UtilsCPP/Array.hpp"
#include "UtilsCPP/Func.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "Graphics/GraphicAPI.hpp"
#include "Graphics/KeyCodes.hpp"
#include "Vertex.hpp"

namespace gfx
{
    template<>
    class VertexBuffer::Layout<gfx_test::Vertex> : public VertexBuffer::LayoutBase
    {
    public:
        #ifdef USING_OPENGL
        Layout()
        {
            m_elements.append({
                .size = 2,
                .type = GL_FLOAT,
                .normalized = GL_FALSE,
                .stride = sizeof(gfx_test::Vertex),
                .pointer = (void*)0
            });
        }
        inline const utils::Array<Element>& getElements() const override { return m_elements; };
        #endif
        inline utils::uint64 getSize() const override { return sizeof(gfx_test::Vertex); };

    private:
        utils::Array<Element> m_elements;
    };
}

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

class GraphicsAPIedTest : public GraphicsWindowedTest
{
public:
    GraphicsAPIedTest() : GraphicsWindowedTest(), m_graphicAPI(GraphicAPI::newDefaultGraphicAPI(m_window))
    {
    }

    ~GraphicsAPIedTest()
    {

    }
protected:
    SharedPtr<GraphicAPI> m_graphicAPI;
};

TEST(GraphicsTest, window)
{
    SharedPtr<Window> window = Platform::shared().newWindow(800, 600);
}

TEST_F(GraphicsWindowedTest, event)
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

TEST_F(GraphicsAPIedTest, vertexBuffer)
{
    Array<Vertex> vertices;

    vertices.append({-1, -1});
    vertices.append({ 1,  1});
    vertices.append({ 1,  0});

    SharedPtr<VertexBuffer> vertexBuffer = m_graphicAPI->newVertexBuffer(vertices);
}

TEST_F(GraphicsAPIedTest, graphicPipeline)
{
    SharedPtr<GraphicPipeline> graphicPipeline = m_graphicAPI->newGraphicsPipeline("vtx1", "fra1");
}

TEST_F(GraphicsAPIedTest, triangle)
{
    bool running = true;

    Array<Vertex> vertices;

    vertices.append({-1, -1});
    vertices.append({ 0,  1});
    vertices.append({ 1, -1});

    SharedPtr<VertexBuffer> vertexBuffer = m_graphicAPI->newVertexBuffer(vertices);
    SharedPtr<GraphicPipeline> graphicPipeline = m_graphicAPI->newGraphicsPipeline("vtx1", "fra1");

    Platform::shared().setEventCallBack([&](Event& ev)
    {
        ev.dispatch<KeyDownEvent>([&](KeyDownEvent& e)
        {
            if (e.keyCode() == ESC_KEY)
                running = false;
        });
    });


    while (running)
    {
        Platform::shared().pollEvents();
        
        m_graphicAPI->beginFrame();
        
        m_graphicAPI->useVertexBuffer(vertexBuffer);
        m_graphicAPI->useGraphicsPipeline(graphicPipeline);
        m_graphicAPI->drawVertices(0, 3);

        m_graphicAPI->endFrame();
    }
}

TEST_F(GraphicsWindowedTest, triangleAPISwichable)
{
    bool running = true;
    bool usingMetal = true;

    Array<Vertex> vertices;

    vertices.append({-1, -1});
    vertices.append({ 0,  1});
    vertices.append({ 1, -1});

    while (running)
    {
        bool looping = true;

        SharedPtr<GraphicAPI> graphicAPI = usingMetal ? GraphicAPI::newMetalGraphicAPI(m_window) : GraphicAPI::newOpenGLGraphicAPI(m_window);

        SharedPtr<VertexBuffer> vertexBuffer = graphicAPI->newVertexBuffer(vertices);
        SharedPtr<GraphicPipeline> graphicPipeline = graphicAPI->newGraphicsPipeline("vtx1", "fra1");

        Platform::shared().setEventCallBack([&](Event& ev)
        {
            ev.dispatch<KeyDownEvent>([&](KeyDownEvent& e)
            {
                switch (e.keyCode())
                {
                case ESC_KEY:
                    looping = false;
                    running = false;
                    break;
                case ONE_KEY:
                    usingMetal = true;
                    looping = false;
                    logDebug << "Swiching to Metal" << std::endl;
                    break;
                case TWO_KEY:
                    usingMetal = false;
                    looping = false;
                    logDebug << "Swiching to OpenGL" << std::endl;
                    break;
                }
            });
        });

        while (looping)
        {
            Platform::shared().pollEvents();
            
            graphicAPI->beginFrame();
            
            graphicAPI->useVertexBuffer(vertexBuffer);
            graphicAPI->useGraphicsPipeline(graphicPipeline);
            graphicAPI->drawVertices(0, 3);

            graphicAPI->endFrame();
        }
    }
}

}
