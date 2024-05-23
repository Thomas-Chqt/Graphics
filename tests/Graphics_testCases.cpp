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
#include "Graphics/IndexBuffer.hpp"
#include "Graphics/VertexBuffer.hpp"
#include "Graphics/Window.hpp"
#include "Logger/Logger.hpp"
#include "Graphics/Platform.hpp"
#include "UtilsCPP/Array.hpp"
#include "UtilsCPP/Func.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "Graphics/GraphicAPI.hpp"
#include "Graphics/KeyCodes.hpp"
#include "UtilsCPP/Types.hpp"
#include "Vertex.hpp"
#ifdef IMGUI_ENABLED
    #include "imgui/imgui.h"
#endif

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

    #ifdef USING_OPENGL
    private:
        utils::Array<Element> m_elements;
    #endif
    };
}

namespace gfx_test
{

using namespace gfx;
using namespace tlog;
using namespace utils;


#if defined (USING_METAL) && defined (USING_OPENGL)
    class GraphicsTestWindowMetal : public ::testing::Test
    {
        public: GraphicsTestWindowMetal() : m_window(Platform::shared().newMetalWindow(800, 600)) {}
        protected: SharedPtr<Window> m_window;
    };

    class GraphicsTestWindowOpenGL : public ::testing::Test
    {
        public: GraphicsTestWindowOpenGL() : m_window(Platform::shared().newOpenGLWindow(800, 600)) {}
        protected: SharedPtr<Window> m_window;
    };
#else
    class GraphicsTestWindow : public ::testing::Test
    {
        public: GraphicsTestWindow() : m_window(Platform::shared().newDefaultWindow(800, 600)) {}
        protected: SharedPtr<Window> m_window;
    };
#endif

#if defined (USING_METAL) && defined (USING_OPENGL)
    class GraphicsTestAPIMetal : public GraphicsTestWindowMetal
    {
        public: GraphicsTestAPIMetal() : GraphicsTestWindowMetal(), m_graphicAPI(Platform::shared().newMetalGraphicAPI(m_window)) {}
        protected: SharedPtr<GraphicAPI> m_graphicAPI;
    };

    class GraphicsTestAPIOpenGL : public GraphicsTestWindowOpenGL
    {
        public: GraphicsTestAPIOpenGL() : GraphicsTestWindowOpenGL(), m_graphicAPI(Platform::shared().newOpenGLGraphicAPI(m_window)) {}
        protected: SharedPtr<GraphicAPI> m_graphicAPI;
    };
#else
    class GraphicsTestAPI : public GraphicsTestWindow
    {
        public: GraphicsTestAPI() : GraphicsTestWindow(), m_graphicAPI(Platform::shared().newDefaultGraphicAPI(m_window)) {}
        protected: SharedPtr<GraphicAPI> m_graphicAPI;
    };
#endif

#if defined (USING_METAL) && defined (USING_OPENGL)
    TEST(GraphicsTest, windowMetal)
    {
        SharedPtr<Window> window = Platform::shared().newMetalWindow(800, 600);
    }

    TEST(GraphicsTest, windowOpenGL)
    {
        SharedPtr<Window> window = Platform::shared().newOpenGLWindow(800, 600);
    }
#else
    TEST(GraphicsTest, window)
    {
        SharedPtr<Window> window = Platform::shared().newDefaultWindow(800, 600);
    }
#endif

#if defined (USING_METAL) && defined (USING_OPENGL)
    TEST_F(GraphicsTestWindowMetal, event)
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

    TEST_F(GraphicsTestWindowOpenGL, event)
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
#else
    TEST_F(GraphicsTestWindow, event)
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
#endif

#if defined (USING_METAL) && defined (USING_OPENGL)
    TEST_F(GraphicsTestWindowMetal, graphicAPI)
    {
        SharedPtr<GraphicAPI> graphicAPI = Platform::shared().newMetalGraphicAPI(m_window);

        graphicAPI->beginFrame();
        graphicAPI->endFrame();
    }

