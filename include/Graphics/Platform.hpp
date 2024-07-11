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

#include "Graphics/GraphicAPI.hpp"
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

    virtual void addEventCallBack(const utils::Func<void(Event&)>&, void* id) = 0;
    inline void addEventCallBack(const utils::Func<void(Event&)>& event) { addEventCallBack(event, nullptr); }
    virtual void clearCallbacks(void* id) = 0;
    inline void clearCallbacks() { clearCallbacks(nullptr); }

#ifdef GFX_BUILD_METAL
    virtual utils::SharedPtr<Window> newMetalWindow(int w, int h) const = 0;
    utils::SharedPtr<GraphicAPI> newMetalGraphicAPI(const utils::SharedPtr<Window>&) const;
#endif
#ifdef GFX_BUILD_OPENGL
    virtual utils::SharedPtr<Window> newOpenGLWindow(int w, int h) const = 0;
    utils::SharedPtr<GraphicAPI> newOpenGLGraphicAPI(const utils::SharedPtr<Window>&) const;
#endif

    utils::SharedPtr<Window> newWindow(int w, int h) const;
    utils::SharedPtr<GraphicAPI> newGraphicAPI(const utils::SharedPtr<Window>&) const;

    virtual void pollEvents() = 0;

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