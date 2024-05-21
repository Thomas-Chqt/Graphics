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
    Event()             = default;
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

private:
    virtual std::ostream& print(std::ostream&) const = 0;

public:
    Event& operator = (const Event&) = delete;
    Event& operator = (Event&&)      = delete;

    inline friend std::ostream& operator << (std::ostream& os, const Event& e) { return e.print(os); }
};

class ApplicationRequestTerminationEvent : public Event
{
public:
    ApplicationRequestTerminationEvent()                                          = default;
    ApplicationRequestTerminationEvent(const ApplicationRequestTerminationEvent&) = delete;
    ApplicationRequestTerminationEvent(ApplicationRequestTerminationEvent&&)      = delete;

    virtual ~ApplicationRequestTerminationEvent() = default;

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

    virtual ~WindowEvent() = default;

protected:
    WindowEvent(Window& window);

    Window& m_window;

private:
    inline std::ostream& print(std::ostream& os) const override { return os << "(WindowEvent) -> Window: " << &m_window; }

public:
    WindowEvent& operator = (const WindowEvent&) = delete;
    WindowEvent& operator = (WindowEvent&&)      = delete;
};

class WindowResizeEvent : public WindowEvent
{
public:
    WindowResizeEvent()                         = delete;
    WindowResizeEvent(const WindowResizeEvent&) = delete;
    WindowResizeEvent(WindowResizeEvent&&)      = delete;

    WindowResizeEvent(Window& window, int w, int h);

    inline int width() const { return m_width; }
    inline int height() const { return m_height; }

    virtual ~WindowResizeEvent() = default;

private:
    inline std::ostream& print(std::ostream& os) const override { return os << "(WindowResizeEvent) -> Window: " << &m_window << " width: " << m_width << " height: " << m_height; }

    int m_width;
    int m_height;

public:
    WindowResizeEvent& operator = (const WindowResizeEvent&) = delete;
    WindowResizeEvent& operator = (WindowResizeEvent&&)      = delete;
};

class WindowRequestCloseEvent : public WindowEvent
{
public:
    WindowRequestCloseEvent()                               = delete;
    WindowRequestCloseEvent(const WindowRequestCloseEvent&) = delete;
    WindowRequestCloseEvent(WindowRequestCloseEvent&&)      = delete;

    WindowRequestCloseEvent(Window& window);

    virtual ~WindowRequestCloseEvent() = default;

private:
    inline std::ostream& print(std::ostream& os) const override { return os << "(WindowRequestCloseEvent) -> Window: " << &m_window; }

public:
    WindowRequestCloseEvent& operator = (const WindowRequestCloseEvent&) = delete;
    WindowRequestCloseEvent& operator = (WindowRequestCloseEvent&&)      = delete;
};

class KeyDownEvent : public WindowEvent
{
public:
    KeyDownEvent()                    = delete;
    KeyDownEvent(const KeyDownEvent&) = delete;
    KeyDownEvent(KeyDownEvent&&)      = delete;

    KeyDownEvent(Window& window, utils::uint8 keyCode, bool isRepeat);

    inline utils::uint8 keyCode() const { return m_keyCode; }
    inline bool isRepeat() const { return m_isRepeat; }

    virtual ~KeyDownEvent() = default;

private:
    inline std::ostream& print(std::ostream& os) const override { return os << "(KeyDownEvent) -> Window: " << &m_window << " keyCode: " << (int)m_keyCode << " isRepeat: " << m_isRepeat; }
    
    utils::uint8 m_keyCode;
    bool m_isRepeat;

public:
    KeyDownEvent& operator = (const KeyDownEvent&) = delete;
    KeyDownEvent& operator = (KeyDownEvent&&)      = delete;
};

class KeyUpEvent : public WindowEvent
{
public:
    KeyUpEvent()                  = delete;
    KeyUpEvent(const KeyUpEvent&) = delete;
    KeyUpEvent(KeyUpEvent&&)      = delete;

    KeyUpEvent(Window& window, utils::uint8 keyCode);

    inline utils::uint8 keyCode() const { return m_keyCode; }

    virtual ~KeyUpEvent() = default;

private:
    inline std::ostream& print(std::ostream& os) const override { return os << "(KeyUpEvent) -> Window: " << &m_window << " keyCode: " << (int)m_keyCode; }

    utils::uint8 m_keyCode;

public:
    KeyUpEvent& operator = (const KeyUpEvent&) = delete;
    KeyUpEvent& operator = (KeyUpEvent&&)      = delete;
};

class MouseDownEvent : public WindowEvent
{
public:
    MouseDownEvent()                      = delete;
    MouseDownEvent(const MouseDownEvent&) = delete;
    MouseDownEvent(MouseDownEvent&&)      = delete;

    MouseDownEvent(Window& window, utils::uint8 mouseCode, int x, int y);

    inline utils::uint8 mouseCode() const { return m_mouseCode; }
    inline int mousePosX() const { return m_pos_x; }
    inline int mousePosY() const { return m_pos_y; }

    virtual ~MouseDownEvent() = default;

private:
    inline std::ostream& print(std::ostream& os) const override { return os << "(MouseDownEvent) -> Window: " << &m_window << " mouseCode: " << (int)m_mouseCode << " X: " << m_pos_x << " Y: " << m_pos_y; }

    utils::uint8 m_mouseCode;
    int m_pos_x;
    int m_pos_y;

public:
    MouseDownEvent& operator = (const MouseDownEvent&) = delete;
    MouseDownEvent& operator = (MouseDownEvent&&)      = delete;
};

class MouseUpEvent : public WindowEvent
{
public:
    MouseUpEvent()                    = delete;
    MouseUpEvent(const MouseUpEvent&) = delete;
    MouseUpEvent(MouseUpEvent&&)      = delete;

    MouseUpEvent(Window& window, utils::uint8 mouseCode, int x, int y);

    inline utils::uint8 mouseCode() const { return m_mouseCode; }
    inline int mousePosX() const { return m_pos_x; }
    inline int mousePosY() const { return m_pos_y; }

    virtual ~MouseUpEvent() = default;

private:
    inline std::ostream& print(std::ostream& os) const override { return os << "(MouseUpEvent) -> Window: " << &m_window << " mouseCode: " << (int)m_mouseCode << " X: " << m_pos_x << " Y: " << m_pos_y; }

    utils::uint8 m_mouseCode;
    int m_pos_x;
    int m_pos_y;

public:
    MouseUpEvent& operator = (const MouseUpEvent&) = delete;
    MouseUpEvent& operator = (MouseUpEvent&&)      = delete;
};

class MouseMoveEvent : public WindowEvent
{
public:
    MouseMoveEvent()                      = delete;
    MouseMoveEvent(const MouseMoveEvent&) = delete;
    MouseMoveEvent(MouseMoveEvent&&)      = delete;

    MouseMoveEvent(Window& window, int x, int y);

    inline int mousePosX() const { return m_pos_x; }
    inline int mousePosY() const { return m_pos_y; }

    virtual ~MouseMoveEvent() = default;

private:
    inline std::ostream& print(std::ostream& os) const override { return os << "(MouseMoveEvent) -> Window: " << &m_window << " X: " << m_pos_x << " Y: " << m_pos_y; }

    int m_pos_x;
    int m_pos_y;

public:
    MouseMoveEvent& operator = (const MouseMoveEvent&) = delete;
    MouseMoveEvent& operator = (MouseMoveEvent&&)      = delete;
};

}

#endif // EVENT_HPP
