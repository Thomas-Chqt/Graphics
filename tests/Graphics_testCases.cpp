/*
 * ---------------------------------------------------
 * Graphics_testCases.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/15 13:53:46
 * ---------------------------------------------------
 */

#include <gtest/gtest.h>

#include "test_fixitures.hpp"
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

namespace gfx_test
{

using namespace gfx;
using namespace tlog;
using namespace utils;

MULTI_TEST(GraphicsTestWindow, simple,
{
})

MULTI_TEST(GraphicsTestWindow, event,
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
})

MULTI_TEST(GraphicsTestAPI, simple,
{
    m_graphicAPI->beginFrame();
    m_graphicAPI->endFrame();
})

MULTI_TEST(GraphicsTestAPI, setClearColor,
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
})

MULTI_TEST(GraphicsTestAPI, vertexBuffer,
{
    Array<Vertex> vertices;

    vertices.append({-1, -1});
    vertices.append({ 1,  1});
    vertices.append({ 1,  0});

    SharedPtr<VertexBuffer> vertexBuffer = m_graphicAPI->newVertexBuffer(vertices);
})

MULTI_TEST(GraphicsTestAPI, graphicPipeline,
{
    SharedPtr<GraphicPipeline> graphicPipeline = m_graphicAPI->newGraphicsPipeline("vtx1", "fra1");
})

MULTI_TEST(GraphicsTestAPI, triangle,
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
})

MULTI_TEST(GraphicsTestAPI, indexedShape,
{
    bool running = true;

    Array<Vertex> vertices = Array<Vertex>({
        {-0.5, -0.5},
        {-0.5,  0.5},
        { 0.5,  0.5},
        { 0.5, -0.5}
    });
    Array<uint32> indices = Array<uint32>({ 0, 1, 2, 0, 2, 3 });

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
})

#if defined (USING_METAL) && defined (USING_OPENGL)
TEST(GraphicsTest, APISwitch)
{
    bool usingMetal = true;
    bool running = true;
    bool looping = true;

    Platform::shared().setEventCallBack([&](Event& event){
        event.dispatch<KeyDownEvent>([&](KeyDownEvent& keyDownEvent)
        {
            if (keyDownEvent.isRepeat())
                return;
            switch (keyDownEvent.keyCode())
            {
            case ESC_KEY:
                looping = false;
                running = false;
                break;

            case ONE_KEY:
                if (usingMetal == false)
                {
                    usingMetal = true;
                    looping = false;
                }
                break;

            case TWO_KEY:
                if (usingMetal == true)
                {
                    usingMetal = false;
                    looping = false;
                }
                break;
            }
        });
    });

    while (running)
    {
        looping = true;
        
        SharedPtr<GraphicAPI> graphicAPI = usingMetal ? Platform::shared().newMetalGraphicAPI(Platform::shared().newMetalWindow(800, 600)) : Platform::shared().newOpenGLGraphicAPI(Platform::shared().newOpenGLWindow(800, 600));
        SharedPtr<VertexBuffer> vertexBuffer = graphicAPI->newVertexBuffer(Array<Vertex>({{-1, -1}, { 0,  1}, { 1, -1}}));
        SharedPtr<GraphicPipeline> graphicPipeline = graphicAPI->newGraphicsPipeline("vtx1", "fra1");

        while(looping)
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
#endif

#ifdef IMGUI_ENABLED

MULTI_TEST(GraphicsTestAPI, imguiDemoWindow,
{
    bool running = true;

    m_graphicAPI->useForImGui([](){
        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable; 
        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    });

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
})

#if defined (USING_METAL) && defined (USING_OPENGL)
TEST(GraphicsTest, imguiDemoWindowAPISwitch)
{
    bool usingMetal = false;
    bool running = true;
    bool looping = true;

    Platform::shared().setEventCallBack([&](Event& event)
    {
        event.dispatch<KeyDownEvent>([&](KeyDownEvent& keyDownEvent)
        {
            if (keyDownEvent.isRepeat())
                return;
            switch (keyDownEvent.keyCode())
            {
            case ESC_KEY:
                looping = false;
                running = false;
                break;

            case ONE_KEY:
                if (usingMetal == false)
                {
                    usingMetal = true;
                    looping = false;
                }
                break;

            case TWO_KEY:
                if (usingMetal == true)
                {
                    usingMetal = false;
                    looping = false;
                }
                break;
            }
        });
    });

    while (running)
    {
        looping = true;

        SharedPtr<GraphicAPI> graphicAPI = usingMetal ? Platform::shared().newMetalGraphicAPI(Platform::shared().newMetalWindow(800, 600)) : Platform::shared().newOpenGLGraphicAPI(Platform::shared().newOpenGLWindow(800, 600));

        graphicAPI->useForImGui([](){
            ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
            ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
            ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable; 
            ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
        });

        while(looping)
        {
            Platform::shared().pollEvents();
            
            graphicAPI->beginFrame();
            ImGui::NewFrame();
            
            ImGui::ShowDemoWindow();
            
            ImGui::Render();
            graphicAPI->endFrame();

            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }
    }
}
#endif

MULTI_TEST(GraphicsTestImGui, imguiHelloWorld,
{
    bool running = true;

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

        ImGui::Text("Hello World!");

        ImGui::Render();
        m_graphicAPI->endFrame();

        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
})

MULTI_TEST(GraphicsTestImGui, fragmentUniform,
{
    const Array<Vertex> vertices = Array<Vertex>({
        {-0.5, -0.5},
        {-0.5,  0.5},
        { 0.5,  0.5},
        { 0.5, -0.5}
    });

    const Array<uint32> indices = Array<uint32>({ 0, 1, 2, 0, 2, 3 });

    bool running = true;

    SharedPtr<GraphicPipeline> graphicPipeline = m_graphicAPI->newGraphicsPipeline("vtx1", "fra2");
    SharedPtr<VertexBuffer> vertexBuffer = m_graphicAPI->newVertexBuffer(vertices);
    SharedPtr<IndexBuffer> indexBuffer = m_graphicAPI->newIndexBuffer(indices);

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

        float color[4];
        ImGui::ColorPicker4("u_color", color);

        m_graphicAPI->useGraphicsPipeline(graphicPipeline);
        m_graphicAPI->useVertexBuffer(vertexBuffer);

        m_graphicAPI->setFragmentUniform(graphicPipeline->findFragmentUniformIndex("u_color"), color[0], color[1], color[2], color[3]);

        m_graphicAPI->drawIndexedVertices(indexBuffer);

        ImGui::Render();
        m_graphicAPI->endFrame();

        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
})

#endif // IMGUI_ENABLED

}