    TEST_F(GraphicsTestWindowOpenGL, graphicAPI)
    {
        SharedPtr<GraphicAPI> graphicAPI = Platform::shared().newOpenGLGraphicAPI(m_window);

        graphicAPI->beginFrame();
        graphicAPI->endFrame();
    }
#else
    TEST_F(GraphicsTestWindow, graphicAPI)
    {
        SharedPtr<GraphicAPI> graphicAPI = Platform::shared().newDefaultGraphicAPI(m_window);

        graphicAPI->beginFrame();
        graphicAPI->endFrame();
    }
#endif

#if defined (USING_METAL) && defined (USING_OPENGL)
    TEST_F(GraphicsTestAPIMetal, setClearColor)
    {
        bool running = true;

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
                    m_graphicAPI->setClearColor(1, 0, 0, 1);
                if (e.mouseCode() == MOUSE_R)
                    m_graphicAPI->setClearColor(0, 1, 0, 1);
            });

            ev.dispatch<MouseUpEvent>([&](MouseUpEvent& e)
            {
                m_graphicAPI->setClearColor(0, 0, 0, 0);
            });
        });

        while (running)
        {
            Platform::shared().pollEvents();
            m_graphicAPI->beginFrame();
            m_graphicAPI->endFrame();
        }
    }

    TEST_F(GraphicsTestAPIOpenGL, setClearColor)
    {
        bool running = true;

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
                    m_graphicAPI->setClearColor(1, 0, 0, 1);
                if (e.mouseCode() == MOUSE_R)
                    m_graphicAPI->setClearColor(0, 1, 0, 1);
            });

            ev.dispatch<MouseUpEvent>([&](MouseUpEvent& e)
            {
                m_graphicAPI->setClearColor(0, 0, 0, 0);
            });
        });

        while (running)
        {
            Platform::shared().pollEvents();
            m_graphicAPI->beginFrame();
            m_graphicAPI->endFrame();
        }
    }
#else
    TEST_F(GraphicsTestAPI, setClearColor)
    {
        bool running = true;

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
                    m_graphicAPI->setClearColor(1, 0, 0, 1);
                if (e.mouseCode() == MOUSE_R)
                    m_graphicAPI->setClearColor(0, 1, 0, 1);
            });

            ev.dispatch<MouseUpEvent>([&](MouseUpEvent& e)
            {
                m_graphicAPI->setClearColor(0, 0, 0, 0);
            });
        });

        while (running)
        {
            Platform::shared().pollEvents();
            m_graphicAPI->beginFrame();
            m_graphicAPI->endFrame();
        }
    }
#endif

#if defined (USING_METAL) && defined (USING_OPENGL)
TEST_F(GraphicsTestAPIMetal, vertexBuffer)
    {
        Array<Vertex> vertices;

        vertices.append({-1, -1});
        vertices.append({ 1,  1});
        vertices.append({ 1,  0});

        SharedPtr<VertexBuffer> vertexBuffer = m_graphicAPI->newVertexBuffer(vertices);
    }

    TEST_F(GraphicsTestAPIOpenGL, vertexBuffer)
    {
        Array<Vertex> vertices;

        vertices.append({-1, -1});
        vertices.append({ 1,  1});
        vertices.append({ 1,  0});

        SharedPtr<VertexBuffer> vertexBuffer = m_graphicAPI->newVertexBuffer(vertices);
    }
#else
    TEST_F(GraphicsTestAPI, vertexBuffer)
    {
        Array<Vertex> vertices;

        vertices.append({-1, -1});
        vertices.append({ 1,  1});
        vertices.append({ 1,  0});

        SharedPtr<VertexBuffer> vertexBuffer = m_graphicAPI->newVertexBuffer(vertices);
    }
#endif

#if defined (USING_METAL) && defined (USING_OPENGL)
    TEST_F(GraphicsTestAPIMetal, graphicPipeline)
    {
        SharedPtr<GraphicPipeline> graphicPipeline = m_graphicAPI->newGraphicsPipeline("vtx1", "fra1");
    }

    TEST_F(GraphicsTestAPIOpenGL, graphicPipeline)
    {
        SharedPtr<GraphicPipeline> graphicPipeline = m_graphicAPI->newGraphicsPipeline("vtx1", "fra1");
    }
#else
    TEST_F(GraphicsTestAPI, graphicPipeline)
    {
        SharedPtr<GraphicPipeline> graphicPipeline = m_graphicAPI->newGraphicsPipeline("vtx1", "fra1");
    }
#endif

#if defined (USING_METAL) && defined (USING_OPENGL)
    TEST_F(GraphicsTestAPIMetal, triangle)
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
    TEST_F(GraphicsTestAPIOpenGL, triangle)
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
#else
    TEST_F(GraphicsTestAPI, triangle)
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
#endif

