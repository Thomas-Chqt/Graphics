# ---------------------------------------------------
# CMakeLists.txt
#
# Author: Thomas Choquet <thomas.publique@icloud.com>
# Date: 2024/06/06 16:59:39
# ---------------------------------------------------

if(GFX_ENABLE_METAL)
    list(APPEND MTL_SHADER_BUILD_FLAGS "-I${CMAKE_CURRENT_SOURCE_DIR}")
    
    set(MTL_SHADER_LIB "${CMAKE_CURRENT_BINARY_DIR}/MTLShaderLib.metallib")
    
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
            
            add_custom_target(MTLShaderLib_triangle DEPENDS ${MTL_SHADER_LIB} SOURCES "${CMAKE_CURRENT_BINARY_DIR}/triangle.air")
        endif()
        
        if (GFX_ENABLE_OPENGL)
    set(OPENGL_SHADER_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
endif()

add_executable(triangle)
target_sources(triangle PRIVATE "main.cpp")
target_include_directories(triangle PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_definitions(triangle PRIVATE "MTL_SHADER_LIB=\"${MTL_SHADER_LIB}\"")
target_compile_definitions(triangle PRIVATE "OPENGL_SHADER_DIR=\"${OPENGL_SHADER_DIR}\"")
add_dependencies(triangle MTLShaderLib_triangle)
target_link_libraries(triangle PRIVATE Graphics)