# ---------------------------------------------------
# CMakeLists.txt
#
# Author: Thomas Choquet <thomas.publique@icloud.com>
# Date: 2024/05/15 13:43:58
# ---------------------------------------------------

cmake_minimum_required(VERSION 3.22)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(FetchContent)

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(CMAKE_WARN_DEPRECATED OFF CACHE BOOL "")
set(FETCHCONTENT_QUIET OFF)

project(Graphics)

option(BUILD_SHARED_LIBS "Build using shared libraries" OFF)

if (APPLE)
    option(GFX_BUILD_METAL "Build the metal backend" ON)
endif()
option(GFX_BUILD_VULKAN   "Build the vulkan backend"             ON)
option(GFX_ENABLE_IMGUI   "Build with imgui capabilities"       OFF)
option(GFX_ENABLE_GLFW    "Build with glfw capabilities"        OFF)
option(GFX_BUILD_EXAMPLES "Build Graphics examples executables" OFF)
option(GFX_INSTALL        "Enable the install command"           ON)

if (GFX_USE_UTILSCPP OR GFX_USE_MATH)
    message(FATAL_ERROR "Not yet implemented")
endif()

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
    FetchContent_Declare(Vulkan
        GIT_REPOSITORY    https://github.com/KhronosGroup/Vulkan-Headers.git
        GIT_TAG           vulkan-sdk-1.4.321.0
        GIT_SHALLOW       1
        GIT_PROGRESS      TRUE
        FIND_PACKAGE_ARGS
    )
    FetchContent_MakeAvailable(Vulkan)
endif()

if (NOT GFX_BUILD_METAL AND NOT GFX_BUILD_VULKAN)
    message(FATAL_ERROR "One backend must be build")
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
    target_compile_definitions(VulkanMemoryAllocator PUBLIC "VMA_STATIC_VULKAN_FUNCTIONS=0")
    target_compile_definitions(VulkanMemoryAllocator PUBLIC "VMA_DYNAMIC_VULKAN_FUNCTIONS=1")
    target_link_libraries(VulkanMemoryAllocator PRIVATE Vulkan::Headers)
    set_target_properties(VulkanMemoryAllocator PROPERTIES FOLDER "dependencies")
endif()

add_library(Graphics ${GFX_LIB_TYPE}) # Use GFX_LIB_TYPE to override BUILD_SHARED_LIBS for `Graphics`

target_compile_features(Graphics PUBLIC cxx_std_23)

if(MSVC)
    target_compile_options(Graphics PRIVATE /W4)
else()
    target_compile_options(Graphics PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(Graphics PRIVATE $<$<CXX_COMPILER_ID:GNU>:-Wno-missing-field-initializers>)
endif()

file(GLOB_RECURSE GFX_INC "include/*.hpp")
target_sources(Graphics PRIVATE ${GFX_INC})

file(GLOB GFX_SRC "src/*.cpp" "src/*.hpp" "src/*.h")
if (NOT GFX_ENABLE_IMGUI)
    list(REMOVE_ITEM GFX_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/imconfig.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/imgui.h")
endif()
target_sources(Graphics PRIVATE ${GFX_SRC})

if (GFX_BUILD_METAL)
    file(GLOB_RECURSE GFX_METAL_SRC "src/Metal/*.cpp" "src/Metal/*.mm" "src/Metal/*.hpp" "src/Metal/*.h")
    if (NOT GFX_ENABLE_IMGUI)
        list(REMOVE_ITEM GFX_METAL_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/Metal/imgui_impl_metal.mm" "${CMAKE_CURRENT_SOURCE_DIR}/src/Metal/imgui_impl_metal.h")
    endif()
    target_sources(Graphics PRIVATE ${GFX_METAL_SRC})
endif()

if (GFX_BUILD_VULKAN)
    file(GLOB_RECURSE GFX_VULKAN_SRC "src/Vulkan/*.cpp" "src/Vulkan/*.hpp" "src/Vulkan/*.h")
    if (NOT GFX_ENABLE_IMGUI)
        list(REMOVE_ITEM GFX_VULKAN_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/Vulkan/imgui_impl_vulkan.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/Vulkan/imgui_impl_vulkan.h")
    endif()
    target_sources(Graphics PRIVATE ${GFX_VULKAN_SRC})
endif()

target_include_directories(Graphics PRIVATE "src" PUBLIC "include")
target_precompile_headers(Graphics PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/pch.hpp")

if (GFX_BUILD_VULKAN AND GFX_ENABLE_IMGUI)
    set_source_files_properties("${CMAKE_CURRENT_SOURCE_DIR}/src/Vulkan/imgui_impl_vulkan.cpp" PROPERTIES SKIP_PRECOMPILE_HEADERS ON)
endif()

if (GFX_BUILD_METAL)
    target_compile_definitions(Graphics PUBLIC "GFX_BUILD_METAL")
endif()

if (GFX_BUILD_VULKAN)
    target_compile_definitions(Graphics PUBLIC "GFX_BUILD_VULKAN")
    target_compile_definitions(Graphics PRIVATE "VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1")
    target_compile_definitions(Graphics PRIVATE "VULKAN_HPP_NO_CONSTRUCTORS")
    target_compile_definitions(Graphics PRIVATE "VULKAN_HPP_HANDLES_MOVE_EXCHANGE=1")
endif()

if (GFX_ENABLE_IMGUI)
    target_compile_definitions(Graphics PUBLIC "GFX_IMGUI_ENABLED")
endif()

if (GFX_ENABLE_GLFW)
    target_compile_definitions(Graphics PUBLIC "GFX_GLFW_ENABLED")
endif()

if (GFX_BUILD_METAL)
    target_link_libraries(Graphics PRIVATE ${METAL_FRAMEWORK} ${QUARTZ_CORE_FRAMEWORK} ${FOUNDATION_FRAMEWORK})
endif()

if (GFX_BUILD_VULKAN)
    target_link_libraries(Graphics PRIVATE Vulkan::Headers VulkanMemoryAllocator)
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

add_subdirectory("tools")

if (GFX_BUILD_METAL)
    set(GFX_SHADER_SLIB "${CMAKE_CURRENT_BINARY_DIR}/shaders/gfx.slib")
    file(GLOB GFX_SHADER_SRCS "shaders/*.slang")
    add_custom_command(
        OUTPUT ${GFX_SHADER_SLIB}
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/shaders"
        COMMAND $<TARGET_FILE:gfxsc> -t metal -o ${GFX_SHADER_SLIB} ${GFX_SHADER_SRCS}
        DEPENDS gfxsc ${GFX_SHADER_SRCS}
        COMMENT "Building gfx shaders"
        VERBATIM
    )
    add_custom_target(gfx_shaders ALL DEPENDS ${GFX_SHADER_SLIB})
    set_target_properties(gfx_shaders PROPERTIES FOLDER "shaders")
    target_compile_definitions(Graphics PRIVATE "GFX_SHADER_SLIB=\"${GFX_SHADER_SLIB}\"")
    add_dependencies(Graphics gfx_shaders)
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
