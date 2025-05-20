# ---------------------------------------------------
# CMakeLists.txt
#
# Author: Thomas Choquet <thomas.publique@icloud.com>
# Date: 2024/05/15 13:50:09
# ---------------------------------------------------

include(FetchContent)

FetchContent_Declare(assimp GIT_REPOSITORY https://github.com/assimp/assimp.git GIT_TAG v5.4.1 GIT_SHALLOW 1 GIT_PROGRESS TRUE)
set(ASSIMP_BUILD_TESTS OFF)
set(ASSIMP_INSTALL OFF)
set(ASSIMP_NO_EXPORT OFF)
set(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF)
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF)
set(ASSIMP_BUILD_OBJ_IMPORTER ON)
set(ASSIMP_BUILD_FBX_IMPORTER ON)
set(ASSIMP_BUILD_GLTF_IMPORTER ON)
set(CMAKE_POLICY_DEFAULT_CMP0175 OLD)
FetchContent_MakeAvailable(assimp)
unset(CMAKE_POLICY_DEFAULT_CMP0175)
if(TARGET zlibstatic OR TARGET UpdateAssimpLibsDebugSymbolsAndDLLs)
    set_target_properties(assimp PROPERTIES FOLDER "dependencies/assimp")
    if(TARGET zlibstatic)
        set_target_properties(zlibstatic PROPERTIES FOLDER "dependencies/assimp")
    endif()
    if(TARGET UpdateAssimpLibsDebugSymbolsAndDLLs)
        set_target_properties(UpdateAssimpLibsDebugSymbolsAndDLLs PROPERTIES FOLDER "dependencies/assimp")
    endif()
else()
    set_target_properties(assimp PROPERTIES FOLDER "dependencies")
endif()

add_subdirectory(triangle)

if(GFX_BUILD_IMGUI)
    add_subdirectory(imguiDemo)
endif()

add_subdirectory(texturedSquare)

if(GFX_BUILD_IMGUI)
    add_subdirectory(offscreenRenderPass)
endif()

if(GFX_BUILD_IMGUI)
    add_subdirectory(materialEditor)
endif()
