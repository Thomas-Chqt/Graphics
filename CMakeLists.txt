# ---------------------------------------------------
# CMakeLists.txt
#
# Author: Thomas Choquet <thomas.publique@icloud.com>
# Date: 2024/05/15 13:43:58
# ---------------------------------------------------

cmake_minimum_required(VERSION 3.16)

include(FetchContent)

enable_testing()

option(BUILD_SHARED_LIBS  "Build using shared libraries"        OFF)
option(GFX_BUILD_EXEMPLES "Build Graphics exemples executables" OFF)
option(GFX_BUILD_IMGUI    "Build with imgui capabilities"       OFF)

if (APPLE)
    set(GFX_USED_WINDOW_LIBRARY "GLFW" CACHE STRING "Used library for window handling (available options: GLFW)")
    if (GFX_USED_WINDOW_LIBRARY STREQUAL "GLFW")
        option(GFX_ENABLE_METAL  "Build with metal enable"  ON)
        option(GFX_ENABLE_OPENGL "Build with OpenGL enable" ON)
    else()
        message(FATAL_ERROR "Unknown window library")
    endif()
else()
    set(GFX_USED_WINDOW_LIBRARY "GLFW" CACHE STRING "Used library for window handling (available options: GLFW)")
    if (GFX_USED_WINDOW_LIBRARY STREQUAL "GLFW")
        if(GFX_ENABLE_METAL)
            message(FATAL_ERROR "Metal is only available on apple platforms")
        endif()
        option(GFX_ENABLE_OPENGL "Build with OpenGL enable" ON)
    else()
        message(FATAL_ERROR "Unknown window library")
    endif()
endif()

if (NOT GFX_ENABLE_METAL AND NOT GFX_ENABLE_OPENGL)
    message(FATAL_ERROR "One graphic api must be enable")
endif()


project(Graphics)

enable_language(CXX)

if (APPLE AND GFX_USED_WINDOW_LIBRARY STREQUAL "GLFW")
    enable_language(OBJC)
endif()
if(GFX_ENABLE_METAL)
    enable_language(OBJCXX)
endif()


add_library(Graphics ${GFX_LIBRARY_TYPE})

set_target_properties(Graphics PROPERTIES CXX_STANDARD 11 CXX_STANDARD_REQUIRED ON)


file(GLOB_RECURSE GRAPHICS_SRC "include/Graphics/*.hpp")
target_sources(Graphics PRIVATE ${GRAPHICS_SRC})
if(GFX_BUILD_IMGUI)
    target_sources(Graphics PRIVATE "include/imgui/imgui.h")
endif()

add_subdirectory("src")


target_include_directories(Graphics PRIVATE "src" PUBLIC "include")

if (GFX_BUILD_IMGUI)
    target_compile_definitions(Graphics PUBLIC GFX_IMGUI_ENABLED)
endif()

if(GFX_USED_WINDOW_LIBRARY STREQUAL "GLFW")
    target_compile_definitions(Graphics PUBLIC GFX_USING_GLFW)
    target_compile_definitions(Graphics PUBLIC GLFW_INCLUDE_NONE)
endif()

if (GFX_ENABLE_METAL)
    target_compile_definitions(Graphics PUBLIC GFX_METAL_ENABLED)
endif()
if (GFX_ENABLE_OPENGL)
    if(APPLE)
        target_compile_definitions(Graphics PUBLIC GL_SILENCE_DEPRECATION)
    endif()
    target_compile_definitions(Graphics PUBLIC GFX_OPENGL_ENABLED)
endif()


add_subdirectory("dependencies")

if (GFX_USED_WINDOW_LIBRARY STREQUAL "GLFW")
    set(GLFW_BUILD_DOCS OFF)
    set(GLFW_INSTALL    OFF)

    FetchContent_Declare(glfw GIT_REPOSITORY https://github.com/glfw/glfw.git GIT_TAG master)
    FetchContent_MakeAvailable(glfw)

    get_target_property(GLFW_FOLDER glfw FOLDER)
    set_target_properties(glfw PROPERTIES FOLDER "dependencies/${GLFW_FOLDER}")
    get_target_property(UPDATE_MAP_FOLDER update_mappings FOLDER)
    set_target_properties(update_mappings PROPERTIES FOLDER "dependencies/${UPDATE_MAP_FOLDER}")

    target_compile_options(glfw PRIVATE "-w")

    target_link_libraries(Graphics PRIVATE glfw)
endif()

if (GFX_ENABLE_METAL)
    target_link_libraries(Graphics PRIVATE "-framework Metal -framework QuartzCore")
endif()
    
if (GFX_ENABLE_OPENGL)
    set(glew-cmake_BUILD_SHARED OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(glew-cmake GIT_REPOSITORY https://github.com/Perlmint/glew-cmake.git GIT_TAG glew-cmake-2.2.0)
    FetchContent_MakeAvailable(glew-cmake)
    set_target_properties(libglew_static PROPERTIES FOLDER "dependencies")
    target_link_libraries(Graphics PRIVATE libglew_static)
endif()

if(GFX_BUILD_EXEMPLES)
    add_subdirectory("exemples")
endif()