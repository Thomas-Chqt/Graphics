# ---------------------------------------------------
# CMakeLists.txt
#
# Author: Thomas Choquet <thomas.publique@icloud.com>
# Date: 2024/06/07 16:51:27
# ---------------------------------------------------

if(GFX_ENABLE_METAL)
    set(MTL_SHADER_LIB "${CMAKE_CURRENT_BINARY_DIR}/MTLShaderLib.metallib")
endif()
if(GFX_ENABLE_OPENGL)
    set(OPENGL_SHADER_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
endif()
set(RESSOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(texturedSquare)

set_target_properties(texturedSquare PROPERTIES 
    CXX_STANDARD          11
    CXX_STANDARD_REQUIRED ON
)

if(GFX_ENABLE_METAL)
    set_target_properties(texturedSquare PROPERTIES FOLDER "exemples/texturedSquare")
else()
    set_target_properties(texturedSquare PROPERTIES FOLDER "exemples")
endif()

file(GLOB EXE_SRCS "*.cpp" "*.hpp" "*.h")
target_sources(texturedSquare PRIVATE ${EXE_SRCS})
if(GFX_ENABLE_METAL)
    file(GLOB EXE_SRCS "*.metal")
    target_sources(texturedSquare PRIVATE ${EXE_SRCS})
endif()
if(GFX_ENABLE_OPENGL)
    file(GLOB EXE_SRCS "*.vs" "*.fs")
    target_sources(texturedSquare PRIVATE ${EXE_SRCS})
endif()
target_include_directories(texturedSquare PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

if(GFX_ENABLE_METAL)
    target_compile_definitions(texturedSquare PRIVATE "MTL_SHADER_LIB=\"${MTL_SHADER_LIB}\"")
endif()
if(GFX_ENABLE_OPENGL)
    target_compile_definitions(texturedSquare PRIVATE "OPENGL_SHADER_DIR=\"${OPENGL_SHADER_DIR}\"")
endif()
target_compile_definitions(texturedSquare PRIVATE "RESSOURCES_DIR=\"${RESSOURCES_DIR}\"")

target_link_libraries(texturedSquare PRIVATE Graphics)

if(GFX_ENABLE_METAL)
    add_custom_target(MTLShaderLib_texturedSquare DEPENDS ${MTL_SHADER_LIB} SOURCES "${CMAKE_CURRENT_BINARY_DIR}/texturedSquare.air")

    set_target_properties(MTLShaderLib_texturedSquare PROPERTIES
        FOLDER "exemples/texturedSquare"
    )

    target_sources(MTLShaderLib_texturedSquare PRIVATE "texturedSquare.metal")

    add_dependencies(texturedSquare MTLShaderLib_texturedSquare)
    

    get_target_property(EXE_INCLUDE_DIRS texturedSquare INCLUDE_DIRECTORIES)
    foreach(dir IN LISTS EXE_INCLUDE_DIRS)
        list(APPEND MTL_SHADER_BUILD_FLAGS "-I${dir}")
    endforeach()
    list(APPEND MTL_SHADER_BUILD_FLAGS "-I${CMAKE_SOURCE_DIR}/dependencies/Math/include")
    
    find_program(XCRUN_PATH xcrun REQUIRED)
    
    add_custom_command(
        OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/texturedSquare.air"
        COMMAND ${XCRUN_PATH} -sdk macosx metal ${MTL_SHADER_BUILD_FLAGS} -o "${CMAKE_CURRENT_BINARY_DIR}/texturedSquare.air" -c "${CMAKE_CURRENT_SOURCE_DIR}/texturedSquare.metal"
        DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/texturedSquare.metal"
    )
        
    add_custom_command(
        OUTPUT ${MTL_SHADER_LIB}
        COMMAND ${XCRUN_PATH} -sdk macosx metallib -o ${MTL_SHADER_LIB} "${CMAKE_CURRENT_BINARY_DIR}/texturedSquare.air"
        DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/texturedSquare.air"
    )
endif()