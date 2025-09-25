# ---------------------------------------------------
# CMakeLists.txt
#
# Author: Thomas Choquet <thomas.publique@icloud.com>
# Date: 2024/05/15 13:50:09
# ---------------------------------------------------

if (GFX_ENABLE_GLFW)
    add_subdirectory(triangle)
    add_subdirectory(multiBuffer)
    if (GFX_ENABLE_IMGUI)
        add_subdirectory(imgui_usage)
        add_subdirectory(mc_cube)
        add_subdirectory(scop)
    endif()
endif()
