# ---------------------------------------------------
# CMakeLists.txt
#
# Author: Thomas Choquet <thomas.publique@icloud.com>
# Date: 2024/05/15 13:43:58
# ---------------------------------------------------

cmake_minimum_required(VERSION 3.22)

include(FetchContent)

project(Graphics)

option(BUILD_SHARED_LIBS "Build using shared libraries" OFF)

if (APPLE)
    option(GFX_BUILD_METAL "Build the metal backend"              ON)
endif()
option(GFX_BUILD_VULKAN    "Build the vulkan backend"             ON)
option(GFX_USE_UTILSCPP    "Use the UtilsCPP library"            OFF)
option(GFX_USE_MATH        "Use a custom math library"           OFF)
option(GFX_ENABLE_IMGUI    "Build with imgui capabilities"       OFF)
option(GFX_ENABLE_GLFW     "Build with glfw capabilities"        OFF)
option(GFX_BUILD_EXAMPLES  "Build Graphics examples executables" OFF)
option(GFX_INSTALL         "Enable the install command"           ON)

if (GFX_USE_UTILSCPP OR GFX_USE_MATH)
    message(FATAL_ERROR "Not yet implemented")
endif()

set(FETCHCONTENT_QUIET FALSE)

enable_language(CXX)

if(APPLE AND GFX_BUILD_METAL)
    enable_language(OBJCXX)
endif()

if (APPLE AND GFX_BUILD_METAL)
    find_library(METAL_FRAMEWORK Metal)
    if (METAL_FRAMEWORK-NOTFOUND)
        message(WARNING "Metal not found, disabling metal backend")
        set(GFX_BUILD_METAL OFF CACHE BOOL "Build the metal backend" FORCE)
    else()
        message(STATUS "Metal framework found")
    endif()
    find_library(QUARTZ_CORE_FRAMEWORK QuartzCore)
    if (QUARTZ_CORE_FRAMEWORK-NOTFOUND)
        message(WARNING "QuartzCore not found, disabling metal backend")
        set(GFX_BUILD_METAL OFF CACHE BOOL "Build the metal backend" FORCE)
    else()
        message(STATUS "QuartzCore framework found")
    endif()
    find_library(FOUNDATION_FRAMEWORK Foundation)
    if (FOUNDATION_FRAMEWORK-NOTFOUND)
        message(WARNING "Foundation not found, disabling metal backend")
        set(GFX_BUILD_METAL OFF CACHE BOOL "Build the metal backend" FORCE)
    else()
        message(STATUS "Foundation framework found")
    endif()
endif()

if (GFX_BUILD_VULKAN)
    find_package(Vulkan QUIET)
    if (NOT Vulkan_FOUND)
        message(WARNING "Vulkan not found, disabling vulkan backend")
        set(GFX_BUILD_VULKAN OFF CACHE BOOL "Build the vulkan backend" FORCE)
    else()
        message(STATUS "Vulkan found")
    endif()
endif()

if (NOT GFX_BUILD_METAL AND NOT GFX_BUILD_VULKAN)
    message(FATAL_ERROR "One backend must be build")
endif()

