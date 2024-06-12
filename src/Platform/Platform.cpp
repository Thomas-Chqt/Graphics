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
    return utils::SharedPtr<GraphicAPI>(new MetalGraphicAPI(renderTarget));
}
#endif

#ifdef GFX_OPENGL_ENABLED
utils::SharedPtr<GraphicAPI> Platform::newOpenGLGraphicAPI(const utils::SharedPtr<Window>& renderTarget) const
{
    return utils::SharedPtr<GraphicAPI>(new OpenGLGraphicAPI(renderTarget));
}
#endif

}
