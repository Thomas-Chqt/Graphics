# Example:
#   add_shader(myShader SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/shader.slang" TARGETS spirv metal)
# OR
#   add_shader(myShader)
#   set_target_properties(myShader PROPERTIES SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/shader.slang" TARGETS "spirv;metal")

function(add_shader_lib SLIB_NAME)
    cmake_parse_arguments(SLIB "" "" "SOURCES;TARGETS" ${ARGN})

    if(NOT SLIB_SOURCES)
        message(FATAL_ERROR "add_shader_lib: SOURCES is required")
    endif()

    if(NOT SLIB_TARGETS)
        message(FATAL_ERROR "add_shader_lib: TARGETS is required")
    endif()

    find_package(Python3 REQUIRED COMPONENTS Interpreter)

    set(SLIB_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${SLIB_NAME}.slib")

    define_property(TARGET PROPERTY SLIB_OUTPUT
      BRIEF_DOCS "Shader output file for a shader library target."
      FULL_DOCS "Path to the compiled shader output (.slib file) produced by this shader library target."
    )

    add_custom_command(
        OUTPUT ${SLIB_OUTPUT}
        COMMAND ${Python3_EXECUTABLE} ${GFXSC_PATH}
            -t ${SLIB_TARGETS}
            -o ${SLIB_OUTPUT}
            ${SLIB_SOURCES}
        DEPENDS ${SLIB_SOURCES} ${GFXSC_PATH}
        COMMENT "Compiling shader ${SLIB_NAME}"
        VERBATIM
    )

    add_custom_target(${SLIB_NAME} ALL
        DEPENDS ${SLIB_OUTPUT}
    )

    set_target_properties(${SLIB_NAME} PROPERTIES
        SLIB_OUTPUT "${SLIB_OUTPUT}"
    )
endfunction()
