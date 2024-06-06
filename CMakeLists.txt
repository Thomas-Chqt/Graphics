# ---------------------------------------------------
# CMakeLists.txt
#
# Author: Thomas Choquet <thomas.publique@icloud.com>
# Date: 2024/05/15 13:43:58
# ---------------------------------------------------

cmake_minimum_required(VERSION 3.27)

include(FetchContent)

enable_testing()

option(BUILD_SHARED_LIBS  "Build using shared libraries"        OFF)
option(GFX_BUILD_TESTS    "Build Graphics tests"                OFF)
option(GFX_BUILD_EXEMPLES "Build Graphics exemples executables" OFF)
option(ENABLE_IMGUI       "Build with imgui capabilities"       OFF)

if(GFX_BUILD_TESTS)
    if(GFX_BUILD_EXEMPLES)
        message(WARNING "GFX_BUILD_EXEMPLES cannot be enable when GFX_BUILD_TESTS is enable, disabling...")
        set(GFX_BUILD_EXEMPLES OFF CACHE FORCE)
    endif()
    if(ENABLE_IMGUI)
        message(WARNING "ENABLE_IMGUI cannot be enable when GFX_BUILD_TESTS is enable, disabling...")
        set(ENABLE_IMGUI OFF CACHE FORCE)
    endif()
endif()

if (APPLE)
    set(USED_WINDOW_LIBRARY "GLFW" CACHE STRING "Libray to use for handle window (available options: GLFW)")
    if (USED_WINDOW_LIBRARY STREQUAL "GLFW")
        option(ENABLE_METAL  "Build with metal enable"  ON)
        option(ENABLE_OPENGL "Build with OpenGL enable" ON)
    else()
        message(FATAL_ERROR "Unknown window library")
    endif()
else()
    set(USED_WINDOW_LIBRARY "GLFW" CACHE STRING "Libray to use for handle window (available options: GLFW)")
    if (USED_WINDOW_LIBRARY STREQUAL "GLFW")
        if(ENABLE_METAL)
            message(FATAL_ERROR "Metal is only available on apple platforms")
        endif()
        option(ENABLE_OPENGL "Build with OpenGL enable" ON)
    else()
        message(FATAL_ERROR "Unknown window library")
    endif()
endif()

if (NOT ENABLE_METAL AND NOT ENABLE_OPENGL)
    message(FATAL_ERROR "One graphic api must be enable")
endif()


project(Graphics ${GFX_LIBRARY_TYPE} CXX)

add_subdirectory("dependencies")

if (APPLE AND USED_WINDOW_LIBRARY STREQUAL "GLFW")
    enable_language(OBJC)
endif()
if(ENABLE_METAL)
    enable_language(OBJCXX)
endif()


add_library(Graphics)

set_target_properties(Graphics PROPERTIES CXX_STANDARD 11 CXX_STANDARD_REQUIRED ON)


file(GLOB_RECURSE GRAPHICS_SRC "include/*.tpp" "include/*.hpp")
target_sources(Graphics PRIVATE ${GRAPHICS_SRC})
add_subdirectory(src)


target_include_directories(Graphics PRIVATE "src" PUBLIC "include")
if(GFX_BUILD_TESTS)
    target_include_directories(Graphics SYSTEM PRIVATE "tests/MockLibs/includes")
endif()


if(USED_WINDOW_LIBRARY STREQUAL "GLFW")
    target_compile_definitions(Graphics PUBLIC USING_GLFW)
    target_compile_definitions(Graphics PUBLIC GLFW_INCLUDE_NONE)
endif()

if (ENABLE_METAL)
    target_compile_definitions(Graphics PUBLIC USING_METAL) # deprecated
    target_compile_definitions(Graphics PUBLIC METAL_ENABLED) 
endif()
if (ENABLE_OPENGL)
    target_compile_definitions(Graphics PUBLIC USING_OPENGL) # deprecated
    target_compile_definitions(Graphics PUBLIC OPENGL_ENABLED)
endif()

if (ENABLE_IMGUI)
    target_compile_definitions(Graphics PUBLIC IMGUI_ENABLED)
endif()

if (APPLE AND ENABLE_OPENGL)
    target_compile_definitions(Graphics PUBLIC GL_SILENCE_DEPRECATION)
endif()


target_link_libraries(Graphics PUBLIC UtilsCPP Math)

if (NOT GFX_BUILD_TESTS)
    if (USED_WINDOW_LIBRARY STREQUAL "GLFW")
        set(GLFW_BUILD_DOCS OFF)
        set(GLFW_INSTALL OFF)
        FetchContent_Declare(glfw GIT_REPOSITORY https://github.com/glfw/glfw.git GIT_TAG master)
        FetchContent_MakeAvailable(glfw)
        target_compile_options(glfw PRIVATE -w)
        target_link_libraries(Graphics PRIVATE glfw)
    endif()

    if (ENABLE_METAL)
        target_link_libraries(Graphics PRIVATE "-framework Metal -framework QuartzCore")
    endif()
        
    if (ENABLE_OPENGL)
        cmake_policy(SET CMP0077 NEW)
        set(glew-cmake_BUILD_SHARED OFF)
        FetchContent_Declare(glew-cmake GIT_REPOSITORY https://github.com/Perlmint/glew-cmake.git GIT_TAG glew-cmake-2.2.0)
        FetchContent_MakeAvailable(glew-cmake)
        target_link_libraries(Graphics PRIVATE libglew_static)
    endif()
else()
    add_subdirectory(tests)
endif()