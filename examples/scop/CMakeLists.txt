# ---------------------------------------------------
# CMakeLists.txt
#
# Author: Thomas Choquet <semoir.dense-0h@icloud.com>
# Date: 2025/09/16 07:05:47
# ---------------------------------------------------

include(FetchContent)

## GLFW ##########################################################

FetchContent_Declare(glfw3
    GIT_REPOSITORY    https://github.com/glfw/glfw.git
    GIT_TAG           3.4
    GIT_SHALLOW       1
    GIT_PROGRESS      TRUE
    FIND_PACKAGE_ARGS
)
set(GLFW_BUILD_TESTS    OFF)
set(GLFW_BUILD_DOCS     OFF)
set(GLFW_INSTALL        OFF)
set(GLFW_BUILD_EXAMPLES OFF)
if(NOT GLFW_BUILD_WAYLAND)
    set(GLFW_BUILD_WAYLAND OFF)
endif()
if (APPLE)
    enable_language(OBJC)
endif()
FetchContent_MakeAvailable(glfw3)
if (glfw3_SOURCE_DIR)
    if (TARGET update_mappings)
        set_target_properties(glfw PROPERTIES FOLDER "dependencies/GLFW3")
        set_target_properties(update_mappings PROPERTIES FOLDER "dependencies/GLFW3")
    else()
        set_target_properties(glfw PROPERTIES FOLDER "dependencies")
    endif()
endif()

## GLM ###############################################################

FetchContent_Declare(gml
    GIT_REPOSITORY    https://github.com/g-truc/glm.git
    GIT_TAG           1.0.1
    GIT_SHALLOW       1
    GIT_PROGRESS      TRUE
    FIND_PACKAGE_ARGS NAMES glm
)
FetchContent_MakeAvailable(gml)
if (gml_SOURCE_DIR)
    set_target_properties(glm PROPERTIES FOLDER "dependencies")
endif()

## IMGUI ###############################################################

FetchContent_Declare(imgui
    GIT_REPOSITORY    https://github.com/Thomas-Chqt/imgui.git
    GIT_TAG           4d8f55183c2e974916daf6336ee2c4b9c8e72891
    GIT_SHALLOW       1
    GIT_PROGRESS      TRUE
    FIND_PACKAGE_ARGS
)
set(IM_BUILD_GLFW ON)
FetchContent_MakeAvailable(imgui)
target_compile_definitions(imgui PRIVATE GLFW_INCLUDE_NONE)
target_link_libraries(imgui PUBLIC glfw)
if (imgui_SOURCE_DIR)
    set_target_properties(imgui PROPERTIES FOLDER "dependencies")
endif()

## STB_IMAGE ###############################################################

FetchContent_Declare(stb_image
    GIT_REPOSITORY    https://github.com/Thomas-Chqt/stb_image.git
    GIT_TAG           a09b799a2ba95c14f511493db4ea59811b2fcc97
    GIT_SHALLOW       1
    GIT_PROGRESS      TRUE
    FIND_PACKAGE_ARGS
)
FetchContent_MakeAvailable(stb_image)
if (stb_image_SOURCE_DIR)
    set_target_properties(stb_image PROPERTIES FOLDER "dependencies")
endif()

## ASSIMP ###################################################################

FetchContent_Declare(assimp
    GIT_REPOSITORY    https://github.com/assimp/assimp.git
    GIT_TAG           v5.4.1
    GIT_SHALLOW       1
    GIT_PROGRESS      TRUE
    FIND_PACKAGE_ARGS
)
set(ASSIMP_BUILD_TESTS                    OFF)
set(ASSIMP_INSTALL                        OFF)
set(ASSIMP_NO_EXPORT                      ON)
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF)
set(ASSIMP_BUILD_OBJ_IMPORTER             ON)
set(ASSIMP_BUILD_FBX_IMPORTER             ON)
set(ASSIMP_BUILD_GLTF_IMPORTER            ON)
set(CMAKE_POLICY_DEFAULT_CMP0175 OLD)
FetchContent_MakeAvailable(assimp)
unset(CMAKE_POLICY_DEFAULT_CMP0175)
if (assimp_SOURCE_DIR)
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
endif()

##################################################################

add_subdirectory(shaders)

add_executable(scop)

if (APPLE)
    enable_language(OBJC)
endif()

set_target_properties(scop PROPERTIES
    FOLDER         "examples/scop"
    ENABLE_EXPORTS ON
)

target_compile_features(scop PRIVATE cxx_std_23)

if(MSVC)
    target_compile_options(scop PRIVATE /W4)
else()
    target_compile_options(scop PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(scop PRIVATE $<$<CXX_COMPILER_ID:GNU>:-Wno-missing-field-initializers>)
endif()

file(GLOB EXE_SRCS "*.cpp" "*.hpp" "Entity/*.cpp" "Entity/*.hpp")
target_sources(scop PRIVATE ${EXE_SRCS})
if (WIN32)
    target_sources(scop PRIVATE scop.def)
endif()

target_include_directories(scop PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

target_compile_definitions(scop PRIVATE "SHADER_DIR=\"${SHADER_DIR}\"")
if(CMAKE_GENERATOR STREQUAL "Xcode")
    target_compile_definitions(scop PRIVATE "__XCODE__")
endif()
target_compile_definitions(scop PRIVATE "RESOURCE_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/resources\"")

target_link_libraries(scop PRIVATE Graphics glm::glm imgui stb_image assimp::assimp)
add_dependencies(scop flat_color_shader textured_shader)
