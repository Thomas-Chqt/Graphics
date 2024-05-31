/*
 * ---------------------------------------------------
 * Event.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2023/11/17 00:53:33
 * ---------------------------------------------------
 */

#ifndef EVENT_HPP
#define EVENT_HPP

#include <ostream>

#include "UtilsCPP/Func.hpp"
#include "UtilsCPP/Types.hpp"

namespace gfx
{

class Window;

class Event
{
public:
    Event(const Event&) = delete;
    Event(Event&&)      = delete;

    template<typename T>
    bool dispatch(const utils::Func<void(T&)>& f)
    {
        T* casted_event = dynamic_cast<T*>(this);
        if (casted_event == nullptr)
            return false;
        f(*casted_event);
        return true;
    }

    virtual ~Event() = default;

protected:
    Event() = default;

private:
    virtual std::ostream& print(std::ostream&) const = 0;

public:
    Event& operator = (const Event&) = delete;
    Event& operator = (Event&&)      = delete;

    inline friend std::ostream& operator << (std::ostream& os, const Event& e) { return e.print(os); }
};

class ApplicationRequestTerminationEvent final : public Event
{
public:
    ApplicationRequestTerminationEvent()                                          = default;
    ApplicationRequestTerminationEvent(const ApplicationRequestTerminationEvent&) = delete;
    ApplicationRequestTerminationEvent(ApplicationRequestTerminationEvent&&)      = delete;

    ~ApplicationRequestTerminationEvent() override = default;

private:
    inline std::ostream& print(std::ostream& os) const override { return os << "(ApplicationRequestTerminationEvent) ->"; }

public:
    ApplicationRequestTerminationEvent& operator = (const ApplicationRequestTerminationEvent&) = delete;
    ApplicationRequestTerminationEvent& operator = (ApplicationRequestTerminationEvent&&)      = delete;
};

class WindowEvent : public Event
{
public:
    WindowEvent()                   = delete;
    WindowEvent(const WindowEvent&) = delete;
    WindowEvent(WindowEvent&&)      = delete;

    inline const Window& window() const { return m_window; }

    virtual ~WindowEvent() override = default;

protected:
    WindowEvent(Window& window);

private:
    virtual std::ostream& print(std::ostream& os) const override = 0;

protected:
    Window& m_window;

public:
    WindowEvent& operator = (const WindowEvent&) = delete;
    WindowEvent& operator = (WindowEvent&&)      = delete;
};

class WindowResizeEvent final : public WindowEvent
{
public:
    WindowResizeEvent()                         = delete;
    WindowResizeEvent(const WindowResizeEvent&) = delete;
    WindowResizeEvent(WindowResizeEvent&&)      = delete;

    WindowResizeEvent(Window& window, int w, int h);

    inline int width() const { return m_width; }
    inline int height() const { return m_height; }

    ~WindowResizeEvent() override = default;

private:
    inline std::ostream& print(std::ostream& os) const override { return os << "(WindowResizeEvent) -> Window: " << &m_window << " width: " << m_width << " height: " << m_height; }

    int m_width;
    int m_height;

public:
    WindowResizeEvent& operator = (const WindowResizeEvent&) = delete;
    WindowResizeEvent& operator = (WindowResizeEvent&&)      = delete;
};

class WindowRequestCloseEvent final : public WindowEvent
{
public:
    WindowRequestCloseEvent()                               = delete;
    WindowRequestCloseEvent(const WindowRequestCloseEvent&) = delete;
    WindowRequestCloseEvent(WindowRequestCloseEvent&&)      = delete;

    WindowRequestCloseEvent(Window& window);

    ~WindowRequestCloseEvent() override = default;

private:
    inline std::ostream& print(std::ostream& os) const override { return os << "(WindowRequestCloseEvent) -> Window: " << &m_window; }

public:
    WindowRequestCloseEvent& operator = (const WindowRequestCloseEvent&) = delete;
    WindowRequestCloseEvent& operator = (WindowRequestCloseEvent&&)      = delete;
};

class InputEvent : public WindowEvent
{
public:
    InputEvent()                  = delete;
    InputEvent(const InputEvent&) = delete;
    InputEvent(InputEvent&&)      = delete;

    virtual ~InputEvent() override = default;

protected:
    InputEvent(Window& window);

private:
    virtual std::ostream& print(std::ostream& os) const override = 0;

public:
    InputEvent& operator = (const InputEvent&) = delete;
    InputEvent& operator = (InputEvent&&)      = delete;
};

class KeyboardEvent : public InputEvent
{
public:
    KeyboardEvent()                     = delete;
    KeyboardEvent(const KeyboardEvent&) = delete;
    KeyboardEvent(KeyboardEvent&&)      = delete;

    inline utils::uint8 keyCode() const { return m_keyCode; }

    virtual ~KeyboardEvent() override = default;

protected:
    KeyboardEvent(Window& window, utils::uint8 keyCode);

private:
    virtual std::ostream& print(std::ostream& os) const override = 0;

protected:
    utils::uint8 m_keyCode;

public:
    KeyboardEvent& operator = (const KeyboardEvent&) = delete;
    KeyboardEvent& operator = (KeyboardEvent&&)      = delete;
};

class KeyDownEvent final : public KeyboardEvent
{
public:
    KeyDownEvent()                    = delete;
    KeyDownEvent(const KeyDownEvent&) = delete;
    KeyDownEvent(KeyDownEvent&&)      = delete;

    KeyDownEvent(Window& window, utils::uint8 keyCode, bool isRepeat);

    inline bool isRepeat() const { return m_isRepeat; }

    ~KeyDownEvent() override = default;

private:
    inline std::ostream& print(std::ostream& os) const override { return os << "(KeyDownEvent) -> Window: " << &m_window << " keyCode: " << (int)m_keyCode << " isRepeat: " << m_isRepeat; }
    
