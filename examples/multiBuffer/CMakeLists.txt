# ---------------------------------------------------
# CMakeLists.txt
#
# Author: Thomas Choquet <semoir.dense-0h@icloud.com>
# Date: 2025/07/12 14:01:42
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
add_custom_target(multiBuffer_shader ALL DEPENDS ${SHADER_SLIB})

add_executable(multiBuffer)

if (APPLE)
    enable_language(OBJC)
endif()

set_target_properties(multiBuffer PROPERTIES
    FOLDER         "examples/multiBuffer"
    ENABLE_EXPORTS ON
)

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

target_link_libraries(multiBuffer PRIVATE Graphics glfw)
add_dependencies(multiBuffer multiBuffer_shader)
