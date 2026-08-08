set(SHADER_TARGET_LIST)
if (GFX_BUILD_METAL)
    list(APPEND SHADER_TARGET_LIST metal)
endif()
if (GFX_BUILD_VULKAN)
    list(APPEND SHADER_TARGET_LIST spirv)
endif()
list(JOIN SHADER_TARGET_LIST "," SHADER_TARGETS)

set(PBR_PRECOMPUTE_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/pbr_precompute.slang")
set(PBR_PRECOMPUTE_SLIBS)
foreach(ENTRY_POINT equirectangularToCubemap convolveDiffuseIrradiance prefilterEnvironment integrateBrdf)
    set(ENTRY_POINT_SLIB "${CMAKE_CURRENT_BINARY_DIR}/${ENTRY_POINT}.slib")
    add_custom_command(
        OUTPUT ${ENTRY_POINT_SLIB}
        COMMAND $<TARGET_FILE:gfxsc> -t ${SHADER_TARGETS} -e ${ENTRY_POINT} -o ${ENTRY_POINT_SLIB} "${PBR_PRECOMPUTE_SRCS}"
        DEPENDS gfxsc "${PBR_PRECOMPUTE_SRCS}" "${CMAKE_CURRENT_SOURCE_DIR}/utils.slang" "${CMAKE_CURRENT_SOURCE_DIR}/brdf.slang"
        COMMENT "Building ${ENTRY_POINT} shader lib"
        VERBATIM
    )
    list(APPEND PBR_PRECOMPUTE_SLIBS ${ENTRY_POINT_SLIB})
endforeach()

set(PBR_SLIB "${CMAKE_CURRENT_BINARY_DIR}/pbr.slib")
set(PBR_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/pbr.slang")
add_custom_command(
    OUTPUT ${PBR_SLIB}
    COMMAND $<TARGET_FILE:gfxsc> -t ${SHADER_TARGETS} -o ${PBR_SLIB} "${PBR_SRCS}"
    DEPENDS gfxsc "${PBR_SRCS}" "${CMAKE_CURRENT_SOURCE_DIR}/utils.slang" "${CMAKE_CURRENT_SOURCE_DIR}/brdf.slang" "${CMAKE_CURRENT_SOURCE_DIR}/ibl.slang"
    COMMENT "Building pbr shader lib"
    VERBATIM
)

set(SKYBOX_SLIB "${CMAKE_CURRENT_BINARY_DIR}/skybox.slib")
set(SKYBOX_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/skybox.slang")
add_custom_command(
    OUTPUT ${SKYBOX_SLIB}
    COMMAND $<TARGET_FILE:gfxsc> -t ${SHADER_TARGETS} -o ${SKYBOX_SLIB} "${SKYBOX_SRCS}"
    DEPENDS gfxsc "${SKYBOX_SRCS}" "${CMAKE_CURRENT_SOURCE_DIR}/utils.slang"
    COMMENT "Building skybox shader lib"
    VERBATIM
)

add_custom_target(pbr_shaders ALL DEPENDS ${PBR_PRECOMPUTE_SLIBS} ${PBR_SLIB} ${SKYBOX_SLIB})
set_target_properties(pbr_shaders PROPERTIES FOLDER "examples/pbr")

set(SHADER_DIR ${CMAKE_CURRENT_BINARY_DIR} PARENT_SCOPE)
