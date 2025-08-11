# ---------------------------------------------------
# CMakeLists.txt
#
# Author: Thomas Choquet <thomas.publique@icloud.com>
# Date: 2024/05/15 13:50:09
# ---------------------------------------------------

if (GFX_ENABLE_GLFW)
    add_subdirectory(triangle)
    add_subdirectory(multiBuffer)
    add_subdirectory(mc_cube)
    if (GFX_ENABLE_IMGUI)
        add_subdirectory(imgui_usage)
    endif()
endif()
