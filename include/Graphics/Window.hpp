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

#include "Graphics/RenderTarget.hpp"
#include "UtilsCPP/Func.hpp"
#include "Event.hpp"
#include "UtilsCPP/String.hpp"
#include "UtilsCPP/Types.hpp"
#include <filesystem>

namespace gfx
{

class Window : public RenderTarget
{
public:
    Window(const Window&) = delete;
    Window(Window&&)      = delete;

    virtual utils::uint32 width() override = 0;
    virtual utils::uint32 height() override = 0;

    virtual void addEventCallBack(const utils::Func<void(Event&)>&, void* id) = 0;
    inline void addEventCallBack(const utils::Func<void(Event&)>& event) { addEventCallBack(event, nullptr); }
    virtual void clearCallbacks(void* id) = 0;
    inline void clearCallbacks() { clearCallbacks(nullptr); }

    virtual bool isKeyPress(int) = 0;
    virtual bool isMousePress(int) = 0;

    virtual void setCursorPos(int x, int y) = 0;
    virtual void setCursorVisibility(bool) = 0;

    virtual void getWindowSize(utils::uint32* width, utils::uint32* height) const = 0;
    virtual void getFrameBufferSize(utils::uint32* width, utils::uint32* height) const = 0;
    virtual void getContentScale(float* xScale, float* yScale) const = 0;
    virtual void getFrameBufferScaleFactor(float* xScale, float* yScale) const = 0;

    virtual bool popDroppedFile(std::filesystem::path& dst) = 0;

    virtual void setClipboardString(const utils::String&) const = 0;
    virtual utils::String getClipboardString() const = 0;

    virtual ~Window() = default;

protected:
    Window() = default;

public:
    Window& operator = (const Window&) = delete;
    Window& operator = (Window&&)      = delete;
};

}

#endif // WINDOW_HPP