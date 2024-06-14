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

#include "Graphics/Enums.hpp"
#include "UtilsCPP/Func.hpp"
#include "Event.hpp"
#include "UtilsCPP/Types.hpp"

namespace gfx
{

class Window
{
public:
    Window(const Window&) = delete;
    Window(Window&&)      = delete;

    virtual void setEventCallBack(const utils::Func<void(Event&)>&) = 0;

    virtual void setCursorPos(int x, int y) = 0;
    virtual void setCursorVisibility(bool) = 0;

    virtual void getWindowSize(utils::uint32* width, utils::uint32* height) = 0;
    virtual void getFrameBufferSize(utils::uint32* width, utils::uint32* height) = 0;

    // virtual ColorPixelFormat pixelFormat() = 0;

    virtual ~Window() = default;

protected:
    Window() = default;

public:
    Window& operator = (const Window&) = delete;
    Window& operator = (Window&&)      = delete;
};

}

#endif // WINDOW_HPP