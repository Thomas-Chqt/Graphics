# ---------------------------------------------------
# CMakeLists.txt
#
# Author: Thomas Choquet <thomas.publique@icloud.com>
# Date: 2024/05/15 13:43:58
# ---------------------------------------------------

cmake_minimum_required(VERSION 3.23)

include(FetchContent)

get_directory_property(hasParent PARENT_DIRECTORY)
if(NOT hasParent)
    project(Graphics LANGUAGES CXX)
endif()

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        master
)
FetchContent_Declare(
    glew-cmake
    GIT_REPOSITORY https://github.com/Perlmint/glew-cmake.git
    GIT_TAG        glew-cmake-2.2.0
)

if (GRAPHICS_BUILD_TESTS)
    enable_testing()
endif()

option(BUILD_SHARED_LIBS "Build using shared libraries" OFF)
option(GRAPHICS_BUILD_TESTS "Build utils cpp test executables" OFF)
option(ENABLE_IMGUI "Build with function to init imgui" OFF)
if (APPLE)
    set(USED_WINDOW_LIBRARY "GLFW" CACHE STRING "Libray to use for handle window (available options: GLFW)")
    if (NOT USED_WINDOW_LIBRARY STREQUAL GLFW)
        message(FATAL_ERROR "Unknown window library")
    endif()
else()
    set(USED_WINDOW_LIBRARY "GLFW" CACHE STRING "Libray to use for handle window (available options: GLFW)")
    if (NOT USED_WINDOW_LIBRARY STREQUAL GLFW)
        message(FATAL_ERROR "Unknown window library")
    endif()
endif()

if (USED_WINDOW_LIBRARY STREQUAL AppKit)
    option(ENABLE_METAL "Build with metal enable" ON)
    option(ENABLE_OPENGL "Build with OpenGL enable" ON)
elseif (USED_WINDOW_LIBRARY STREQUAL GLFW)
    if (APPLE)
        option(ENABLE_METAL "Build with metal enable" ON)
    endif()
    option(ENABLE_OPENGL "Build with OpenGL enable" ON)
endif()

if (NOT ENABLE_METAL AND NOT ENABLE_OPENGL)
    message(FATAL_ERROR "One graphic api must be enable")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (USED_WINDOW_LIBRARY STREQUAL AppKit OR ENABLE_METAL)
    enable_language(OBJCXX)
endif()


add_library(Graphics)


file(GLOB_RECURSE GRAPHICS_INCLUDES "include/*.tpp" "include/*.hpp")
target_sources(Graphics PRIVATE ${GRAPHICS_INCLUDES})

add_subdirectory(src)


if (USED_WINDOW_LIBRARY STREQUAL AppKit)
    target_compile_definitions(Graphics PUBLIC USING_APPKIT)
elseif(USED_WINDOW_LIBRARY STREQUAL GLFW)
    target_compile_definitions(Graphics PUBLIC USING_GLFW)
endif()

if (ENABLE_METAL)
    target_compile_definitions(Graphics PUBLIC USING_METAL)
endif()
if (ENABLE_OPENGL)
    target_compile_definitions(Graphics PUBLIC USING_OPENGL)
endif()

if (APPLE AND ENABLE_OPENGL)
    target_compile_definitions(Graphics PUBLIC GL_SILENCE_DEPRECATION)
endif()

if (ENABLE_IMGUI)
    target_compile_definitions(Graphics PUBLIC IMGUI_ENABLED)
endif()

target_include_directories(Graphics PRIVATE "src" PUBLIC "include")


if (USED_WINDOW_LIBRARY STREQUAL AppKit)
    target_link_libraries(Graphics PUBLIC "-framework AppKit")
endif()

if (USED_WINDOW_LIBRARY STREQUAL GLFW)
    FetchContent_MakeAvailable(glfw)
    target_compile_options(glfw PRIVATE -w)
    target_link_libraries(Graphics PUBLIC glfw)
endif()

if (ENABLE_METAL)
    target_link_libraries(Graphics PUBLIC "-framework Metal -framework QuartzCore")
endif()
    
if (ENABLE_OPENGL)
    FetchContent_MakeAvailable(glew-cmake)
    target_link_libraries(Graphics PUBLIC libglew_static)
endif()
    
add_subdirectory(dependencies)

if (GRAPHICS_BUILD_TESTS)
    add_subdirectory(tests)
endif()