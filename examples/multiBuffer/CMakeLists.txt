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

set(SHADER1_SLIB "${CMAKE_CURRENT_BINARY_DIR}/shader1.slib")
set(SHADER1_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/shader1.slang")
add_custom_command(
    OUTPUT ${SHADER1_SLIB}
    COMMAND $<TARGET_FILE:gfxsc> -t ${SHADER_TARGETS} -o ${SHADER1_SLIB} "${SHADER1_SRCS}"
    DEPENDS gfxsc "${SHADER1_SRCS}"
    COMMENT "Building multiBuffer shader 1"
    VERBATIM
)
add_custom_target(multiBuffer_shader1 ALL DEPENDS ${SHADER1_SLIB})

set(SHADER2_SLIB "${CMAKE_CURRENT_BINARY_DIR}/shader2.slib")
set(SHADER2_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/shader2.slang")
add_custom_command(
    OUTPUT ${SHADER2_SLIB}
    COMMAND $<TARGET_FILE:gfxsc> -t ${SHADER_TARGETS} -o ${SHADER2_SLIB} "${SHADER2_SRCS}"
    DEPENDS gfxsc "${SHADER2_SRCS}"
    COMMENT "Building multiBuffer shader 2"
    VERBATIM
)
add_custom_target(multiBuffer_shader2 ALL DEPENDS ${SHADER2_SLIB})

set(SHADER3_SLIB "${CMAKE_CURRENT_BINARY_DIR}/shader3.slib")
set(SHADER3_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/shader3.slang")
add_custom_command(
    OUTPUT ${SHADER3_SLIB}
    COMMAND $<TARGET_FILE:gfxsc> -t ${SHADER_TARGETS} -o ${SHADER3_SLIB} "${SHADER3_SRCS}"
    DEPENDS gfxsc "${SHADER3_SRCS}"
    COMMENT "Building multiBuffer shader 3"
    VERBATIM
)
add_custom_target(multiBuffer_shader3 ALL DEPENDS ${SHADER3_SLIB})

set(SHADER4_SLIB "${CMAKE_CURRENT_BINARY_DIR}/shader4.slib")
set(SHADER4_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/shader4.slang")
add_custom_command(
    OUTPUT ${SHADER4_SLIB}
    COMMAND $<TARGET_FILE:gfxsc> -t ${SHADER_TARGETS} -o ${SHADER4_SLIB} "${SHADER4_SRCS}"
    DEPENDS gfxsc "${SHADER4_SRCS}"
    COMMENT "Building multiBuffer shader 4"
    VERBATIM
)
add_custom_target(multiBuffer_shader4 ALL DEPENDS ${SHADER4_SLIB})


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

target_compile_definitions(multiBuffer PRIVATE "SHADER1_SLIB=\"${SHADER1_SLIB}\"")
target_compile_definitions(multiBuffer PRIVATE "SHADER2_SLIB=\"${SHADER2_SLIB}\"")
target_compile_definitions(multiBuffer PRIVATE "SHADER3_SLIB=\"${SHADER3_SLIB}\"")
target_compile_definitions(multiBuffer PRIVATE "SHADER4_SLIB=\"${SHADER4_SLIB}\"")
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
add_dependencies(multiBuffer multiBuffer_shader1 multiBuffer_shader2 multiBuffer_shader3 multiBuffer_shader4)
