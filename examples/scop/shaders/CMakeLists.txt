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

set(FLAT_COLOR_SSRC "${CMAKE_CURRENT_SOURCE_DIR}/flat_color.slang")
set(FLAT_COLOR_SLIB ${CMAKE_CURRENT_BINARY_DIR}/flat_color.slib)
add_custom_command(
    OUTPUT ${FLAT_COLOR_SLIB}
    COMMAND $<TARGET_FILE:gfxsc> -t ${SHADER_TARGETS} -o ${FLAT_COLOR_SLIB} ${FLAT_COLOR_SSRC}
    DEPENDS gfxsc ${FLAT_COLOR_SSRC}
    COMMENT "Building flat_color shaders"
    VERBATIM
)
add_custom_target(flat_color_shader ALL DEPENDS ${FLAT_COLOR_SLIB})
set_target_properties(flat_color_shader PROPERTIES FOLDER "examples/scop/shader")

set(TEXTURED_SSRC "${CMAKE_CURRENT_SOURCE_DIR}/textured.slang")
set(TEXTURED_SLIB ${CMAKE_CURRENT_BINARY_DIR}/textured.slib)
add_custom_command(
    OUTPUT ${TEXTURED_SLIB}
    COMMAND $<TARGET_FILE:gfxsc> -t ${SHADER_TARGETS} -o ${TEXTURED_SLIB} ${TEXTURED_SSRC}
    DEPENDS gfxsc ${TEXTURED_SSRC}
    COMMENT "Building textured shaders"
    VERBATIM
)
add_custom_target(textured_shader ALL DEPENDS ${TEXTURED_SLIB})
set_target_properties(textured_shader PROPERTIES FOLDER "examples/scop/shader")

set(scop_SSRC "${CMAKE_CURRENT_SOURCE_DIR}/scop.slang")
set(scop_SLIB ${CMAKE_CURRENT_BINARY_DIR}/scop.slib)
add_custom_command(
    OUTPUT ${scop_SLIB}
    COMMAND $<TARGET_FILE:gfxsc> -t ${SHADER_TARGETS} -o ${scop_SLIB} ${scop_SSRC}
    DEPENDS gfxsc ${scop_SSRC}
    COMMENT "Building scop shaders"
    VERBATIM
)
add_custom_target(scop_shader ALL DEPENDS ${scop_SLIB})
set_target_properties(scop_shader PROPERTIES FOLDER "examples/scop/shader")

set(PBR_MATERIAL_SSRC "${CMAKE_CURRENT_SOURCE_DIR}/pbr_material.slang")
set(PBR_MATERIAL_SLIB "${CMAKE_CURRENT_BINARY_DIR}/pbr_material.slib")
add_custom_command(
    OUTPUT ${PBR_MATERIAL_SLIB}
    COMMAND $<TARGET_FILE:gfxsc> -t ${SHADER_TARGETS} -o ${PBR_MATERIAL_SLIB} ${PBR_MATERIAL_SSRC}
    DEPENDS
        gfxsc
        ${PBR_MATERIAL_SSRC}
        "${CMAKE_CURRENT_SOURCE_DIR}/pbr.slang"
        "${CMAKE_CURRENT_SOURCE_DIR}/SceneData.slang"
        "${CMAKE_CURRENT_SOURCE_DIR}/Light.slang"
        "${CMAKE_CURRENT_SOURCE_DIR}/brdf.slang"
        "${CMAKE_CURRENT_SOURCE_DIR}/ibl.slang"
        "${CMAKE_CURRENT_SOURCE_DIR}/utils.slang"
        "${CMAKE_CURRENT_SOURCE_DIR}/shader.h"
    COMMENT "Building PBR material shaders"
    VERBATIM
)
add_custom_target(pbr_material_shader ALL DEPENDS ${PBR_MATERIAL_SLIB})
set_target_properties(pbr_material_shader PROPERTIES FOLDER "examples/scop/shader")

set(PBR_PRECOMPUTE_SSRC "${CMAKE_CURRENT_SOURCE_DIR}/pbr_precompute.slang")
set(PBR_PRECOMPUTE_SLIBS)
foreach(ENTRY_POINT equirectangularToCubemap convolveDiffuseIrradiance prefilterEnvironment integrateBrdf)
    set(ENTRY_POINT_SLIB "${CMAKE_CURRENT_BINARY_DIR}/${ENTRY_POINT}.slib")
    add_custom_command(
        OUTPUT ${ENTRY_POINT_SLIB}
        COMMAND $<TARGET_FILE:gfxsc> -t ${SHADER_TARGETS} -e ${ENTRY_POINT} -o ${ENTRY_POINT_SLIB} ${PBR_PRECOMPUTE_SSRC}
        DEPENDS
            gfxsc
            ${PBR_PRECOMPUTE_SSRC}
            "${CMAKE_CURRENT_SOURCE_DIR}/brdf.slang"
            "${CMAKE_CURRENT_SOURCE_DIR}/utils.slang"
            "${CMAKE_CURRENT_SOURCE_DIR}/shader.h"
        COMMENT "Building ${ENTRY_POINT} shader lib for scop"
        VERBATIM
    )
    list(APPEND PBR_PRECOMPUTE_SLIBS ${ENTRY_POINT_SLIB})
endforeach()
add_custom_target(scop_pbr_precompute_shaders ALL DEPENDS ${PBR_PRECOMPUTE_SLIBS})
set_target_properties(scop_pbr_precompute_shaders PROPERTIES FOLDER "examples/scop/shader")

set(SKYBOX_SSRC "${CMAKE_CURRENT_SOURCE_DIR}/skybox.slang")
set(SKYBOX_SLIB "${CMAKE_CURRENT_BINARY_DIR}/skybox.slib")
add_custom_command(
    OUTPUT ${SKYBOX_SLIB}
    COMMAND $<TARGET_FILE:gfxsc> -t ${SHADER_TARGETS} -o ${SKYBOX_SLIB} ${SKYBOX_SSRC}
    DEPENDS
        gfxsc
        ${SKYBOX_SSRC}
        "${CMAKE_CURRENT_SOURCE_DIR}/utils.slang"
        "${CMAKE_CURRENT_SOURCE_DIR}/shader.h"
    COMMENT "Building skybox shaders"
    VERBATIM
)
add_custom_target(skybox_shader ALL DEPENDS ${SKYBOX_SLIB})
set_target_properties(skybox_shader PROPERTIES FOLDER "examples/scop/shader")

set(SHADER_DIR ${CMAKE_CURRENT_BINARY_DIR} PARENT_SCOPE)
