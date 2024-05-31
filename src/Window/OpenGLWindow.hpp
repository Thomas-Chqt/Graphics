/*
 * ---------------------------------------------------
 * OpenGLWindow.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/23 09:17:46
 * ---------------------------------------------------
 */

#ifndef OPENGLWINDOW_HPP
# define OPENGLWINDOW_HPP

#include "Window/Window_internal.hpp"

namespace gfx
{

class OpenGLWindow : virtual public Window_internal
{
public:
    OpenGLWindow(const OpenGLWindow&) = delete;
    OpenGLWindow(OpenGLWindow&&)      = delete;

    virtual void makeContextCurrent() = 0;
    virtual void swapBuffer() = 0;

    virtual ~OpenGLWindow() = default;

protected:
    OpenGLWindow() = default;

public:
    OpenGLWindow& operator = (const OpenGLWindow&) = delete;
    OpenGLWindow& operator = (OpenGLWindow&&)      = delete;
};

}

#endif // OPENGLWINDOW_HPP