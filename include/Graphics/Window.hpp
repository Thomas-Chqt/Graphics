/*
 * ---------------------------------------------------
 * Window.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2023/11/14 17:09:45
 * ---------------------------------------------------
 */

#ifndef WINDOW_HPP
# define WINDOW_HPP

#include "UtilsCPP/Func.hpp"
#include "Event.hpp"

namespace gfx
{

class Window
{
public:
    Window(const Window&) = delete;
    Window(Window&&)      = delete;

    virtual void setEventCallBack(const utils::Func<void(Event&)>&) = 0;

    virtual ~Window() = default;

protected:
    Window() = default;

public:
    Window& operator = (const Window&) = delete;
    Window& operator = (Window&&)      = delete;
};

}

#endif // WINDOW_HPP