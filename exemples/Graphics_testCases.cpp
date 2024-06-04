/*
 * ---------------------------------------------------
 * Graphics_testCases.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/15 13:53:46
 * ---------------------------------------------------
 */

#include <cassert>
#include <gtest/gtest.h>

#include "Graphics/ShaderLibrary.hpp"
#include "Graphics/Texture.hpp"
#include "Math/Matrix.hpp"
#include "Math/Vector.hpp"
#include "test_fixitures.hpp"
#include "Graphics/Event.hpp"
#include "Graphics/GraphicPipeline.hpp"
#include "Graphics/IndexBuffer.hpp"
#include "Graphics/VertexBuffer.hpp"
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
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h" 

#include "Math/Constants.hpp" // IWYU pragma: keep

namespace gfx_test
{

using namespace gfx;
using namespace utils;
using namespace math;

MULTI_TEST(GraphicsTestWindow, simple,
{
})

MULTI_TEST(GraphicsTestWindow, event,
{
    bool running = true;

    Platform::shared().setEventCallBack([&](Event& ev)
    {
        std::cout << ev << std::endl;

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
                m_graphicAPI->setClearColor(RED);
            if (e.mouseCode() == MOUSE_R)
                m_graphicAPI->setClearColor(GREEN);
        });

        ev.dispatch<MouseUpEvent>([&](MouseUpEvent& e)
        {
            m_graphicAPI->setClearColor(BLACK);
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
    Array<Vertex_vertexBuffer> vertices;

    vertices.append({-1, -1});
    vertices.append({ 1,  1});
    vertices.append({ 1,  0});

    SharedPtr<VertexBuffer> vertexBuffer = m_graphicAPI->newVertexBuffer(vertices);
})

MULTI_TEST(GraphicsTestAPI, graphicPipeline,
{
    SharedPtr<GraphicPipeline> graphicPipeline = m_graphicAPI->newGraphicsPipeline("graphicPipeline_vertex", "graphicPipeline_fragment");
})

MULTI_TEST(GraphicsTestAPI, triangle,
{
    bool running = true;

    Array<Vertex_triangle> vertices;

    vertices.append({-1, -1});
    vertices.append({ 0,  1});
    vertices.append({ 1, -1});

    SharedPtr<VertexBuffer> vertexBuffer = m_graphicAPI->newVertexBuffer(vertices);
    SharedPtr<GraphicPipeline> graphicPipeline = m_graphicAPI->newGraphicsPipeline("triangle_vertex", "triangle_fragment");

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

    Array<Vertex_indexedShape> vertices = Array<Vertex_indexedShape>({
        {-0.5, -0.5},
        {-0.5,  0.5},
        { 0.5,  0.5},
        { 0.5, -0.5}
    });
    Array<uint32> indices = Array<uint32>({ 0, 1, 2, 0, 2, 3 });

    SharedPtr<GraphicPipeline> graphicPipeline = m_graphicAPI->newGraphicsPipeline("indexedShape_vertex", "indexedShape_fragment");
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
    #if defined (USING_METAL) && defined (USING_OPENGL)
        ShaderLibrary::shared().registerShader("APISwitch_vertex",   "APISwitch_vertex",   utils::String::contentOfFile(OPENGL_SHADER_DIR"/APISwitch/vertex.glsl"));
        ShaderLibrary::shared().registerShader("APISwitch_fragment", "APISwitch_fragment", utils::String::contentOfFile(OPENGL_SHADER_DIR"/APISwitch/fragment.glsl"));
    #elif defined (USING_METAL)
        ShaderLibrary::shared().registerShader("APISwitch_vertex",   "APISwitch_vertex");
        ShaderLibrary::shared().registerShader("APISwitch_fragment", "APISwitch_fragment");
    #elif defined (USING_OPENGL)
        ShaderLibrary::shared().registerShader("APISwitch_vertex",   utils::String::contentOfFile(OPENGL_SHADER_DIR"/APISwitch/vertex.glsl"));
        ShaderLibrary::shared().registerShader("APISwitch_fragment", utils::String::contentOfFile(OPENGL_SHADER_DIR"/APISwitch/fragment.glsl"));
    #else
    #endif

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
        SharedPtr<VertexBuffer> vertexBuffer = graphicAPI->newVertexBuffer(Array<Vertex_APISwitch>({{-1, -1}, { 0,  1}, { 1, -1}}));
        SharedPtr<GraphicPipeline> graphicPipeline = graphicAPI->newGraphicsPipeline("APISwitch_vertex", "APISwitch_fragment");

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
    const Array<Vertex_fragmentUniform> vertices = Array<Vertex_fragmentUniform>({
        {-0.5, -0.5},
        {-0.5,  0.5},
        { 0.5,  0.5},
        { 0.5, -0.5}
    });

    const Array<uint32> indices = Array<uint32>({ 0, 1, 2, 0, 2, 3 });

    bool running = true;

    SharedPtr<GraphicPipeline> graphicPipeline = m_graphicAPI->newGraphicsPipeline("fragmentUniform_vertex", "fragmentUniform_fragment");
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

    rgba color = WHITE;
    while (running)
    {
        Platform::shared().pollEvents();
        
        m_graphicAPI->beginFrame();
        ImGui::NewFrame();

        ImGui::ColorPicker4("u_color", (float *)&color);

        m_graphicAPI->useGraphicsPipeline(graphicPipeline);
        m_graphicAPI->useVertexBuffer(vertexBuffer);

        m_graphicAPI->setFragmentUniform(graphicPipeline->findFragmentUniformIndex("u_color"), color);

        m_graphicAPI->drawIndexedVertices(indexBuffer);

        ImGui::Render();
        m_graphicAPI->endFrame();

        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
})

MULTI_TEST(GraphicsTestImGui, flatColorCube,
{
    const Array<Vertex_flatColorCube> vertices = Array<Vertex_flatColorCube>({
        { -0.5, -0.5, -0.5 },
        { -0.5,  0.5, -0.5 },
        {  0.5,  0.5, -0.5 },
        {  0.5, -0.5, -0.5 },
        {  0.5, -0.5,  0.5 },
        {  0.5,  0.5,  0.5 },
        { -0.5,  0.5,  0.5 },
        { -0.5, -0.5,  0.5 }
    });

    const Array<uint32> indices = Array<uint32>({
        0, 1, 2, 0, 2, 3, // Front
        4, 5, 6, 4, 6, 7, // Back
        3, 2, 5, 3, 5, 4, // Right
        7, 6, 1, 7, 1, 0, // Left
        1, 6, 5, 1, 5, 2, // Top
        0, 3, 4, 0, 4, 7  // Bottom
    });

    bool running = true;

    SharedPtr<GraphicPipeline> graphicPipeline = m_graphicAPI->newGraphicsPipeline("flatColorCube_vertex", "flatColorCube_fragment");
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

    float fov = 60 * (M_PI / 180.0f);
    float aspectRatio = 800.0f / 600.0f;
    float zNear = 0.1f;
    float zFar = 100;

    float ys = 1 / std::tanf(fov * 0.5);
    float xs = ys / aspectRatio;
    float zs = zFar / (zFar - zNear);

    mat4x4 projectionMatrix (xs,  0,   0,           0,
                              0,  ys,  0,           0,
                              0,   0, zs, -zNear * zs,
                              0,   0,  1,           0 );

    vec3f cubePos(0, 0, 3);
    rgba cubeColor = WHITE;

    while (running)
    {
        Platform::shared().pollEvents();
        
        m_graphicAPI->beginFrame();
        ImGui::NewFrame();

        m_graphicAPI->useGraphicsPipeline(graphicPipeline);
        m_graphicAPI->useVertexBuffer(vertexBuffer);

        mat4x4 modelMatrix(1, 0, 0, cubePos.x,
                           0, 1, 0, cubePos.y,
                           0, 0, 1, cubePos.z,
                           0, 0, 0, 1         );

        ImGui::Text("FPS : %.1f", ImGui::GetIO().Framerate);
        ImGui::SliderFloat("Pos X", &cubePos.x, -5, 5);
        ImGui::SliderFloat("Pos Y", &cubePos.y, -5, 5);
        ImGui::SliderFloat("Pos Z", &cubePos.z, -1, 10);
        ImGui::ColorPicker4("u_color", (float *)&cubeColor);

        m_graphicAPI->setVertexUniform(graphicPipeline->findVertexUniformIndex("u_MVPMatrix"), projectionMatrix * modelMatrix);
        m_graphicAPI->setFragmentUniform(graphicPipeline->findFragmentUniformIndex("u_color"), cubeColor);

        m_graphicAPI->drawIndexedVertices(indexBuffer);

        ImGui::Render();
        m_graphicAPI->endFrame();

        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
})

#endif // IMGUI_ENABLED

MULTI_TEST(GraphicsTestAPI, texturedSquare,
{    
    const Array<Vertex_texturedSquare> vertices = Array<Vertex_texturedSquare>({
        { math::vec2f{-0.5, -0.5}, math::vec2f{ 0.0, 0.0 } },
        { math::vec2f{-0.5,  0.5}, math::vec2f{ 0.0, 1.0 } },
        { math::vec2f{ 0.5,  0.5}, math::vec2f{ 1.0, 1.0 } },
        { math::vec2f{ 0.5, -0.5}, math::vec2f{ 1.0, 0.0 } }
    });
    const Array<uint32> indices = Array<uint32>({ 0, 1, 2, 0, 2, 3 });

    SharedPtr<GraphicPipeline> graphicPipeline = m_graphicAPI->newGraphicsPipeline("texturedSquare_vertex", "texturedSquare_fragment");
    SharedPtr<VertexBuffer> vertexBuffer = m_graphicAPI->newVertexBuffer(vertices);
    SharedPtr<IndexBuffer> indexBuffer = m_graphicAPI->newIndexBuffer(indices);

    int width;
    int height;
    stbi_set_flip_vertically_on_load(true);
    stbi_uc* imgBytes = stbi_load("tests/Ressources/mc_grass.jpg", &width, &height, nullptr, STBI_rgb_alpha);
    ASSERT_NE(imgBytes, nullptr);
    SharedPtr<Texture> texture = m_graphicAPI->newTexture(width, height);
    texture->setBytes(imgBytes);
    stbi_image_free(imgBytes);

    bool running = true;
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
        m_graphicAPI->setFragmentTexture(graphicPipeline->findFragmentUniformIndex("u_texture"), texture);

        m_graphicAPI->drawIndexedVertices(indexBuffer);

        m_graphicAPI->endFrame();
    }

})

MULTI_TEST(GraphicsTestWindow, noEventCallBack,
{
    bool running = true;

    while (running)
        Platform::shared().pollEvents();
})

MULTI_TEST(GraphicsTestWindow, eventCallBackOnWindow,
{
    bool running = true;

    m_window->setEventCallBack([&](Event& ev)
    {
        std::cout << ev << std::endl;

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
        Platform::shared().pollEvents();
})

MULTI_TEST(GraphicsTestWindow, setCursorPos,
{
    bool running = true;

    Platform::shared().setEventCallBack([&](Event& ev)
    {
        ev.dispatch<KeyDownEvent>([&](KeyDownEvent& e)
        {
            switch (e.keyCode())
            {
            case ESC_KEY:
                running = false;
                break;
                
            case ONE_KEY:
                m_window->setCursorPos(0, 0);
                break;

            case TWO_KEY:
                m_window->setCursorPos(100, 100);
                break;
            }
        });
    });

    while (running)
        Platform::shared().pollEvents();
})

MULTI_TEST(GraphicsTestWindow, hideCusor,
{
    bool running = true;

    Platform::shared().setEventCallBack([&](Event& ev)
    {
        ev.dispatch<KeyDownEvent>([&](KeyDownEvent& e)
        {
            switch (e.keyCode())
            {
            case ESC_KEY:
                running = false;
                break;

            case ONE_KEY:
                m_window->setCursorVisibility(false);
                break;

            case TWO_KEY:
                m_window->setCursorVisibility(true);
                break;
            }
        });
    });

    while (running)
        Platform::shared().pollEvents();
})

#ifdef IMGUI_ENABLED

MULTI_TEST(GraphicsTestImGui, noClearBuffer, {
    Array<Vertex_noClearBuffer> vertices = Array<Vertex_noClearBuffer>({
        { {-0.25, -0.25} },
        { {-0.25,  0.25} },
        { { 0.25,  0.25} },
        { { 0.25, -0.25} }
    });
    Array<uint32> indices = Array<uint32>({ 0, 1, 2, 0, 2, 3 });

    SharedPtr<GraphicPipeline> graphicPipeline = m_graphicAPI->newGraphicsPipeline("noClearBuffer_vertex", "noClearBuffer_fragment");
    SharedPtr<VertexBuffer> vertexBuffer = m_graphicAPI->newVertexBuffer(vertices);
    SharedPtr<IndexBuffer> indexBuffer = m_graphicAPI->newIndexBuffer(indices);

    bool running = true;
    Platform::shared().setEventCallBack([&](Event& ev)
    {
        ev.dispatch<KeyDownEvent>([&](KeyDownEvent& e)
        {
            if (e.keyCode() == ESC_KEY)
                running = false;
        });
    });

    vec2f pos(0, 0);
    rgba color = WHITE;

    while (running)
    {
        Platform::shared().pollEvents();
        
        m_graphicAPI->beginFrame(false);
        ImGui::NewFrame();

        m_graphicAPI->useGraphicsPipeline(graphicPipeline);
        m_graphicAPI->useVertexBuffer(vertexBuffer);

        mat3x3 modelMatrix(1, 0, pos.x,
                           0, 1, pos.y,
                           0, 0,     1); 

        ImGui::Text("FPS : %.1f", ImGui::GetIO().Framerate);
        ImGui::SliderFloat("Pos X", &pos.x, -1, 1);
        ImGui::SliderFloat("Pos Y", &pos.y, -1, 1);
        ImGui::ColorPicker4("u_color", (float *)&color);

        m_graphicAPI->setVertexUniform(graphicPipeline->findVertexUniformIndex("u_MVPMatrix"), modelMatrix);
        m_graphicAPI->setFragmentUniform(graphicPipeline->findFragmentUniformIndex("u_color"), color);

        m_graphicAPI->drawIndexedVertices(indexBuffer);

        ImGui::Render();
        m_graphicAPI->endFrame();

        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
})

#endif // IMGUI_ENABLED


}
