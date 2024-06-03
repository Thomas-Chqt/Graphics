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
#include "UtilsCPP/Func.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "UtilsCPP/String.hpp"

namespace gfx
{

struct GFLWError
{
public:
    GFLWError() = default;
    static inline void setLastError(int code, const char* description) { s_lastError = GFLWError(description); }
    static inline GFLWError getLastError()
    {
        GFLWError output = s_lastError;
        s_lastError = GFLWError();
        return output;
    }
private:
    inline GFLWError(const char* description) : m_description(std::move(description)) {}
    static GFLWError s_lastError;
    utils::String m_description;
public:
    operator utils::String () { return utils::String("GLFW error: ") + ": " + m_description; }
};

class GLFWPlatform : public Platform
{
public:
    struct InitError : public utils::Error { inline const char* description() const override { return utils::String("fail to initialize GLFW. ") + GFLWError::getLastError(); }  };

private:
    friend void Platform::init();

public:
    GLFWPlatform(const GLFWPlatform&) = delete;
    GLFWPlatform(GLFWPlatform&&)      = delete;

    inline void setEventCallBack(const utils::Func<void(Event&)>& cb) override { m_eventCallBack = cb; }

#ifdef USING_METAL
    utils::SharedPtr<Window> newMetalWindow(int w, int h) const override;
#endif
#ifdef USING_OPENGL
    utils::SharedPtr<Window> newOpenGLWindow(int w, int h) const override;
#endif

    void pollEvents() override;

    ~GLFWPlatform() override;

private:
    GLFWPlatform();

    utils::Func<void(Event&)> m_eventCallBack;

public:
    GLFWPlatform& operator = (const GLFWPlatform&) = delete;
    GLFWPlatform& operator = (GLFWPlatform&&)      = delete;

};

}

#endif // GLFWPLATFORM_HPP