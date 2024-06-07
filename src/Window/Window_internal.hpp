/*
 * ---------------------------------------------------
 * Window_internal.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/23 09:14:30
 * ---------------------------------------------------
 */

#ifndef WINDOW_INTERNAL_HPP
# define WINDOW_INTERNAL_HPP

#include "Graphics/Window.hpp"

namespace gfx
{

class Window_internal : public Window
{
public:
    Window_internal(const Window_internal&) = delete;
    Window_internal(Window_internal&&)      = delete;
    
    #ifdef GFX_IMGUI_ENABLED
        virtual void imGuiInit()     = 0;
        virtual void imGuiShutdown() = 0;
        virtual void imGuiNewFrame() = 0;
    #endif

    virtual ~Window_internal() = default;

protected:
    Window_internal() = default;

public:
    Window_internal& operator = (const Window_internal&) = delete;
    Window_internal& operator = (Window_internal&&)      = delete;
      
};

}

#endif // WINDOW_INTERNAL_HPP