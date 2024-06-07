# ---------------------------------------------------
# CMakeLists.txt
#
# Author: Thomas Choquet <thomas.publique@icloud.com>
# Date: 2024/06/07 16:25:29
# ---------------------------------------------------

if(GFX_ENABLE_METAL)
    set(MTL_SHADER_LIB "${CMAKE_CURRENT_BINARY_DIR}/MTLShaderLib.metallib")
endif()
if(GFX_ENABLE_OPENGL)
    set(OPENGL_SHADER_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
endif()

add_executable(fragmentUniform)

set_target_properties(fragmentUniform PROPERTIES 
    CXX_STANDARD          11
    CXX_STANDARD_REQUIRED ON
)

if(GFX_ENABLE_METAL)
    set_target_properties(fragmentUniform PROPERTIES FOLDER "exemples/fragmentUniform")
else()
    set_target_properties(fragmentUniform PROPERTIES FOLDER "exemples")
endif()

file(GLOB EXE_SRCS "*.cpp" "*.hpp" "*.h")
target_sources(fragmentUniform PRIVATE ${EXE_SRCS})
if(GFX_ENABLE_METAL)
    file(GLOB EXE_SRCS "*.metal")
    target_sources(fragmentUniform PRIVATE ${EXE_SRCS})
endif()
if(GFX_ENABLE_OPENGL)
    file(GLOB EXE_SRCS "*.vs" "*.fs")
    target_sources(fragmentUniform PRIVATE ${EXE_SRCS})
endif()
target_include_directories(fragmentUniform PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

if(GFX_ENABLE_METAL)
    target_compile_definitions(fragmentUniform PRIVATE "MTL_SHADER_LIB=\"${MTL_SHADER_LIB}\"")
endif()
if(GFX_ENABLE_OPENGL)
    target_compile_definitions(fragmentUniform PRIVATE "OPENGL_SHADER_DIR=\"${OPENGL_SHADER_DIR}\"")
endif()

target_link_libraries(fragmentUniform PRIVATE Graphics)

if(GFX_ENABLE_METAL)
    add_custom_target(MTLShaderLib_fragmentUniform DEPENDS ${MTL_SHADER_LIB} SOURCES "${CMAKE_CURRENT_BINARY_DIR}/fragmentUniform.air")

    set_target_properties(MTLShaderLib_fragmentUniform PROPERTIES
        FOLDER "exemples/fragmentUniform"
    )

    target_sources(MTLShaderLib_fragmentUniform PRIVATE "fragmentUniform.metal")

    add_dependencies(fragmentUniform MTLShaderLib_fragmentUniform)
    

    get_target_property(EXE_INCLUDE_DIRS fragmentUniform INCLUDE_DIRECTORIES)
    foreach(dir IN LISTS EXE_INCLUDE_DIRS)
        list(APPEND MTL_SHADER_BUILD_FLAGS "-I${dir}")
    endforeach()
    
    find_program(XCRUN_PATH xcrun REQUIRED)
    
    add_custom_command(
        OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/fragmentUniform.air"
        COMMAND ${XCRUN_PATH} -sdk macosx metal ${MTL_SHADER_BUILD_FLAGS} -o "${CMAKE_CURRENT_BINARY_DIR}/fragmentUniform.air" -c "${CMAKE_CURRENT_SOURCE_DIR}/fragmentUniform.metal"
        DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/fragmentUniform.metal"
    )
        
    add_custom_command(
        OUTPUT ${MTL_SHADER_LIB}
        COMMAND ${XCRUN_PATH} -sdk macosx metallib -o ${MTL_SHADER_LIB} "${CMAKE_CURRENT_BINARY_DIR}/fragmentUniform.air"
        DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/fragmentUniform.air"
    )
endif()