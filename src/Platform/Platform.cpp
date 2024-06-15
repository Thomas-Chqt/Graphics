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
#include "UtilsCPP/String.hpp"
#include <iostream>

#ifdef GFX_METAL_ENABLED
    #include "GraphicAPI/Metal/MetalGraphicAPI.hpp"
#endif
#ifdef GFX_OPENGL_ENABLED
    #include "GraphicAPI/OpenGL/OpenGLGraphicAPI.hpp"
#endif

namespace gfx
{

#ifdef GFX_METAL_ENABLED
utils::SharedPtr<GraphicAPI> Platform::newMetalGraphicAPI(const utils::SharedPtr<Window>& renderTarget) const
{
    std::cout << "Creating MetalGraphicAPI" << std::endl;
    return utils::SharedPtr<GraphicAPI>(new MetalGraphicAPI(renderTarget));
}
#endif

#ifdef GFX_OPENGL_ENABLED
utils::SharedPtr<GraphicAPI> Platform::newOpenGLGraphicAPI(const utils::SharedPtr<Window>& renderTarget) const
{
    std::cout << "Creating OpenGLGraphicAPI" << std::endl;
    return utils::SharedPtr<GraphicAPI>(new OpenGLGraphicAPI(renderTarget));
}
#endif

utils::SharedPtr<Window> Platform::newDefaultWindow(int w, int h)
{
    #if defined (GFX_METAL_ENABLED) && !defined (GFX_OPENGL_ENABLED)
        return newMetalWindow(w, h);
    #elif !defined (GFX_METAL_ENABLED) && defined (GFX_OPENGL_ENABLED)
        return newOpenGLWindow(w, h);
    #else
        if (const char* val = std::getenv("GFX_USED_API"))
        {
            if (utils::String(std::getenv("GFX_USED_API")) == utils::String("OPENGL"))
                return newOpenGLWindow(w, h);
        }
        return newMetalWindow(w, h);
    #endif
}

utils::SharedPtr<GraphicAPI> Platform::newDefaultGraphicAPI(const utils::SharedPtr<Window>& renderTarget)
{
    #if defined (GFX_METAL_ENABLED) && !defined (GFX_OPENGL_ENABLED)
        return newMetalGraphicAPI(renderTarget);
    #elif !defined (GFX_METAL_ENABLED) && defined (GFX_OPENGL_ENABLED)
        return newOpenGLGraphicAPI(renderTarget);
    #else
        if (const char* val = std::getenv("GFX_USED_API"))
        {
            if (utils::String(std::getenv("GFX_USED_API")) == utils::String("OPENGL"))
               return newOpenGLGraphicAPI(renderTarget);
        }
        return newMetalGraphicAPI(renderTarget);
    #endif
}

}
