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

InputEvent::InputEvent(Window& window) : WindowEvent(window)
{
}

KeyboardEvent::KeyboardEvent(Window& window, int keyCode) : InputEvent(window), m_keyCode(keyCode)
{
}

KeyDownEvent::KeyDownEvent(Window& window, int keyCode, bool isRepeat) : KeyboardEvent(window, keyCode), m_isRepeat(isRepeat)
{
}

KeyUpEvent::KeyUpEvent(Window& window, int keyCode) : KeyboardEvent(window, keyCode)
{
}

MouseEvent::MouseEvent(Window& window, int posX, int posY) : InputEvent(window), m_pos_x(posX), m_pos_y(posY)
{
}

ScrollEvent::ScrollEvent(Window& window, int posX, int posY, double offsetX, double offsetY) : MouseEvent(window, posX, posY), m_offsetX(offsetX), m_offsetY(offsetY)
{
}

MouseMoveEvent::MouseMoveEvent(Window& window, int posX, int posY) : MouseEvent(window, posX, posY)
{
}

MouseButtonEvent::MouseButtonEvent(Window& window, int posX, int posY, int mouseCode) : MouseEvent(window, posX, posY), m_mouseCode(mouseCode)
{
}

MouseDownEvent::MouseDownEvent(Window& window, int posX, int posY, int mouseCode) : MouseButtonEvent(window, posX, posY, mouseCode)
{
}

MouseUpEvent::MouseUpEvent(Window& window, int posX, int posY, int mouseCode) : MouseButtonEvent(window, posX, posY, mouseCode)
{
}

}