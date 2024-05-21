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
option(FORCE_USE_GLFW "Use GLFW to handle windows even if the native API is directly usable" OFF)

if (APPLE AND NOT FORCE_USE_GLFW)
    set(USING_APPKIT ON)
else()
    set(USING_GLFW ON)
endif()

if (USING_APPKIT)
    set(USING_METAL ON)
    set(USING_OPENGL ON)
endif()
if (USING_GLFW)
    set(USING_OPENGL ON)
endif()
    
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (USING_APPKIT)
    enable_language(OBJCXX)
endif()

if(BUILD_SHARED_LIBS)
    add_library(Graphics SHARED)
else()
    add_library(Graphics STATIC)
endif()

file(GLOB GRAPHICS_SRC "src/*.cpp" "src/*.tpp" "src/*.hpp")
target_sources(Graphics PRIVATE ${GRAPHICS_SRC})

file(GLOB_RECURSE GRAPHICS_SRC "include/*.tpp" "include/*.hpp")
target_sources(Graphics PRIVATE ${GRAPHICS_SRC})

file(GLOB_RECURSE GRAPHICS_SRC "src/Event/*.cpp" "src/Event/*.tpp" "src/Event/*.hpp")
target_sources(Graphics PRIVATE ${GRAPHICS_SRC})

file(GLOB GRAPHICS_SRC "src/GraphicAPI/*.cpp" "src/GraphicAPI/*.tpp" "src/GraphicAPI/*.hpp")
target_sources(Graphics PRIVATE ${GRAPHICS_SRC})

if (USING_METAL)
    file(GLOB_RECURSE GRAPHICS_SRC "src/GraphicAPI/Metal/*.cpp" "src/GraphicAPI/Metal/*.tpp" "src/GraphicAPI/Metal/*.hpp" "src/GraphicAPI/Metal/*.h" "src/GraphicAPI/Metal/*.mm")
    target_sources(Graphics PRIVATE ${GRAPHICS_SRC})
endif()
    
if (USING_OPENGL)
    file(GLOB_RECURSE GRAPHICS_SRC "src/GraphicAPI/OpenGL/*.cpp" "src/GraphicAPI/OpenGL/*.tpp" "src/GraphicAPI/OpenGL/*.hpp")
    target_sources(Graphics PRIVATE ${GRAPHICS_SRC})    
endif()

file(GLOB GRAPHICS_SRC "src/Platform/*.cpp" "src/Platform/*.tpp" "src/Platform/*.hpp")
target_sources(Graphics PRIVATE ${GRAPHICS_SRC})

if (USING_APPKIT)
    file(GLOB_RECURSE GRAPHICS_SRC "src/Platform/MacOS/*.cpp" "src/Platform/MacOS/*.tpp" "src/Platform/MacOS/*.hpp" "src/Platform/MacOS/*.h" "src/Platform/MacOS/*.mm")
    target_sources(Graphics PRIVATE ${GRAPHICS_SRC})
endif()

if(USING_GLFW)
    file(GLOB_RECURSE GRAPHICS_SRC "src/Platform/GLFW/*.cpp" "src/Platform/GLFW/*.tpp" "src/Platform/GLFW/*.hpp")
    target_sources(Graphics PRIVATE ${GRAPHICS_SRC})
endif()

file(GLOB GRAPHICS_SRC "src/Window/*.cpp" "src/Window/*.tpp" "src/Window/*.hpp")
target_sources(Graphics PRIVATE ${GRAPHICS_SRC})

if (USING_APPKIT)
    file(GLOB_RECURSE GRAPHICS_SRC "src/Window/MacOS/*.cpp" "src/Window/MacOS/*.tpp" "src/Window/MacOS/*.hpp" "src/Window/MacOS/*.h" "src/Window/MacOS/*.mm")
    target_sources(Graphics PRIVATE ${GRAPHICS_SRC})
endif()

if(USING_GLFW)
    file(GLOB_RECURSE GRAPHICS_SRC "src/Window/GLFW/*.cpp" "src/Window/GLFW/*.tpp" "src/Window/GLFW/*.hpp")
    target_sources(Graphics PRIVATE ${GRAPHICS_SRC})
endif()

if (USING_APPKIT)
    target_compile_definitions(Graphics PUBLIC USING_APPKIT)
endif()

if (USING_GLFW)
    target_compile_definitions(Graphics PUBLIC USING_GLFW)
endif()

if (APPLE AND USING_OPENGL)
    target_compile_definitions(Graphics PUBLIC GL_SILENCE_DEPRECATION)
endif()

if (USING_OPENGL)
    target_compile_definitions(Graphics PUBLIC USING_OPENGL)
endif()

if (USING_METAL)
    target_compile_definitions(Graphics PUBLIC USING_METAL)
endif()

target_include_directories(Graphics PRIVATE "src" PUBLIC "include")

if (APPLE AND USING_APPKIT)
    target_link_libraries(Graphics PUBLIC "-framework AppKit")
endif()

if (APPLE AND USING_METAL)
    target_link_libraries(Graphics PUBLIC "-framework Metal -framework QuartzCore")
endif()

if (USING_GLFW)
    FetchContent_MakeAvailable(glfw)
    target_link_libraries(Graphics PUBLIC glfw)
endif()
    
if (USING_OPENGL)
    if(BUILD_SHARED_LIBS)
        set(glew-cmake_BUILD_STATIC NO)
    else()
        set(glew-cmake_BUILD_SHARED NO)
    endif()
    set(ONLY_LIBS ON)
    FetchContent_MakeAvailable(glew-cmake)
    if(BUILD_SHARED_LIBS)
        target_link_libraries(Graphics PUBLIC libglew_shared)
    else()
        target_link_libraries(Graphics PUBLIC libglew_static)
    endif()
endif()
    
add_subdirectory(dependencies)

if (GRAPHICS_BUILD_TESTS)
    add_subdirectory(tests)
endif()