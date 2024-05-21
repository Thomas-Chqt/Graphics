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
#include "UtilsCPP/SharedPtr.hpp"

namespace gfx
{

class GLFWPlatform : public Platform
{
private:
    friend void Platform::init();

public:
    GLFWPlatform(const GLFWPlatform&) = delete;
    GLFWPlatform(GLFWPlatform&&)      = delete;

    utils::SharedPtr<Window> newWindow(int w, int h) override;
    void pollEvents() override;
    inline void setEventCallBack(const utils::Func<void(Event&)>& cb) override { m_nextEventCallback = cb; }

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