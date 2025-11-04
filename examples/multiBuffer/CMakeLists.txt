# ---------------------------------------------------
# CMakeLists.txt
#
# Author: Thomas Choquet <semoir.dense-0h@icloud.com>
# Date: 2025/07/12 14:01:42
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

##################################################################

set(SHADER_TARGET_LIST)
if (GFX_BUILD_METAL)
    list(APPEND SHADER_TARGET_LIST metal)
endif()
if (GFX_BUILD_VULKAN)
    list(APPEND SHADER_TARGET_LIST spirv)
endif()
list(JOIN SHADER_TARGET_LIST "," SHADER_TARGETS)

set(SHADER_SLIB "${CMAKE_CURRENT_BINARY_DIR}/shader.slib")
file(GLOB SHADER_SRCS "*.slang")

add_custom_command(
    OUTPUT ${SHADER_SLIB}
    COMMAND $<TARGET_FILE:gfxsc> -t ${SHADER_TARGETS} -o ${SHADER_SLIB} ${SHADER_SRCS}
    DEPENDS gfxsc ${SHADER_SRCS}
    COMMENT "Building triangle shader"
    VERBATIM
)
add_custom_target(multiBuffer_shader ALL DEPENDS ${SHADER_SLIB})
set_target_properties(multiBuffer_shader PROPERTIES FOLDER "examples/multiBuffer")

add_executable(multiBuffer)

if (APPLE)
    enable_language(OBJC)
endif()

set_target_properties(multiBuffer PROPERTIES
    FOLDER         "examples/multiBuffer"
    ENABLE_EXPORTS ON
)

target_compile_features(multiBuffer PRIVATE cxx_std_23)

if(MSVC)
    target_compile_options(multiBuffer PRIVATE /W4)
else()
    target_compile_options(multiBuffer PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(multiBuffer PRIVATE $<$<CXX_COMPILER_ID:GNU>:-Wno-missing-field-initializers>)
endif()

file(GLOB EXE_SRCS "*.cpp" "*.hpp")
target_sources(multiBuffer PRIVATE ${EXE_SRCS})
if (WIN32)
    target_sources(multiBuffer PRIVATE multiBuffer.def)
endif()

target_include_directories(multiBuffer PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

target_compile_definitions(multiBuffer PRIVATE "SHADER_SLIB=\"${SHADER_SLIB}\"")
if(CMAKE_GENERATOR STREQUAL "Xcode")
    target_compile_definitions(multiBuffer PRIVATE "__XCODE__")
endif()

target_link_libraries(multiBuffer PRIVATE Graphics glfw glm::glm)
add_dependencies(multiBuffer multiBuffer_shader)
