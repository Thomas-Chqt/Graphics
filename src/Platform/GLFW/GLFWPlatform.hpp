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

#include "Graphics/Event.hpp"
#include "Graphics/Platform.hpp"
#include "UtilsCPP/Array.hpp"
#include "UtilsCPP/Dictionary.hpp"
#include "UtilsCPP/Func.hpp"
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

    inline void setEventCallBack(const utils::Func<void(Event&)>& cb) override { m_eventCallbacks.get((void*)1) = { cb }; }
    inline void addEventCallBack(const utils::Func<void(Event&)>& cb, void* id = (void*)0) override { m_eventCallbacks.get(id).append(cb); }
    inline void clearCallbacks(void* id) override { m_eventCallbacks.remove(id); }

#ifdef GFX_METAL_ENABLED
    utils::SharedPtr<Window> newMetalWindow(int w, int h) const override;
#endif
#ifdef GFX_OPENGL_ENABLED
    utils::SharedPtr<Window> newOpenGLWindow(int w, int h) const override;
#endif

    void pollEvents() override;

    ~GLFWPlatform() override;

private:
    GLFWPlatform();

    utils::Dictionary<void*, utils::Array<utils::Func<void(Event&)>>> m_eventCallbacks;

public:
    GLFWPlatform& operator = (const GLFWPlatform&) = delete;
    GLFWPlatform& operator = (GLFWPlatform&&)      = delete;

};

}

#endif // GLFWPLATFORM_HPP