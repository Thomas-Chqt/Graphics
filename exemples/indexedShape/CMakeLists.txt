# ---------------------------------------------------
# CMakeLists.txt
#
# Author: Thomas Choquet <thomas.publique@icloud.com>
# Date: 2024/06/07 15:52:59
# ---------------------------------------------------

if(GFX_ENABLE_METAL)
    set(MTL_SHADER_LIB "${CMAKE_CURRENT_BINARY_DIR}/MTLShaderLib.metallib")
endif()
if(GFX_ENABLE_OPENGL)
    set(OPENGL_SHADER_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
endif()

add_executable(indexedShape)

set_target_properties(indexedShape PROPERTIES 
    CXX_STANDARD          11
    CXX_STANDARD_REQUIRED ON
)

if(GFX_ENABLE_METAL)
    set_target_properties(indexedShape PROPERTIES FOLDER "exemples/indexedShape")
else()
    set_target_properties(indexedShape PROPERTIES FOLDER "exemples")
endif()

file(GLOB EXE_SRCS "*.cpp" "*.hpp" "*.h")
target_sources(indexedShape PRIVATE ${EXE_SRCS})
if(GFX_ENABLE_METAL)
    file(GLOB EXE_SRCS "*.metal")
    target_sources(indexedShape PRIVATE ${EXE_SRCS})
endif()
if(GFX_ENABLE_OPENGL)
    file(GLOB EXE_SRCS "*.vs" "*.fs")
    target_sources(indexedShape PRIVATE ${EXE_SRCS})
endif()
target_include_directories(indexedShape PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

if(GFX_ENABLE_METAL)
    target_compile_definitions(indexedShape PRIVATE "MTL_SHADER_LIB=\"${MTL_SHADER_LIB}\"")
endif()
if(GFX_ENABLE_OPENGL)
    target_compile_definitions(indexedShape PRIVATE "OPENGL_SHADER_DIR=\"${OPENGL_SHADER_DIR}\"")
endif()

target_link_libraries(indexedShape PRIVATE Graphics)

if(GFX_ENABLE_METAL)
    add_custom_target(MTLShaderLib_indexedShape DEPENDS ${MTL_SHADER_LIB} SOURCES "${CMAKE_CURRENT_BINARY_DIR}/indexedShape.air")

    set_target_properties(MTLShaderLib_indexedShape PROPERTIES
        FOLDER "exemples/indexedShape"
    )

    target_sources(MTLShaderLib_indexedShape PRIVATE "indexedShape.metal")

    add_dependencies(indexedShape MTLShaderLib_indexedShape)
    

    get_target_property(INDEXEDSHAPE_INCLUDE_DIRS indexedShape INCLUDE_DIRECTORIES)
    foreach(dir IN LISTS INDEXEDSHAPE_INCLUDE_DIRS)
        list(APPEND MTL_SHADER_BUILD_FLAGS "-I${dir}")
    endforeach()
    
    find_program(XCRUN_PATH xcrun REQUIRED)
    
    add_custom_command(
        OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/indexedShape.air"
        COMMAND ${XCRUN_PATH} -sdk macosx metal ${MTL_SHADER_BUILD_FLAGS} -o "${CMAKE_CURRENT_BINARY_DIR}/indexedShape.air" -c "${CMAKE_CURRENT_SOURCE_DIR}/indexedShape.metal"
        DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/indexedShape.metal"
    )
        
    add_custom_command(
        OUTPUT ${MTL_SHADER_LIB}
        COMMAND ${XCRUN_PATH} -sdk macosx metallib -o ${MTL_SHADER_LIB} "${CMAKE_CURRENT_BINARY_DIR}/indexedShape.air"
        DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/indexedShape.air"
    )
endif()