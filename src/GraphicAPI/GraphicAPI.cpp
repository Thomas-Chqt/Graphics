/*
 * ---------------------------------------------------
 * GraphicAPI.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/25 16:15:15
 * ---------------------------------------------------
 */

#ifdef GFX_IMGUI_ENABLED
    #include "Graphics/GraphicAPI.hpp"
#endif

namespace gfx
{

#ifdef GFX_IMGUI_ENABLED
    GraphicAPI* GraphicAPI::s_imguiEnabledAPI = nullptr;
#endif

}