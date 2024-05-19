/*
 * ---------------------------------------------------
 * Event.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/15 16:13:22
 * ---------------------------------------------------
 */

#include "Graphics/Event.hpp"

namespace gfx
{

WindowEvent::WindowEvent(Window& window) : m_window(window)
{
}

WindowResizeEvent::WindowResizeEvent(Window& window, int w, int h) : WindowEvent(window), m_width(w), m_height(h)
{
}

WindowRequestCloseEvent::WindowRequestCloseEvent(Window& window) : WindowEvent(window)
{
}

KeyDownEvent::KeyDownEvent(Window& window, utils::uint8 keyCode, bool isRepeat) : WindowEvent(window), m_keyCode(keyCode), m_isRepeat(isRepeat)
{
}

KeyUpEvent::KeyUpEvent(Window& window, utils::uint8 keyCode) : WindowEvent(window), m_keyCode(keyCode)
{
}

MouseDownEvent::MouseDownEvent(Window& window, utils::uint8 mouseCode, int x, int y) : WindowEvent(window), m_mouseCode(mouseCode), m_pos_x(x), m_pos_y(y)
{
}

MouseUpEvent::MouseUpEvent(Window& window, utils::uint8 mouseCode, int x, int y) : WindowEvent(window), m_mouseCode(mouseCode), m_pos_x(x), m_pos_y(y)
{
}

MouseMoveEvent::MouseMoveEvent(Window& window, int x, int y) : WindowEvent(window), m_pos_x(x), m_pos_y(y)
{
}

}