/*
 * ---------------------------------------------------
 * GLFWMetalWindow.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/23 09:56:51
 * ---------------------------------------------------
 */

#ifndef GLFWMETALWINDOW_HPP
# define GLFWMETALWINDOW_HPP

#include "UtilsCPP/Types.hpp"
#include "Window/GLFW/GLFWWindow.hpp"
#include "Window/MetalWindow.hpp"
#include <GLFW/glfw3.h>

#ifdef __OBJC__
    #import <Metal/Metal.h>
#else
    template<typename T> using id = T*;

    class CAMetalDrawable;
#endif // __OBJC__

namespace gfx
{

class GLFWMetalWindow final : public GLFWWindow, public MetalWindow
{
public:
    GLFWMetalWindow(int w, int h);
    GLFWMetalWindow(const GLFWMetalWindow&) = delete;
    GLFWMetalWindow(GLFWMetalWindow&&)      = delete;
    
    #ifdef GFX_BUILD_IMGUI
        void imGuiInit() override;
    #endif

    void setGraphicAPI(MetalGraphicAPI*) override;

    void makeCurrentDrawables() override;
    inline const id<CAMetalDrawable>& currentDrawable() override { return m_currentDrawable; }
    inline const MetalTexture& currentDepthTexture() override { return m_currentDepthTexture; }
    void clearCurrentDrawables() override;

    ~GLFWMetalWindow() override = default;

private:
    void recreateDepthTexture(utils::uint32 w, utils::uint32 h);

    MetalGraphicAPI* m_graphicAPI = nullptr;
    id<CAMetalDrawable> m_currentDrawable = nullptr;
    MetalTexture m_currentDepthTexture;

public:
    GLFWMetalWindow& operator = (const GLFWMetalWindow&) = delete;
    GLFWMetalWindow& operator = (GLFWMetalWindow&&)      = delete;
};

}

#endif // GLFWMETALWINDOW_HPP