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
    option(GFX_BUILD_METAL "Build the metal backed"               ON)
endif()
option(GFX_BUILD_VULKAN    "Build the vulkan backed"              ON)
option(GFX_USE_UTILSCPP    "Use the UtilsCPP library"            OFF)
option(GFX_USE_MATH        "Use a custom math library"           OFF)
option(GFX_ENABLE_IMGUI    "Build with imgui capabilities"       OFF)
option(GFX_ENABLE_GLFW     "Build with glfw capabilities"        OFF)
option(GFX_BUILD_EXAMPLES  "Build Graphics examples executables" OFF)
option(GFX_INSTALL         "Enable the install command"           ON)

if (GFX_USE_UTILSCPP OR GFX_USE_MATH)
    message(FATAL_ERROR "Not yet implemented")
endif()

if (NOT GFX_BUILD_METAL AND NOT GFX_BUILD_VULKAN)
    message(FATAL_ERROR "One backed must be build")
endif()

set(FETCHCONTENT_QUIET FALSE)

enable_language(CXX)

if(GFX_BUILD_METAL)
    enable_language(OBJCXX)
endif()

if (APPLE AND GFX_BUILD_METAL)
    find_library(METAL_FRAMEWORK Metal)
    if (METAL_FRAMEWORK-NOTFOUND)
        message(WARNING "Metal not found, disabling metal backend")
        set(GFX_BUILD_METAL OFF CACHE BOOL "Build the metal backed" FORCE)
    else()
        message(STATUS "Metal framework found")
    endif()
    find_library(QUARTZ_CORE_FRAMEWORK QuartzCore)
    if (QUARTZ_CORE_FRAMEWORK-NOTFOUND)
        message(WARNING "QuartzCore not found, disabling metal backend")
        set(GFX_BUILD_METAL OFF CACHE BOOL "Build the metal backed" FORCE)
    else()
        message(STATUS "QuartzCore framework found")
    endif()
    find_library(FOUNDATION_FRAMEWORK Foundation)
    if (FOUNDATION_FRAMEWORK-NOTFOUND)
        message(WARNING "Foundation not found, disabling metal backend")
        set(GFX_BUILD_METAL OFF CACHE BOOL "Build the metal backed" FORCE)
    else()
        message(STATUS "Foundation framework found")
    endif()
endif()

if (GFX_BUILD_VULKAN)
    find_package(Vulkan)
    if (NOT Vulkan_FOUND)
        message(WARNING "Vulkan not found, disabling vulkan backend")
        set(GFX_BUILD_VULKAN OFF CACHE BOOL "Build the vulkan backed" FORCE)
    endif()
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

set(GFXSC_PATH ${CMAKE_CURRENT_LIST_DIR}/tools/gfxsc.py CACHE PATH "path to the gfx shader compiler" FORCE)
set(ADD_SHADER_LIB_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake/add_shader_lib.cmake CACHE PATH "path to the add_shader_lib cmake file" FORCE)

if (GFX_ENABLE_GLFW OR GFX_ENABLE_IMGUI)
    FetchContent_Declare(dlLoad
        GIT_REPOSITORY    https://github.com/Thomas-Chqt/dlLoad.git
        GIT_TAG           c31964cc24d009324ae3de1574fa00dcf3c2c113
        GIT_SHALLOW       1
        GIT_PROGRESS      TRUE
    )
    set(DL_BUILD_TESTS OFF)
    set(DL_INSTALL     OFF)
    FetchContent_MakeAvailable(dlLoad)
    set_target_properties(dlLoad PROPERTIES FOLDER "dependencies")
endif()

add_library(Graphics ${GFX_LIB_TYPE}) # Use GFX_LIB_TYPE to override BUILD_SHARED_LIBS for `Graphics`

target_compile_features(Graphics PUBLIC cxx_std_23)

file(GLOB_RECURSE GFX_SRC "include/*.hpp")
target_sources(Graphics PRIVATE ${GFX_SRC})

file(GLOB GFX_SRC "src/*.cpp" "src/*.hpp")
target_sources(Graphics PRIVATE ${GFX_SRC})

if (GFX_BUILD_METAL)
    file(GLOB_RECURSE GFX_SRC "src/Metal/*.cpp" "src/Metal/*.mm" "src/Metal/*.hpp")
    target_sources(Graphics PRIVATE ${GFX_SRC})
endif()

if (GFX_BUILD_VULKAN)
    file(GLOB_RECURSE GFX_SRC "src/Vulkan/*.cpp" "src/Metal/*.hpp")
    target_sources(Graphics PRIVATE ${GFX_SRC})
endif()

target_include_directories(Graphics PRIVATE "src" PUBLIC "include")

if (GFX_BUILD_METAL)
    target_compile_definitions(Graphics PUBLIC "GFX_BUILD_METAL")
endif()

if (GFX_BUILD_VULKAN)
    target_compile_definitions(Graphics PUBLIC "GFX_BUILD_VULKAN")
    target_compile_definitions(Graphics PUBLIC "VULKAN_HPP_NO_CONSTRUCTORS")
    target_compile_definitions(Graphics PUBLIC "VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1")
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
