# ---------------------------------------------------
# CMakeLists.txt
#
# Author: Thomas Choquet <semoir.dense-0h@icloud.com>
# Date: 2025/07/27 10:41:59
# ---------------------------------------------------

include(FetchContent)

## GLFW ##########################################################

FetchContent_Declare(glfw3
    GIT_REPOSITORY    https://github.com/glfw/glfw.git
    GIT_TAG           3.4
    GIT_SHALLOW       1
    GIT_PROGRESS      TRUE
    FIND_PACKAGE_ARGS
)
set(GLFW_BUILD_TESTS    OFF)
set(GLFW_BUILD_DOCS     OFF)
set(GLFW_INSTALL        OFF)
set(GLFW_BUILD_EXAMPLES OFF)
if(NOT GLFW_BUILD_WAYLAND)
    set(GLFW_BUILD_WAYLAND OFF)
endif()
if (APPLE)
    enable_language(OBJC)
endif()
FetchContent_MakeAvailable(glfw3)
if (glfw3_SOURCE_DIR)
    if (TARGET update_mappings)
        set_target_properties(glfw PROPERTIES FOLDER "dependencies/GLFW3")
        set_target_properties(update_mappings PROPERTIES FOLDER "dependencies/GLFW3")
    else()
        set_target_properties(glfw PROPERTIES FOLDER "dependencies")
    endif()
endif()

## IMGUI ###############################################################

FetchContent_Declare(imgui
    GIT_REPOSITORY    https://github.com/Thomas-Chqt/imgui.git
    GIT_TAG           4d8f55183c2e974916daf6336ee2c4b9c8e72891
    GIT_SHALLOW       1
    GIT_PROGRESS      TRUE
    FIND_PACKAGE_ARGS
)
set(IM_BUILD_GLFW ON)
FetchContent_MakeAvailable(imgui)
target_compile_definitions(imgui PRIVATE "GLFW_INCLUDE_NONE")
target_link_libraries(imgui PUBLIC glfw)
if (imgui_SOURCE_DIR)
    set_target_properties(imgui PROPERTIES FOLDER "dependencies")
endif()

##################################################################

add_executable(imgui_usage)

if (APPLE)
    enable_language(OBJC)
endif()

set_target_properties(imgui_usage PROPERTIES
    FOLDER         "examples/imgui_usage"
    ENABLE_EXPORTS ON
)

target_compile_features(imgui_usage PRIVATE cxx_std_23)

if(MSVC)
    target_compile_options(imgui_usage PRIVATE /W4)
else()
    target_compile_options(imgui_usage PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(imgui_usage PRIVATE $<$<CXX_COMPILER_ID:GNU>:-Wno-missing-field-initializers>)
endif()

file(GLOB EXE_SRCS "*.cpp" "*.hpp")
target_sources(imgui_usage PRIVATE ${EXE_SRCS})
if (WIN32)
    target_sources(imgui_usage PRIVATE imgui_usage.def)
endif()

target_include_directories(imgui_usage PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

if(CMAKE_GENERATOR STREQUAL "Xcode")
    target_compile_definitions(imgui_usage PRIVATE "__XCODE__")
endif()
target_compile_definitions(imgui_usage PRIVATE "GLFW_INCLUDE_NONE")

target_link_libraries(imgui_usage PRIVATE Graphics imgui)
