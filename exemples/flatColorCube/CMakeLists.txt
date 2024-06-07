# ---------------------------------------------------
# CMakeLists.txt
#
# Author: Thomas Choquet <thomas.publique@icloud.com>
# Date: 2024/06/07 16:34:59
# ---------------------------------------------------

if(GFX_ENABLE_METAL)
    set(MTL_SHADER_LIB "${CMAKE_CURRENT_BINARY_DIR}/MTLShaderLib.metallib")
endif()
if(GFX_ENABLE_OPENGL)
    set(OPENGL_SHADER_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
endif()

add_executable(flatColorCube)

set_target_properties(flatColorCube PROPERTIES 
    CXX_STANDARD          11
    CXX_STANDARD_REQUIRED ON
)

if(GFX_ENABLE_METAL)
    set_target_properties(flatColorCube PROPERTIES FOLDER "exemples/flatColorCube")
else()
    set_target_properties(flatColorCube PROPERTIES FOLDER "exemples")
endif()

file(GLOB EXE_SRCS "*.cpp" "*.hpp" "*.h")
target_sources(flatColorCube PRIVATE ${EXE_SRCS})
if(GFX_ENABLE_METAL)
    file(GLOB EXE_SRCS "*.metal")
    target_sources(flatColorCube PRIVATE ${EXE_SRCS})
endif()
if(GFX_ENABLE_OPENGL)
    file(GLOB EXE_SRCS "*.vs" "*.fs")
    target_sources(flatColorCube PRIVATE ${EXE_SRCS})
endif()
target_include_directories(flatColorCube PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

if(GFX_ENABLE_METAL)
    target_compile_definitions(flatColorCube PRIVATE "MTL_SHADER_LIB=\"${MTL_SHADER_LIB}\"")
endif()
if(GFX_ENABLE_OPENGL)
    target_compile_definitions(flatColorCube PRIVATE "OPENGL_SHADER_DIR=\"${OPENGL_SHADER_DIR}\"")
endif()

target_link_libraries(flatColorCube PRIVATE Graphics)

if(GFX_ENABLE_METAL)
    add_custom_target(MTLShaderLib_flatColorCube DEPENDS ${MTL_SHADER_LIB} SOURCES "${CMAKE_CURRENT_BINARY_DIR}/flatColorCube.air")

    set_target_properties(MTLShaderLib_flatColorCube PROPERTIES
        FOLDER "exemples/flatColorCube"
    )

    target_sources(MTLShaderLib_flatColorCube PRIVATE "flatColorCube.metal")

    add_dependencies(flatColorCube MTLShaderLib_flatColorCube)
    

    get_target_property(EXE_INCLUDE_DIRS flatColorCube INCLUDE_DIRECTORIES)
    foreach(dir IN LISTS EXE_INCLUDE_DIRS)
        list(APPEND MTL_SHADER_BUILD_FLAGS "-I${dir}")
    endforeach()
    
    find_program(XCRUN_PATH xcrun REQUIRED)
    
    add_custom_command(
        OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/flatColorCube.air"
        COMMAND ${XCRUN_PATH} -sdk macosx metal ${MTL_SHADER_BUILD_FLAGS} -o "${CMAKE_CURRENT_BINARY_DIR}/flatColorCube.air" -c "${CMAKE_CURRENT_SOURCE_DIR}/flatColorCube.metal"
        DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/flatColorCube.metal"
    )
        
    add_custom_command(
        OUTPUT ${MTL_SHADER_LIB}
        COMMAND ${XCRUN_PATH} -sdk macosx metallib -o ${MTL_SHADER_LIB} "${CMAKE_CURRENT_BINARY_DIR}/flatColorCube.air"
        DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/flatColorCube.air"
    )
endif()