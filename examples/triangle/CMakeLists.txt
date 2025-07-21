# ---------------------------------------------------
# CMakeLists.txt
#
# Author: Thomas Choquet <thomas.publique@icloud.com>
# Date: 2024/06/06 16:59:39
# ---------------------------------------------------

if (GFX_BUILD_METAL)
    string(APPEND SHADER_TARGETS "metal")
endif()
if (GFX_BUILD_VULKAN)
    string(APPEND SHADER_TARGETS ",spirv")
endif()

set(SHADER_SLIB "${CMAKE_CURRENT_BINARY_DIR}/shader.slib")
file(GLOB SHADER_SRCS "*.slang")

add_custom_command(
    OUTPUT ${SHADER_SLIB}
    COMMAND $<TARGET_FILE:gfxsc> -t ${SHADER_TARGETS} -o ${SHADER_SLIB} ${SHADER_SRCS}
    DEPENDS gfxsc ${SHADER_SRCS}
    COMMENT "Building triangle shader"
    VERBATIM
)
add_custom_target(triangle_shader ALL DEPENDS ${SHADER_SLIB})
set_target_properties(triangle_shader PROPERTIES FOLDER "examples/triangle/shaders")

add_executable(triangle)

if (APPLE)
    enable_language(OBJC)
endif()

set_target_properties(triangle PROPERTIES
    FOLDER         "examples/triangle"
    ENABLE_EXPORTS ON
)

target_compile_features(triangle PRIVATE cxx_std_23)

if(MSVC)
    target_compile_options(triangle PRIVATE /W4)
else()
    target_compile_options(triangle PRIVATE -Wall -Wextra -Wpedantic)
endif()

file(GLOB EXE_SRCS "*.cpp" "*.hpp")
target_sources(triangle PRIVATE ${EXE_SRCS})
if (WIN32)
    target_sources(triangle PRIVATE triangle.def)
endif()

target_include_directories(triangle PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

target_compile_definitions(triangle PRIVATE "SHADER_SLIB=\"${SHADER_SLIB}\"")
if(CMAKE_GENERATOR STREQUAL "Xcode")
    target_compile_definitions(triangle PRIVATE "__XCODE__")
endif()

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
FetchContent_MakeAvailable(glfw3)

if (TARGET update_mappings)
    set_target_properties(glfw PROPERTIES FOLDER "dependencies/GLFW3")
    set_target_properties(update_mappings PROPERTIES FOLDER "dependencies/GLFW3")
else()
    set_target_properties(glfw PROPERTIES FOLDER "dependencies")
endif()

target_link_libraries(triangle PRIVATE Graphics glfw)
add_dependencies(triangle triangle_shader)
