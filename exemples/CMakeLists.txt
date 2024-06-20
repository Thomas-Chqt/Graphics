# ---------------------------------------------------
# CMakeLists.txt
#
# Author: Thomas Choquet <thomas.publique@icloud.com>
# Date: 2024/05/15 13:50:09
# ---------------------------------------------------

FetchContent_Declare(assimp GIT_REPOSITORY https://github.com/assimp/assimp.git GIT_TAG v5.4.1 GIT_SHALLOW 1)
set(ASSIMP_BUILD_TESTS OFF)
set(ASSIMP_INSTALL OFF)
set(ASSIMP_NO_EXPORT OFF)
set(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF)
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF)
set(ASSIMP_BUILD_OBJ_IMPORTER ON)
FetchContent_MakeAvailable(assimp)
set_target_properties(assimp PROPERTIES FOLDER "dependencies")

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

if(GFX_BUILD_IMGUI)
    add_subdirectory(diffuseLight)
endif()

if(GFX_BUILD_IMGUI)
    add_subdirectory(phongLight)
endif()