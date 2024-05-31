# ---------------------------------------------------
# CMakeLists.txt
#
# Author: Thomas Choquet <thomas.publique@icloud.com>
# Date: 2024/05/15 13:43:58
# ---------------------------------------------------

cmake_minimum_required(VERSION 3.23)

include(FetchContent)

enable_testing()

option(BUILD_SHARED_LIBS       "Build using shared libraries"       OFF)
option(GRAPHICS_IN_PARENT_PROJ "Add Graphics to the parend project" ON)
option(GRAPHICS_INLCUDE_TESTS  "Build utils cpp test executables"   OFF)
option(ENABLE_IMGUI            "Build with imgui capabilities"      OFF)

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


get_directory_property(HAS_PARENT PARENT_DIRECTORY)
if(NOT GRAPHICS_IN_PARENT_PROJ OR NOT HAS_PARENT)
    project(Graphics)
endif()

add_subdirectory("dependencies")

enable_language(CXX)
if(ENABLE_METAL OR USED_WINDOW_LIBRARY STREQUAL "AppKit")
    enable_language(OBJCXX)
endif()


add_library(Graphics)

set_target_properties(Graphics PROPERTIES CXX_STANDARD 11 CXX_STANDARD_REQUIRED ON)


file(GLOB_RECURSE GRAPHICS_SRC "include/*.tpp" "include/*.hpp")
target_sources(Graphics PRIVATE ${GRAPHICS_SRC})
add_subdirectory(src)


target_include_directories(Graphics PRIVATE "src" PUBLIC "include")


if (USED_WINDOW_LIBRARY STREQUAL "AppKit")
    target_compile_definitions(Graphics PUBLIC USING_APPKIT)
elseif(USED_WINDOW_LIBRARY STREQUAL "GLFW")
    target_compile_definitions(Graphics PUBLIC USING_GLFW)
    target_compile_definitions(Graphics PRIVATE GLFW_INCLUDE_NONE)
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

if (USED_WINDOW_LIBRARY STREQUAL "AppKit")
    target_link_libraries(Graphics PUBLIC "-framework AppKit")
endif()

if (USED_WINDOW_LIBRARY STREQUAL "GLFW")
    enable_language(OBJC)
    FetchContent_Declare(glfw GIT_REPOSITORY https://github.com/glfw/glfw.git GIT_TAG master)
    FetchContent_MakeAvailable(glfw)
    target_compile_options(glfw PRIVATE -w)
    target_link_libraries(Graphics PUBLIC glfw)
endif()

if (ENABLE_METAL)
    target_link_libraries(Graphics PUBLIC "-framework Metal -framework QuartzCore")
endif()
    
if (ENABLE_OPENGL)
    FetchContent_Declare(glew-cmake GIT_REPOSITORY https://github.com/Perlmint/glew-cmake.git GIT_TAG glew-cmake-2.2.0)
    FetchContent_MakeAvailable(glew-cmake)
    target_link_libraries(Graphics PUBLIC libglew_static)
endif()


if (GRAPHICS_INLCUDE_TESTS)
    add_subdirectory(tests)
endif()