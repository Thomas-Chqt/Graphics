/*
 * ---------------------------------------------------
 * Platform.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/01/18 14:32:27
 * ---------------------------------------------------
 */

#ifndef PLATFORM_HPP
# define PLATFORM_HPP

#include "UtilsCPP/Func.hpp"
#include "Event.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "UtilsCPP/UniquePtr.hpp"
#include "Window.hpp"

namespace gfx
{

class Platform
{
public:
    Platform(const Platform&) = delete;
    Platform(Platform&&)      = delete;

    static void init();
    inline static Platform& shared() { return *s_shared; };
    static void terminate();

    virtual utils::SharedPtr<Window> newWindow(int w, int h) = 0;
    virtual void pollEvents() = 0;
    virtual void setEventCallBack(const utils::Func<void(Event&)>&) = 0;

    virtual ~Platform() = default;

protected:
    Platform() = default;

private:
    static utils::UniquePtr<Platform> s_shared;

public:
    Platform& operator = (const Platform&) = delete;
    Platform& operator = (Platform&&)      = delete;
};

}

#endif // PLATFORM_HPP