    bool m_isRepeat;

public:
    KeyDownEvent& operator = (const KeyDownEvent&) = delete;
    KeyDownEvent& operator = (KeyDownEvent&&)      = delete;
};

class KeyUpEvent final : public KeyboardEvent
{
public:
    KeyUpEvent()                  = delete;
    KeyUpEvent(const KeyUpEvent&) = delete;
    KeyUpEvent(KeyUpEvent&&)      = delete;

    KeyUpEvent(Window& window, utils::uint8 keyCode);

    ~KeyUpEvent() override = default;

private:
    inline std::ostream& print(std::ostream& os) const override { return os << "(KeyUpEvent) -> Window: " << &m_window << " keyCode: " << (int)m_keyCode; }

public:
    KeyUpEvent& operator = (const KeyUpEvent&) = delete;
    KeyUpEvent& operator = (KeyUpEvent&&)      = delete;
};

class MouseEvent : public InputEvent
{
public:
    MouseEvent()                  = delete;
    MouseEvent(const MouseEvent&) = delete;
    MouseEvent(MouseEvent&&)      = delete;

    inline int mousePosX() const { return m_pos_x; }
    inline int mousePosY() const { return m_pos_y; }

    virtual ~MouseEvent() override = default;

protected:
    MouseEvent(Window& window, int posX, int posY);

private:
    virtual std::ostream& print(std::ostream& os) const override = 0;

protected:
    int m_pos_x;
    int m_pos_y;

public:
    MouseEvent& operator = (const MouseEvent&) = delete;
    MouseEvent& operator = (MouseEvent&&)      = delete;
};

class ScrollEvent final : public MouseEvent
{
public:
    ScrollEvent()                   = delete;
    ScrollEvent(const ScrollEvent&) = delete;
    ScrollEvent(ScrollEvent&&)      = delete;

    ScrollEvent(Window& window, int posX, int posY, double offsetX, double offsetY);

    inline int offsetX() const { return m_offsetX; }
    inline int offsetY() const { return m_offsetY; }

    ~ScrollEvent() override = default;

private:
    double m_offsetX;
    double m_offsetY;

private:
    inline std::ostream& print(std::ostream& os) const override { return os << "(ScrollEvent) -> Window: " << &m_window << " pos: " << m_pos_x << ',' << m_pos_y << " offset:" << m_offsetX << ',' << m_offsetY; };

private:
    ScrollEvent& operator = (const ScrollEvent&) = delete;
    ScrollEvent& operator = (ScrollEvent&&)      = delete;
};

class MouseMoveEvent final : public MouseEvent
{
public:
    MouseMoveEvent()                      = delete;
    MouseMoveEvent(const MouseMoveEvent&) = delete;
    MouseMoveEvent(MouseMoveEvent&&)      = delete;

    MouseMoveEvent(Window& window, int posX, int posY);

    virtual ~MouseMoveEvent() = default;

private:
    inline std::ostream& print(std::ostream& os) const override { return os << "(MouseMoveEvent) -> Window: " << &m_window << " X: " << m_pos_x << " Y: " << m_pos_y; }

public:
    MouseMoveEvent& operator = (const MouseMoveEvent&) = delete;
    MouseMoveEvent& operator = (MouseMoveEvent&&)      = delete;
};

class MouseButtonEvent : public MouseEvent
{
public:
    MouseButtonEvent()                        = delete;
    MouseButtonEvent(const MouseButtonEvent&) = delete;
    MouseButtonEvent(MouseButtonEvent&&)      = delete;

    inline utils::uint8 mouseCode() const { return m_mouseCode; }

    virtual ~MouseButtonEvent() = default;

protected:
    MouseButtonEvent(Window& window, int posX, int posY, utils::uint8 mouseCode);

    utils::uint8 m_mouseCode;

private:
    virtual std::ostream& print(std::ostream& os) const override = 0;

public:
    MouseButtonEvent& operator = (const MouseButtonEvent&) = delete;
    MouseButtonEvent& operator = (MouseButtonEvent&&)      = delete;

};

class MouseDownEvent final : public MouseButtonEvent
{
public:
    MouseDownEvent()                      = delete;
    MouseDownEvent(const MouseDownEvent&) = delete;
    MouseDownEvent(MouseDownEvent&&)      = delete;

    MouseDownEvent(Window& window, int posX, int posY, utils::uint8 mouseCode);

    virtual ~MouseDownEvent() = default;

private:
    inline std::ostream& print(std::ostream& os) const override { return os << "(MouseDownEvent) -> Window: " << &m_window << " mouseCode: " << (int)m_mouseCode << " X: " << m_pos_x << " Y: " << m_pos_y; }

public:
    MouseDownEvent& operator = (const MouseDownEvent&) = delete;
    MouseDownEvent& operator = (MouseDownEvent&&)      = delete;
};

class MouseUpEvent final : public MouseButtonEvent
{
public:
    MouseUpEvent()                    = delete;
    MouseUpEvent(const MouseUpEvent&) = delete;
    MouseUpEvent(MouseUpEvent&&)      = delete;

    MouseUpEvent(Window& window, int posX, int posY, utils::uint8 mouseCode);

    virtual ~MouseUpEvent() = default;

private:
    inline std::ostream& print(std::ostream& os) const override { return os << "(MouseUpEvent) -> Window: " << &m_window << " mouseCode: " << (int)m_mouseCode << " X: " << m_pos_x << " Y: " << m_pos_y; }

public:
    MouseUpEvent& operator = (const MouseUpEvent&) = delete;
    MouseUpEvent& operator = (MouseUpEvent&&)      = delete;
};

}

#endif // EVENT_HPP
