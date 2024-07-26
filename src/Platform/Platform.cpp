/*
 * ---------------------------------------------------
 * Platform.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/12 06:32:30
 * ---------------------------------------------------
 */

#include "Graphics/Platform.hpp"
#include "Graphics/GraphicAPI.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "Graphics/Window.hpp"
#include "UtilsCPP/String.hpp" // IWYU pragma: keep
#include <iostream> // IWYU pragma: keep

#ifdef GFX_BUILD_METAL
    #include "GraphicAPI/Metal/MetalGraphicAPI.hpp"
#endif
#ifdef GFX_BUILD_OPENGL
    #include "GraphicAPI/OpenGL/OpenGLGraphicAPI.hpp"
#endif

namespace gfx
{

#ifdef GFX_BUILD_METAL
utils::SharedPtr<GraphicAPI> Platform::newMetalGraphicAPI(const utils::SharedPtr<Window>& window) const // NOLINT(*-convert-member-functions-to-static)
{
    return utils::SharedPtr<GraphicAPI>(new MetalGraphicAPI(window));
}
#endif

#ifdef GFX_BUILD_OPENGL
utils::SharedPtr<GraphicAPI> Platform::newOpenGLGraphicAPI(const utils::SharedPtr<Window>& window) const // NOLINT(*-convert-member-functions-to-static)
{
    return utils::SharedPtr<GraphicAPI>(new OpenGLGraphicAPI(window));
}
#endif

utils::SharedPtr<Window> Platform::newWindow(int w, int h) const
{
    #if defined (GFX_BUILD_METAL) && !defined (GFX_BUILD_OPENGL)
        return newMetalWindow(w, h);
    #elif !defined (GFX_BUILD_METAL) && defined (GFX_BUILD_OPENGL)
        return newOpenGLWindow(w, h);
    #else
        if (const char* val = std::getenv("GFX_USED_API")) // NOLINT(concurrency-mt-unsafe)
        {
            if (utils::String(val) == utils::String("OPENGL"))
                return newOpenGLWindow(w, h);
        }
        return newMetalWindow(w, h);
    #endif
}

utils::SharedPtr<GraphicAPI> Platform::newGraphicAPI(const utils::SharedPtr<Window>& window) const
{
    #if defined (GFX_BUILD_METAL) && !defined (GFX_BUILD_OPENGL)
        return newMetalGraphicAPI(window);
    #elif !defined (GFX_BUILD_METAL) && defined (GFX_BUILD_OPENGL)
        return newOpenGLGraphicAPI(window);
    #else
        if (const char* val = std::getenv("GFX_USED_API")) // NOLINT(concurrency-mt-unsafe)
        {
            if (utils::String(val) == utils::String("OPENGL"))
            {
                std::cout << "Creating OpenGLGraphicAPI" << std::endl;
                return newOpenGLGraphicAPI(window);
            }
            if (utils::String(val) == utils::String("METAL"))
            {
                std::cout << "Creating MetalGraphicAPI" << std::endl;
                return newMetalGraphicAPI(window);
            }
        }
        return newMetalGraphicAPI(window);
    #endif
}

}
