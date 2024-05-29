/*
 * ---------------------------------------------------
 * GLFWPlatform.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/01/20 12:51:02
 * ---------------------------------------------------
 */

#ifndef GLFWPLATFORM_HPP
# define GLFWPLATFORM_HPP

#include "Graphics/Platform.hpp"
#include "UtilsCPP/Error.hpp"
#include "UtilsCPP/SharedPtr.hpp"

namespace gfx
{

class GLFWPlatform : public Platform
{
public:
    struct GLFWInitError : public utils::Error { inline const char* description() const override { return "fail to initialize GLFW"; }  };

private:
    friend void Platform::init();

public:
    GLFWPlatform(const GLFWPlatform&) = delete;
    GLFWPlatform(GLFWPlatform&&)      = delete;

    inline void setEventCallBack(const utils::Func<void(Event&)>& cb) override { m_nextEventCallback = cb; }

#ifdef USING_METAL
    utils::SharedPtr<Window> newMetalWindow(int w, int h) override;
#endif
#ifdef USING_OPENGL
    utils::SharedPtr<Window> newOpenGLWindow(int w, int h) override;
#endif

    void pollEvents() override;

    ~GLFWPlatform() override;

private:
    GLFWPlatform();

    void eventCallBack(Event& e);

    utils::Func<void(Event&)> m_nextEventCallback;

public:
    GLFWPlatform& operator = (const GLFWPlatform&) = delete;
    GLFWPlatform& operator = (GLFWPlatform&&)      = delete;

};

}

#endif // GLFWPLATFORM_HPP