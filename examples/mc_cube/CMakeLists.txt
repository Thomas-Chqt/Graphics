# ---------------------------------------------------
# CMakeLists.txt
#
# Author: Thomas Choquet <semoir.dense-0h@icloud.com>
# Date: 2025/07/25 09:12:04
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
target_compile_definitions(imgui PRIVATE "GLFW_INCLUDE_NONE")
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

##################################################################

set(SHADER_TARGET_LIST)
if (GFX_BUILD_METAL)
    list(APPEND SHADER_TARGET_LIST metal)
endif()
if (GFX_BUILD_VULKAN)
    list(APPEND SHADER_TARGET_LIST spirv)
endif()
list(JOIN SHADER_TARGET_LIST "," SHADER_TARGETS)

set(SHADER_SLIB "${CMAKE_CURRENT_BINARY_DIR}/mc_cube.slib")
file(GLOB SHADER_SRCS "*.slang")

add_custom_command(
    OUTPUT ${SHADER_SLIB}
    COMMAND $<TARGET_FILE:gfxsc> -t ${SHADER_TARGETS} -o ${SHADER_SLIB} ${SHADER_SRCS}
    DEPENDS gfxsc ${SHADER_SRCS}
    COMMENT "Building mc_cube shader"
    VERBATIM
)
add_custom_target(mc_cube_shader ALL DEPENDS ${SHADER_SLIB})
set_target_properties(mc_cube_shader PROPERTIES FOLDER "examples/mc_cube")

add_executable(mc_cube)

if (APPLE)
    enable_language(OBJC)
endif()

set_target_properties(mc_cube PROPERTIES
    FOLDER         "examples/mc_cube"
    ENABLE_EXPORTS ON
)

target_compile_features(mc_cube PRIVATE cxx_std_23)

if(MSVC)
    target_compile_options(mc_cube PRIVATE /W4)
else()
    target_compile_options(mc_cube PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(mc_cube PRIVATE $<$<CXX_COMPILER_ID:GNU>:-Wno-missing-field-initializers>)
endif()

file(GLOB EXE_SRCS "*.cpp" "*.hpp")
target_sources(mc_cube PRIVATE ${EXE_SRCS})
if (WIN32)
    target_sources(mc_cube PRIVATE mc_cube.def)
endif()

target_include_directories(mc_cube PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

target_compile_definitions(mc_cube PRIVATE "SHADER_SLIB=\"${SHADER_SLIB}\"")
if(CMAKE_GENERATOR STREQUAL "Xcode")
    target_compile_definitions(mc_cube PRIVATE "__XCODE__")
endif()
target_compile_definitions(mc_cube PRIVATE "RESOURCE_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/resources\"")
target_compile_definitions(mc_cube PRIVATE "GLFW_INCLUDE_NONE")

target_link_libraries(mc_cube PRIVATE Graphics glm::glm imgui stb_image)
add_dependencies(mc_cube mc_cube_shader)