#if defined (USING_METAL) && defined (USING_OPENGL)
    TEST_F(GraphicsTestAPIMetal, indexedShape)
    {
        bool running = true;

        Array<Vertex> vertices = {
            {-0.5, -0.5},
            {-0.5,  0.5},
            { 0.5,  0.5},
            { 0.5, -0.5}
        };

        Array<uint32> indices = { 0, 1, 2, 0, 2, 3 };

        SharedPtr<GraphicPipeline> graphicPipeline = m_graphicAPI->newGraphicsPipeline("vtx1", "fra1");
        SharedPtr<VertexBuffer> vertexBuffer = m_graphicAPI->newVertexBuffer(vertices);
        SharedPtr<IndexBuffer> indexBuffer = m_graphicAPI->newIndexBuffer(indices);

        Platform::shared().setEventCallBack([&](Event& ev)
        {
            ev.dispatch<KeyDownEvent>([&](KeyDownEvent& e)
            {
                switch (e.keyCode())
                {
                case ESC_KEY:
                    running = false;
                    break;
                }
            });
        });

        while (running)
        {
            Platform::shared().pollEvents();
            
            m_graphicAPI->beginFrame();
            
            m_graphicAPI->useGraphicsPipeline(graphicPipeline);
            m_graphicAPI->useVertexBuffer(vertexBuffer);
            m_graphicAPI->drawIndexedVertices(indexBuffer);

            m_graphicAPI->endFrame();
        }
    }
    TEST_F(GraphicsTestAPIOpenGL, indexedShape)
    {
        bool running = true;

        Array<Vertex> vertices = {
            {-0.5, -0.5},
            {-0.5,  0.5},
            { 0.5,  0.5},
            { 0.5, -0.5}
        };

        Array<uint32> indices = { 0, 1, 2, 0, 2, 3 };

        SharedPtr<GraphicPipeline> graphicPipeline = m_graphicAPI->newGraphicsPipeline("vtx1", "fra1");
        SharedPtr<VertexBuffer> vertexBuffer = m_graphicAPI->newVertexBuffer(vertices);
        SharedPtr<IndexBuffer> indexBuffer = m_graphicAPI->newIndexBuffer(indices);

        Platform::shared().setEventCallBack([&](Event& ev)
        {
            ev.dispatch<KeyDownEvent>([&](KeyDownEvent& e)
            {
                switch (e.keyCode())
                {
                case ESC_KEY:
                    running = false;
                    break;
                }
            });
        });

        while (running)
        {
            Platform::shared().pollEvents();
            
            m_graphicAPI->beginFrame();
            
            m_graphicAPI->useGraphicsPipeline(graphicPipeline);
            m_graphicAPI->useVertexBuffer(vertexBuffer);
            m_graphicAPI->drawIndexedVertices(indexBuffer);

            m_graphicAPI->endFrame();
        }
    }
#else
    TEST_F(GraphicsTestAPI, indexedShape)
    {
        bool running = true;

        Array<Vertex> vertices = {
            {-0.5, -0.5},
            {-0.5,  0.5},
            { 0.5,  0.5},
            { 0.5, -0.5}
        };

        Array<uint32> indices = { 0, 1, 2, 0, 2, 3 };

        SharedPtr<GraphicPipeline> graphicPipeline = m_graphicAPI->newGraphicsPipeline("vtx1", "fra1");
        SharedPtr<VertexBuffer> vertexBuffer = m_graphicAPI->newVertexBuffer(vertices);
        SharedPtr<IndexBuffer> indexBuffer = m_graphicAPI->newIndexBuffer(indices);

        Platform::shared().setEventCallBack([&](Event& ev)
        {
            ev.dispatch<KeyDownEvent>([&](KeyDownEvent& e)
            {
                switch (e.keyCode())
                {
                case ESC_KEY:
                    running = false;
                    break;
                }
            });
        });

        while (running)
        {
            Platform::shared().pollEvents();
            
            m_graphicAPI->beginFrame();
            
            m_graphicAPI->useGraphicsPipeline(graphicPipeline);
            m_graphicAPI->useVertexBuffer(vertexBuffer);
            m_graphicAPI->drawIndexedVertices(indexBuffer);

            m_graphicAPI->endFrame();
        }
    }
#endif



#ifdef IMGUI_ENABLED
    #if defined (USING_METAL) && defined (USING_OPENGL)
        TEST_F(GraphicsTestAPIMetal, imguiDemoWindow)
        {
            bool running = true;

            m_graphicAPI->useForImGui();

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
                ImGui::NewFrame();

                ImGui::ShowDemoWindow();

                ImGui::Render();
                m_graphicAPI->endFrame();

                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault();
            }
        }
        TEST_F(GraphicsTestAPIOpenGL, imguiDemoWindow)
        {
            bool running = true;

            m_graphicAPI->useForImGui();

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
                ImGui::NewFrame();

                ImGui::ShowDemoWindow();

                ImGui::Render();
                m_graphicAPI->endFrame();

                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault();
            }
        }
    #else
        TEST_F(GraphicsTestAPI, imguiDemoWindow)
        {
            bool running = true;

            m_graphicAPI->useForImGui();

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
                ImGui::NewFrame();

                ImGui::ShowDemoWindow();

                ImGui::Render();
                m_graphicAPI->endFrame();

                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault();
            }
        }
    #endif
#endif

}
