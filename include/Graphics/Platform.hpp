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

#if __APPLE__ && defined (USING_METAL)
    #define newDefaultWindow(w, h) newMetalWindow(w, h)
    #define newDefaultGraphicAPI(target) newMetalGraphicAPI(target)
#else
    #define newDefaultWindow(w, h) newOpenGLWindow(w, h)
    #define newDefaultGraphicAPI(target) newOpenGLGraphicAPI(target)
#endif

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

    virtual void setEventCallBack(const utils::Func<void(Event&)>&) = 0;

#ifdef USING_METAL
    virtual utils::SharedPtr<Window> newMetalWindow(int w, int h) = 0;
    utils::SharedPtr<GraphicAPI> newMetalGraphicAPI(const utils::SharedPtr<Window>& renderTarget = utils::SharedPtr<Window>());
#endif
#ifdef USING_OPENGL
    virtual utils::SharedPtr<Window> newOpenGLWindow(int w, int h) = 0;
    utils::SharedPtr<GraphicAPI> newOpenGLGraphicAPI(const utils::SharedPtr<Window>& renderTarget = utils::SharedPtr<Window>());
#endif

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