if (GFX_USE_UTILSCPP)
    if(NOT TARGET UtilsCPP)
        find_program(GIT_PATH git REQUIRED)
        execute_process(COMMAND ${GIT_PATH} submodule update --init dependencies/UtilsCPP WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
        set(UTILSCPP_BUILD_TESTS OFF)
        set(UTILSCPP_INSTALL ${GFX_INSTALL})
        add_subdirectory(UtilsCPP)
        set_target_properties(UtilsCPP PROPERTIES FOLDER "dependencies")
    endif()

    if (GFX_USE_MATH)
        if(NOT TARGET Math)
            find_program(GIT_PATH git REQUIRED)
            execute_process(COMMAND ${GIT_PATH} submodule update --init dependencies/Math WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
            set(MATH_BUILD_TESTS OFF)
            set(MATH_INSTALL ${GFX_INSTALL})
            add_subdirectory(Math)
            set_target_properties(Math PROPERTIES FOLDER "dependencies")
        endif()
    endif()
endif()

if (GFX_ENABLE_GLFW OR GFX_ENABLE_IMGUI)
    FetchContent_Declare(dlLoad
        GIT_REPOSITORY    https://github.com/Thomas-Chqt/dlLoad.git
        GIT_TAG           c31964cc24d009324ae3de1574fa00dcf3c2c113
        GIT_SHALLOW       1
        GIT_PROGRESS      TRUE
        FIND_PACKAGE_ARGS
    )
    set(DL_BUILD_TESTS OFF)
    set(DL_INSTALL     ${GFX_INSTALL})
    FetchContent_MakeAvailable(dlLoad)
    set_target_properties(dlLoad PROPERTIES FOLDER "dependencies")
endif()

if (GFX_BUILD_VULKAN)
    FetchContent_Declare(VulkanMemoryAllocator
        GIT_REPOSITORY    https://github.com/Thomas-Chqt/VulkanMemoryAllocator.git
        GIT_TAG           210b7512cba580da650b6eebcea1e01d0e34dbbf
        GIT_SHALLOW       1
        GIT_PROGRESS      TRUE
        FIND_PACKAGE_ARGS
    )
    FetchContent_MakeAvailable(VulkanMemoryAllocator)
    target_link_libraries(VulkanMemoryAllocator PRIVATE Vulkan::Vulkan)
endif()

add_library(Graphics ${GFX_LIB_TYPE}) # Use GFX_LIB_TYPE to override BUILD_SHARED_LIBS for `Graphics`

target_compile_features(Graphics PUBLIC cxx_std_23)

if(MSVC)
    target_compile_options(Graphics PRIVATE /W4)
else()
    target_compile_options(Graphics PRIVATE -Wall -Wextra -Wpedantic)
endif()

file(GLOB_RECURSE GFX_INC "include/*.hpp")
target_sources(Graphics PRIVATE ${GFX_INC})

file(GLOB GFX_SRC "src/*.cpp" "src/*.hpp" "src/*.h")
if (NOT GFX_ENABLE_IMGUI)
    list(REMOVE_ITEM GFX_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/imconfig.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/imgui.h")
endif()
target_sources(Graphics PRIVATE ${GFX_SRC})

if (GFX_BUILD_METAL)
    file(GLOB_RECURSE METAL_SRC "src/Metal/*.cpp" "src/Metal/*.mm" "src/Metal/*.hpp" "src/Metal/*.h")
    if (NOT GFX_ENABLE_IMGUI)
        list(REMOVE_ITEM METAL_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/Metal/imgui_impl_metal.mm" "${CMAKE_CURRENT_SOURCE_DIR}/src/Metal/imgui_impl_metal.h")
    else()
        # set_source_files_properties("src/Metal/imgui_impl_metal.mm" PROPERTIES COMPILE_FLAGS -fobjc-arc)
    endif()
    target_sources(Graphics PRIVATE ${METAL_SRC})
endif()

if (GFX_BUILD_VULKAN)
    file(GLOB_RECURSE VULKAN_SRC "src/Vulkan/*.cpp" "src/Vulkan/*.hpp" "src/Vulkan/*.h")
    if (NOT GFX_ENABLE_IMGUI)
        list(REMOVE_ITEM VULKAN_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/Vulkan/imgui_impl_vulkan.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/Vulkan/imgui_impl_vulkan.h")
    endif()
    target_sources(Graphics PRIVATE ${VULKAN_SRC})
endif()

target_include_directories(Graphics PRIVATE "src" PUBLIC "include")
target_precompile_headers(Graphics PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/pch.hpp")

if (GFX_BUILD_METAL)
    target_compile_definitions(Graphics PUBLIC "GFX_BUILD_METAL")
endif()

if (GFX_BUILD_VULKAN)
    target_compile_definitions(Graphics PUBLIC "GFX_BUILD_VULKAN")
    target_compile_definitions(Graphics PUBLIC "VULKAN_HPP_NO_CONSTRUCTORS")
    target_compile_definitions(Graphics PUBLIC "VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1")
    target_compile_definitions(Graphics PUBLIC "VMA_STATIC_VULKAN_FUNCTIONS=0")
    target_compile_definitions(Graphics PUBLIC "VMA_DYNAMIC_VULKAN_FUNCTIONS=1")
    target_compile_definitions(Graphics PUBLIC "VULKAN_HPP_HANDLES_MOVE_EXCHANGE=1")
endif()

if (GFX_ENABLE_IMGUI)
    target_compile_definitions(Graphics PUBLIC "GFX_IMGUI_ENABLED")
endif()

if (GFX_ENABLE_GLFW)
    target_compile_definitions(Graphics PUBLIC "GFX_GLFW_ENABLED")
endif()

if (GFX_USE_UTILSCPP)
    target_compile_definitions(Graphics PUBLIC "GFX_USE_UTILSCPP")
endif()

if (GFX_USE_MATH)
    target_compile_definitions(Graphics PUBLIC "GFX_USE_MATH")
endif()

if (GFX_BUILD_METAL)
    target_link_libraries(Graphics PRIVATE ${METAL_FRAMEWORK} ${QUARTZ_CORE_FRAMEWORK} ${FOUNDATION_FRAMEWORK})
endif()

if (GFX_BUILD_VULKAN)
    target_link_libraries(Graphics PRIVATE Vulkan::Vulkan)
endif()

if (GFX_USE_UTILSCPP)
    target_link_libraries(Graphics PUBLIC UtilsCPP)
    if (GFX_USE_MATH)
        target_link_libraries(Graphics PUBLIC Math)
    endif()
endif()

if (GFX_ENABLE_GLFW OR GFX_ENABLE_IMGUI)
    target_link_libraries(Graphics PRIVATE dlLoad)
endif()

target_link_libraries(Graphics PRIVATE VulkanMemoryAllocator)

add_subdirectory("tools")

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
