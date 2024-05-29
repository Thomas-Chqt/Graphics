/*
 * ---------------------------------------------------
 * GraphicAPI.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/25 16:15:15
 * ---------------------------------------------------
 */

#ifdef IMGUI_ENABLED
    #include "Graphics/GraphicAPI.hpp"
#endif

namespace gfx
{

#ifdef IMGUI_ENABLED
    GraphicAPI* GraphicAPI::s_imguiEnabledAPI = nullptr;
#endif

}