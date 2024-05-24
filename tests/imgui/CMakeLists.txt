# ---------------------------------------------------
# CMakeLists.txt
#
# Author: Thomas Choquet <thomas.publique@icloud.com>
# Date: 2024/05/22 15:24:12
# ---------------------------------------------------

add_library(imgui)

target_sources(imgui PRIVATE
    "src/imgui_demo.cpp"
    "src/imgui_draw.cpp"
    "src/imgui_internal.h"
    "src/imgui_tables.cpp"
    "src/imgui_widgets.cpp"
    "src/imgui.cpp"
    "src/imstb_rectpack.h"
    "src/imstb_textedit.h"
    "src/imstb_truetype.h"
    "include/imgui/imconfig.h"
    "include/imgui/imgui.h"
)

if (USED_WINDOW_LIBRARY STREQUAL GLFW)
    target_sources(imgui PRIVATE
        "src/imgui_impl_glfw.cpp"
        "include/imgui/imgui_impl_glfw.h"
    )
endif()

if (ENABLE_METAL)
    target_sources(imgui PRIVATE
        "src/imgui_impl_metal.mm"
        "include/imgui/imgui_impl_metal.h"
    )
endif()

if (ENABLE_OPENGL)
    target_sources(imgui PRIVATE
        "src/imgui_impl_opengl3.cpp"
        "src/imgui_impl_opengl3_loader.h"
        "include/imgui/imgui_impl_opengl3.h"
    )
endif()

target_compile_options(imgui PRIVATE -w)

target_include_directories(imgui PRIVATE src PUBLIC include)

if (APPLE)
    target_link_libraries(imgui PUBLIC "-framework GameController")
endif()

if (USED_WINDOW_LIBRARY STREQUAL GLFW)
    target_link_libraries(imgui PUBLIC glfw)
endif()