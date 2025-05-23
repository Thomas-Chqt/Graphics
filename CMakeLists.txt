# ---------------------------------------------------
# CMakeLists.txt
#
# Author: Thomas Choquet <thomas.publique@icloud.com>
# Date: 2024/05/15 13:43:58
# ---------------------------------------------------

cmake_minimum_required(VERSION 3.22)

include(FetchContent)

project(Graphics)

option(BUILD_SHARED_LIBS "Build using shared libraries"    OFF)

option(GFX_BUILD_EXAMPLES "Build Graphics examples executables" OFF)
option(GFX_ENABLE_IMGUI   "Build with imgui capabilities"       OFF)
option(GFX_ENABLE_GLFW    "Build with glfw capabilities"        OFF)
option(GFX_INSTALL        "Enable the install command"           ON)

if (APPLE)
    option(GFX_BUILD_METAL "Build the metal backed" ON)
    if (GFX_BUILD_METAL)
        find_library(METAL_FRAMEWORK Metal)
        if (METAL_FRAMEWORK-NOTFOUND)
            message(WARNING "Metal not found, disabling metal backend")
            set(GFX_BUILD_METAL OFF CACHE BOOL "Build the metal backed" FORCE)
        else()
            message(STATUS "Metal framework found")
        endif()
    endif()
endif()

option(GFX_BUILD_VULKAN "Build the vulkan backed" ON)
if (GFX_BUILD_VULKAN)
    find_package(Vulkan)
    if (NOT Vulkan_FOUND)
        message(WARNING "Vulkan not found, disabling vulkan backend")
        set(GFX_BUILD_VULKAN OFF CACHE BOOL "Build the vulkan backed" FORCE)
    endif()
endif()

if (NOT GFX_BUILD_METAL AND NOT GFX_BUILD_VULKAN)
    message(FATAL_ERROR "One backed must be build")
endif()


enable_language(CXX)

if(GFX_BUILD_METAL)
    enable_language(OBJCXX)
endif()

add_library(Graphics ${GFX_LIB_TYPE}) # Use GFX_LIB_TYPE to override BUILD_SHARED_LIBS for `Graphics`

target_compile_features(Graphics PUBLIC cxx_std_20)

file(GLOB_RECURSE GRAPHICS_SRC "include/*.hpp")
target_sources(Graphics PRIVATE ${GRAPHICS_SRC})

add_subdirectory("src")

target_include_directories(Graphics PRIVATE "src" PUBLIC "include")

if (GFX_BUILD_METAL)
    target_compile_definitions(Graphics PUBLIC "GFX_BUILD_METAL")
endif()

if (GFX_BUILD_VULKAN)
    target_compile_definitions(Graphics PUBLIC "GFX_BUILD_VULKAN")
endif()

if (GFX_ENABLE_IMGUI)
    target_compile_definitions(Graphics PUBLIC "GFX_IMGUI_ENABLED")
endif()

if (GFX_ENABLE_GLFW)
    target_compile_definitions(Graphics PUBLIC "GFX_GLFW_ENABLED")
endif()

add_subdirectory("dependencies")

if (GFX_BUILD_METAL)
    target_link_libraries(Graphics PRIVATE ${METAL_FRAMEWORK})
endif()
    
if (GFX_BUILD_VULKAN)
    target_link_libraries(Graphics PRIVATE Vulkan::Vulkan)
endif()

if(GFX_BUILD_EXAMPLES)
    add_subdirectory("examples")
endif()

if(GFX_INSTALL)
    install(TARGETS Graphics
        RUNTIME DESTINATION "bin"
        LIBRARY DESTINATION "lib"
        ARCHIVE DESTINATION "lib"
    )
    install(DIRECTORY "include/" DESTINATION "include")
endif()
