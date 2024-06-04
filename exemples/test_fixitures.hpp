/*
 * ---------------------------------------------------
 * test_fixitures.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/23 20:41:24
 * ---------------------------------------------------
 */

#ifndef TEST_FIXITURES_HPP
# define TEST_FIXITURES_HPP

#include <gtest/gtest.h>

#include "UtilsCPP/SharedPtr.hpp"

#include "Graphics/Platform.hpp"
#include "Graphics/Window.hpp"
#include "Graphics/GraphicAPI.hpp"
#ifdef IMGUI_ENABLED
    #include "imgui/imgui.h"
#endif

using namespace gfx;
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

#ifdef IMGUI_ENABLED
    #if defined (USING_METAL) && defined (USING_OPENGL)
        class GraphicsTestImGuiMetal : public GraphicsTestAPIMetal
        {
        public:
            GraphicsTestImGuiMetal()
            {
                m_graphicAPI->useForImGui([](){
                    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
                    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
                    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable; 
                    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
                });
            }
        };

        class GraphicsTestImGuiOpenGL : public GraphicsTestAPIOpenGL
        {
        public:
            GraphicsTestImGuiOpenGL()
            {
                m_graphicAPI->useForImGui([](){
                    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
                    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
                    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable; 
                    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
                });
            }
        };
    #else
        class GraphicsTestImGui : public GraphicsTestAPI
        {
        public:
            GraphicsTestImGui()
            {
                m_graphicAPI->useForImGui([](){
                    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
                    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
                    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable; 
                    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
                });
            }
        };
    #endif
#endif

#if defined (USING_METAL) && defined (USING_OPENGL)
#define MULTI_TEST(fixiture, name, body)     \
    TEST_F(fixiture##Metal, name)            \
    body                                     \
    TEST_F(fixiture##OpenGL, name)           \
    body
#else
#define MULTI_TEST(fixiture, name, body)     \
    TEST_F(fixiture, name)                   \
    body
#endif

#endif // TEST_FIXITURES_HPP