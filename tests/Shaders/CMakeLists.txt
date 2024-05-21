# ---------------------------------------------------
# CMakeLists.txt
#
# Author: Thomas Choquet <thomas.publique@icloud.com>
# Date: 2024/05/20 18:00:16
# ---------------------------------------------------

get_target_property(GRAPHICS_TEST_INCLUDE_DIRS Graphics_test INCLUDE_DIRECTORIES)

set(MTL_SHADER_LIB ${CMAKE_CURRENT_BINARY_DIR}/MetalShaderLibrary.metallib)

target_compile_definitions(Graphics_test PRIVATE "MTL_SHADER_LIB=\"${MTL_SHADER_LIB}\"")

foreach(dir IN LISTS GRAPHICS_TEST_INCLUDE_DIRS)
    list(APPEND MTL_SHADER_BUILD_FLAGS "-I${dir}")
endforeach()

file(GLOB_RECURSE MTL_SHADER_LIB_SRCS "Metal/*.metal")

find_program(XCRUN_PATH xcrun REQUIRED)

foreach(src IN LISTS MTL_SHADER_LIB_SRCS)
    string(REPLACE ".metal" ".air" air "${src}")
    string(REPLACE ${CMAKE_CURRENT_SOURCE_DIR}/Metal ${CMAKE_CURRENT_BINARY_DIR} air "${air}")

    add_custom_command(OUTPUT "${air}" COMMAND ${XCRUN_PATH} -sdk macosx metal ${MTL_SHADER_BUILD_FLAGS} -o "${air}" -c "${src}" DEPENDS "${src}")

    list(APPEND MTL_SHADER_LIB_AIRS "${air}")
endforeach()

add_custom_command(OUTPUT ${MTL_SHADER_LIB} COMMAND ${XCRUN_PATH} -sdk macosx metallib -o ${MTL_SHADER_LIB} ${MTL_SHADER_LIB_AIRS} DEPENDS ${MTL_SHADER_LIB_AIRS})

add_custom_target(MetalShaderLibrary DEPENDS ${MTL_SHADER_LIB} SOURCES ${MTL_SHADER_LIB_SRCS})

add_dependencies(Graphics_test MetalShaderLibrary)