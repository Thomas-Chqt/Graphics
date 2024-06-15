# ---------------------------------------------------
# CMakeLists.txt
#
# Author: Thomas Choquet <thomas.publique@icloud.com>
# Date: 2024/05/15 13:50:09
# ---------------------------------------------------

add_subdirectory(triangle)

add_subdirectory(setClearColor)

if(GFX_BUILD_IMGUI)
    add_subdirectory(imguiDemo)
endif()

add_subdirectory(indexedShape)

if(GFX_BUILD_IMGUI)
    add_subdirectory(fragmentUniform)
endif()

if(GFX_BUILD_IMGUI)
    add_subdirectory(flatColorCube)
endif()

add_subdirectory(texturedSquare)

add_subdirectory(offscreenRenderPass)

if(GFX_BUILD_IMGUI)
    add_subdirectory(texturedCube)
endif()

if(GFX_BUILD_IMGUI)
    add_subdirectory(camera)
endif()

if(GFX_BUILD_IMGUI)
    add_subdirectory(ambiantLight)
endif()