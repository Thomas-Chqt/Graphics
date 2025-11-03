# ---------------------------------------------------
# CMakeLists.txt
#
# Author: Thomas Choquet <semoir.dense-0h@icloud.com>
# Date: 2025/09/23 08:59:09
# ---------------------------------------------------

set(SHADER_TARGET_LIST)
if (GFX_BUILD_METAL)
    list(APPEND SHADER_TARGET_LIST metal)
endif()
if (GFX_BUILD_VULKAN)
    list(APPEND SHADER_TARGET_LIST spirv)
endif()
list(JOIN SHADER_TARGET_LIST "," SHADER_TARGETS)

set(FLAT_COLOR_SSRC "${CMAKE_CURRENT_SOURCE_DIR}/common_vertex.slang" "${CMAKE_CURRENT_SOURCE_DIR}/flat_color.slang")
set(FLAT_COLOR_SLIB ${CMAKE_CURRENT_BINARY_DIR}/flat_color.slib)
add_custom_command(
    OUTPUT ${FLAT_COLOR_SLIB}
    COMMAND $<TARGET_FILE:gfxsc> -t ${SHADER_TARGETS} -o ${FLAT_COLOR_SLIB} ${FLAT_COLOR_SSRC}
    DEPENDS gfxsc ${FLAT_COLOR_SSRC}
    COMMENT "Building flat_color_shader shaders"
    VERBATIM
)
add_custom_target(flat_color_shader ALL DEPENDS ${FLAT_COLOR_SLIB})
set_target_properties(flat_color_shader PROPERTIES FOLDER "examples/scop/shader")

set(TEXTURED_SSRC "${CMAKE_CURRENT_SOURCE_DIR}/common_vertex.slang" "${CMAKE_CURRENT_SOURCE_DIR}/textured.slang")
set(TEXTURED_SLIB ${CMAKE_CURRENT_BINARY_DIR}/textured.slib)
add_custom_command(
    OUTPUT ${TEXTURED_SLIB}
    COMMAND $<TARGET_FILE:gfxsc> -t ${SHADER_TARGETS} -o ${TEXTURED_SLIB} ${TEXTURED_SSRC}
    DEPENDS gfxsc ${TEXTURED_SSRC}
    COMMENT "Building textured_shader shaders"
    VERBATIM
)
add_custom_target(textured_shader ALL DEPENDS ${TEXTURED_SLIB})
set_target_properties(textured_shader PROPERTIES FOLDER "examples/scop/shader")

set(SHADER_DIR ${CMAKE_CURRENT_BINARY_DIR} PARENT_SCOPE)
