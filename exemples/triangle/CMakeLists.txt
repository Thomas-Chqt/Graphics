# ---------------------------------------------------
# CMakeLists.txt
#
# Author: Thomas Choquet <thomas.publique@icloud.com>
# Date: 2024/06/06 16:59:39
# ---------------------------------------------------

if(GFX_BUILD_METAL)
    set(MTL_SHADER_LIB "${CMAKE_CURRENT_BINARY_DIR}/MTLShaderLib.metallib")
endif()
if(GFX_BUILD_OPENGL)
    set(OPENGL_SHADER_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
endif()

add_executable(triangle)

set_target_properties(triangle PROPERTIES 
    CXX_STANDARD          11
    CXX_STANDARD_REQUIRED ON
)

if(GFX_BUILD_METAL)
    set_target_properties(triangle PROPERTIES FOLDER "exemples/triangle")
else()
    set_target_properties(triangle PROPERTIES FOLDER "exemples")
endif()

file(GLOB EXE_SRCS "*.cpp" "*.hpp" "*.h")
target_sources(triangle PRIVATE ${EXE_SRCS})
if(GFX_BUILD_METAL)
    file(GLOB EXE_SRCS "*.metal")
    target_sources(triangle PRIVATE ${EXE_SRCS})
endif()
if(GFX_BUILD_OPENGL)
    file(GLOB EXE_SRCS "*.vs" "*.fs")
    target_sources(triangle PRIVATE ${EXE_SRCS})
endif()
target_include_directories(triangle PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

if(GFX_BUILD_METAL)
    target_compile_definitions(triangle PRIVATE "MTL_SHADER_LIB=\"${MTL_SHADER_LIB}\"")
endif()
if(GFX_BUILD_OPENGL)
    target_compile_definitions(triangle PRIVATE "OPENGL_SHADER_DIR=\"${OPENGL_SHADER_DIR}\"")
endif()

target_link_libraries(triangle PRIVATE Graphics)

if(GFX_BUILD_METAL)
    add_custom_target(MTLShaderLib_triangle DEPENDS ${MTL_SHADER_LIB} SOURCES "${CMAKE_CURRENT_BINARY_DIR}/triangle.air")

    set_target_properties(MTLShaderLib_triangle PROPERTIES
        FOLDER "exemples/triangle"
    )

    target_sources(MTLShaderLib_triangle PRIVATE "triangle.metal")

    add_dependencies(triangle MTLShaderLib_triangle)
    

    get_target_property(TRIANGLE_INCLUDE_DIRS triangle INCLUDE_DIRECTORIES)
    foreach(dir IN LISTS TRIANGLE_INCLUDE_DIRS)
        list(APPEND MTL_SHADER_BUILD_FLAGS "-I${dir}")
    endforeach()
    
    find_program(XCRUN_PATH xcrun REQUIRED)
    
    add_custom_command(
        OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/triangle.air"
        COMMAND ${XCRUN_PATH} -sdk macosx metal ${MTL_SHADER_BUILD_FLAGS} -o "${CMAKE_CURRENT_BINARY_DIR}/triangle.air" -c "${CMAKE_CURRENT_SOURCE_DIR}/triangle.metal"
        DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/triangle.metal"
    )
        
    add_custom_command(
        OUTPUT ${MTL_SHADER_LIB}
        COMMAND ${XCRUN_PATH} -sdk macosx metallib -o ${MTL_SHADER_LIB} "${CMAKE_CURRENT_BINARY_DIR}/triangle.air"
        DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/triangle.air"
    )
endif()


