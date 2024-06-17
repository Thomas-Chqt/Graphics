# ---------------------------------------------------
# CMakeLists.txt
#
# Author: Thomas Choquet <thomas.publique@icloud.com>
# Date: 2024/06/17 11:47:11
# ---------------------------------------------------

add_library(imgui)

set_target_properties(imgui PROPERTIES CXX_STANDARD 11 CXX_STANDARD_REQUIRED ON)

target_sources(imgui PRIVATE
    "imgui_demo.cpp"
    "imgui_draw.cpp"
    "imgui_internal.h"
    "imgui_tables.cpp"
    "imgui_widgets.cpp"
    "imgui.cpp"
    "imstb_rectpack.h"
    "imstb_textedit.h"
    "imstb_truetype.h"
)

if(GFX_USED_WINDOW_LIBRARY STREQUAL GLFW)
    target_sources(imgui PRIVATE
        "imgui_impl_glfw.cpp"
        "imgui_impl_glfw.h"
    )
endif()

if(GFX_ENABLE_METAL)
    target_sources(imgui PRIVATE
        "imgui_impl_metal.mm"
        "imgui_impl_metal.h"
    )
endif()

if(GFX_ENABLE_OPENGL)
    target_sources(imgui PRIVATE
        "imgui_impl_opengl3.cpp"
        "imgui_impl_opengl3_loader.h"
        "imgui_impl_opengl3.h"
    )
endif()

target_include_directories(imgui PRIVATE "${CMAKE_SOURCE_DIR}/src" "${CMAKE_SOURCE_DIR}/include")

target_compile_options(imgui PRIVATE "-fobjc-arc")

target_link_libraries(imgui PRIVATE glfw)