# ---------------------------------------------------
# CMakeLists.txt
#
# Author: Thomas Choquet <thomas.publique@icloud.com>
# Date: 2024/05/15 13:50:09
# ---------------------------------------------------

if (GFX_ENABLE_GLFW)
    if ("triangle" IN_LIST GFX_EXAMPLES_TO_BUILD)
        add_subdirectory(triangle)
    endif()
    if ("multiBuffer" IN_LIST GFX_EXAMPLES_TO_BUILD)
        add_subdirectory(multiBuffer)
    endif()
    if (GFX_ENABLE_IMGUI)
        if ("imgui_usage" IN_LIST GFX_EXAMPLES_TO_BUILD)
            add_subdirectory(imgui_usage)
        endif()
        if ("mc_cube" IN_LIST GFX_EXAMPLES_TO_BUILD)
            add_subdirectory(mc_cube)
        endif()
        if ("scop" IN_LIST GFX_EXAMPLES_TO_BUILD)
            add_subdirectory(scop)
        endif()
    endif()
endif()
