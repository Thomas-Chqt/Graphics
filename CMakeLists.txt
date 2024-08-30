# ---------------------------------------------------
# CMakeLists.txt
#
# Author: Thomas Choquet <thomas.publique@icloud.com>
# Date: 2024/05/15 13:43:58
# ---------------------------------------------------

cmake_minimum_required(VERSION 3.16)

include(FetchContent)

enable_testing()

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build using shared libraries" FORCE)

option(GFX_BUILD_EXEMPLES "Build Graphics exemples executables" OFF)
option(GFX_BUILD_IMGUI    "Build with imgui capabilities"       OFF)
option(GFX_INSTALL        "Enable Graphics install command"     ON)

if (APPLE)
    set(GFX_USED_WINDOW_LIBRARY "GLFW" CACHE STRING "Used library for window handling (available options: GLFW)")
    if (GFX_USED_WINDOW_LIBRARY STREQUAL "GLFW")
        option(GFX_BUILD_METAL  "Build with metal enable"  ON)
        option(GFX_BUILD_OPENGL "Build with OpenGL enable" ON)
    else()
        message(FATAL_ERROR "Unknown window library")
    endif()
else()
    set(GFX_USED_WINDOW_LIBRARY "GLFW" CACHE STRING "Used library for window handling (available options: GLFW)")
    if (GFX_USED_WINDOW_LIBRARY STREQUAL "GLFW")
        if(GFX_BUILD_METAL)
            message(FATAL_ERROR "Metal is only available on apple platforms")
        endif()
        option(GFX_BUILD_OPENGL "Build with OpenGL enable" ON)
    else()
        message(FATAL_ERROR "Unknown window library")
    endif()
endif()

if (NOT GFX_BUILD_METAL AND NOT GFX_BUILD_OPENGL)
    message(FATAL_ERROR "One graphic api must be enable")
endif()


project(Graphics)
enable_language(CXX)

if (APPLE AND GFX_USED_WINDOW_LIBRARY STREQUAL "GLFW")
    enable_language(OBJC)
endif()
if(GFX_BUILD_METAL)
    enable_language(OBJCXX)
endif()

if(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
endif()

if(NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
endif()

if(NOT CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
endif()

add_library(Graphics STATIC)

set_target_properties(Graphics PROPERTIES CXX_STANDARD 11 CXX_STANDARD_REQUIRED ON)


file(GLOB_RECURSE GRAPHICS_SRC "include/Graphics/*.hpp")
target_sources(Graphics PRIVATE ${GRAPHICS_SRC})

add_subdirectory("src")


target_include_directories(Graphics PRIVATE "src" PUBLIC "include")

if (GFX_BUILD_IMGUI)
    target_compile_definitions(Graphics PUBLIC GFX_BUILD_IMGUI)
endif()

if(GFX_USED_WINDOW_LIBRARY STREQUAL "GLFW")
    target_compile_definitions(Graphics PUBLIC GFX_USING_GLFW)
    target_compile_definitions(Graphics PUBLIC GLFW_INCLUDE_NONE)
endif()

if (GFX_BUILD_METAL)
    target_compile_definitions(Graphics PUBLIC GFX_BUILD_METAL)
endif()
if (GFX_BUILD_OPENGL)
    if(APPLE)
        target_compile_definitions(Graphics PUBLIC GL_SILENCE_DEPRECATION)
    endif()
    target_compile_definitions(Graphics PUBLIC GFX_BUILD_OPENGL)
endif()


add_subdirectory("dependencies")

if (GFX_USED_WINDOW_LIBRARY STREQUAL "GLFW")
    set(GLFW_BUILD_TESTS OFF)
    set(GLFW_BUILD_DOCS  OFF)
    set(GLFW_INSTALL     OFF)

    FetchContent_Declare(glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG        3.4
        GIT_SHALLOW    1
    )
    FetchContent_MakeAvailable(glfw)

    get_target_property(GLFW_FOLDER glfw FOLDER)
    set_target_properties(glfw PROPERTIES FOLDER "dependencies/${GLFW_FOLDER}")
    get_target_property(UPDATE_MAP_FOLDER update_mappings FOLDER)
    set_target_properties(update_mappings PROPERTIES FOLDER "dependencies/${UPDATE_MAP_FOLDER}")

    target_compile_options(glfw PRIVATE "-w")

    target_link_libraries(Graphics PRIVATE glfw)
endif()

if (GFX_BUILD_METAL)
    target_link_libraries(Graphics PRIVATE "-framework Metal -framework QuartzCore")
endif()
    
if (GFX_BUILD_OPENGL)
    FetchContent_Declare(glad
        GIT_REPOSITORY https://github.com/Thomas-Chqt/GLAD.git
        GIT_TAG        d12a9405aeba141e7c6808ebba218a944326f72b
        GIT_SHALLOW    1
    )
    FetchContent_MakeAvailable(glad)
    set_target_properties(glad PROPERTIES FOLDER "dependencies")
    target_link_libraries(Graphics PRIVATE glad)
endif()

if(GFX_BUILD_IMGUI)
    if (GFX_USED_WINDOW_LIBRARY STREQUAL "GLFW")
        set(IM_BUILD_GLFW ON)
    endif()
    set(IM_BUILD_METAL ${GFX_BUILD_METAL})
    set(IM_BUILD_OPENGL3 ${GFX_BUILD_OPENGL})
    if(NOT GFX_INSTALL)
        set(IM_INSTALL OFF)
    endif()
    FetchContent_Declare(imgui
        GIT_REPOSITORY https://github.com/Thomas-Chqt/imgui.git
        GIT_TAG        fabbe27effebeeb452c243b9b3dd6d139f6313d5
        GIT_SHALLOW    1
    )
    FetchContent_MakeAvailable(imgui)
    set_target_properties(imgui PROPERTIES FOLDER "dependencies")
    if (GFX_USED_WINDOW_LIBRARY STREQUAL "GLFW")
        target_link_libraries(imgui PRIVATE glfw)
    endif()
    if (GFX_BUILD_METAL)
        target_link_libraries(imgui PRIVATE "-framework Metal -framework QuartzCore")
    endif()
    if (GFX_BUILD_OPENGL)
        target_link_libraries(imgui PRIVATE glad)
    endif()
    target_link_libraries(Graphics PUBLIC imgui)
endif()

if(GFX_BUILD_EXEMPLES)
    add_subdirectory("exemples")
endif()

if(GFX_INSTALL)
    install(TARGETS Graphics
        RUNTIME DESTINATION "bin"
        LIBRARY DESTINATION "lib"
        ARCHIVE DESTINATION "lib"
    )
    install(DIRECTORY "include/" DESTINATION "include")

    install(TARGETS glfw
        RUNTIME DESTINATION "bin"
        LIBRARY DESTINATION "lib"
        ARCHIVE DESTINATION "lib"
    )

    install(TARGETS glad
        RUNTIME DESTINATION "bin"
        LIBRARY DESTINATION "lib"
        ARCHIVE DESTINATION "lib"
    )    
endif()