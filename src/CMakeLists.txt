# ---------------------------------------------------
# CMakeLists.txt
#
# Author: Thomas Choquet <thomas.publique@icloud.com>
# Date: 2024/05/22 16:59:43
# ---------------------------------------------------

file(GLOB FILES "*.cpp" "*.tpp" "*.hpp")
list(APPEND GRAPHICS_SRC ${FILES})

################################################################################################################

file(GLOB FILES "Platform/*.cpp" "Platform/*.tpp" "Platform/*.hpp")
list(APPEND GRAPHICS_SRC ${FILES})

################################################################################################################

if(GFX_USED_WINDOW_LIBRARY STREQUAL GLFW)
    file(GLOB_RECURSE FILES "Platform/GLFW/*.cpp" "Platform/GLFW/*.tpp" "Platform/GLFW/*.hpp")
    list(APPEND GRAPHICS_SRC ${FILES})
endif()

################################################################################################################

file(GLOB_RECURSE FILES "Event/*.cpp" "Event/*.tpp" "Event/*.hpp")
list(APPEND GRAPHICS_SRC ${FILES})

################################################################################################################

list(APPEND GRAPHICS_SRC "Window/Window_internal.hpp")

if (GFX_ENABLE_METAL)
    list(APPEND GRAPHICS_SRC ${FILES} "Window/MetalWindow.hpp")
endif()
    
if (GFX_ENABLE_OPENGL)
    list(APPEND GRAPHICS_SRC ${FILES} "Window/OpenGLWindow.hpp")
endif()

if(GFX_USED_WINDOW_LIBRARY STREQUAL GLFW)
    list(APPEND GRAPHICS_SRC "Window/GLFW/GLFWWindow.cpp" "Window/GLFW/GLFWWindow.hpp")

    if (GFX_ENABLE_METAL)
        list(APPEND GRAPHICS_SRC "Window/GLFW/GLFWMetalWindow.mm" "Window/GLFW/GLFWMetalWindow.hpp")
    endif()
    if (GFX_ENABLE_OPENGL)
        list(APPEND GRAPHICS_SRC "Window/GLFW/GLFWOpenGLWindow.cpp" "Window/GLFW/GLFWOpenGLWindow.hpp")
    endif()
endif()

################################################################################################################

file(GLOB FILES "GraphicAPI/*.cpp" "GraphicAPI/*.tpp" "GraphicAPI/*.hpp")
list(APPEND GRAPHICS_SRC ${FILES})

if (GFX_ENABLE_METAL)
    file(GLOB_RECURSE FILES "GraphicAPI/Metal/*.cpp" "GraphicAPI/Metal/*.tpp" "GraphicAPI/Metal/*.hpp" "GraphicAPI/Metal/*.h" "GraphicAPI/Metal/*.mm")
    list(APPEND GRAPHICS_SRC ${FILES} "Window/MetalWindow.hpp")
endif()
if (GFX_ENABLE_OPENGL)
    file(GLOB_RECURSE FILES "GraphicAPI/OpenGL/*.cpp" "GraphicAPI/OpenGL/*.tpp" "GraphicAPI/OpenGL/*.hpp")
    list(APPEND GRAPHICS_SRC ${FILES} "Window/OpenGLWindow.hpp")
endif()

################################################################################################################

if (GFX_ENABLE_METAL)
    list(APPEND GRAPHICS_SRC "Utils/MetalPixelTypesConv.mm")
endif()
if (GFX_ENABLE_OPENGL)
    list(APPEND GRAPHICS_SRC "Utils/OpenGLPixelTypesConv.cpp")
endif()

################################################################################################################

target_sources(Graphics PRIVATE ${GRAPHICS_SRC})