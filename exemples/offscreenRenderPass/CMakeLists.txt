# ---------------------------------------------------
# CMakeLists.txt
#
# Author: Thomas Choquet <thomas.publique@icloud.com>
# Date: 2024/07/12 17:43:26
# ---------------------------------------------------

if(GFX_BUILD_METAL)
    set(MTL_SHADER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/shaders")
    set(MTL_SHADER_LIB "${CMAKE_CURRENT_BINARY_DIR}/MTLShaderLib.metallib")
endif()
if(GFX_BUILD_OPENGL)
    set(OPENGL_SHADER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/shaders")
endif()
set(RESSOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ressources")

add_executable(offscreenRenderPass)

set_target_properties(offscreenRenderPass PROPERTIES 
    CXX_STANDARD          11
    CXX_STANDARD_REQUIRED ON
)

if(GFX_BUILD_METAL)
    set_target_properties(offscreenRenderPass PROPERTIES FOLDER "exemples/offscreenRenderPass")
else()
    set_target_properties(offscreenRenderPass PROPERTIES FOLDER "exemples")
endif()

file(GLOB EXE_SRCS "*.cpp" "*.hpp" "*.h")
target_sources(offscreenRenderPass PRIVATE ${EXE_SRCS})

target_include_directories(offscreenRenderPass PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

if(GFX_BUILD_METAL)
    target_compile_definitions(offscreenRenderPass PRIVATE "MTL_SHADER_LIB=\"${MTL_SHADER_LIB}\"")
endif()
if(GFX_BUILD_OPENGL)
    target_compile_definitions(offscreenRenderPass PRIVATE "OPENGL_SHADER_DIR=\"${OPENGL_SHADER_DIR}\"")
endif()
target_compile_definitions(offscreenRenderPass PRIVATE "RESSOURCES_DIR=\"${RESSOURCES_DIR}\"")

target_link_libraries(offscreenRenderPass PRIVATE Graphics)


if(GFX_BUILD_METAL)
    add_custom_target(MTLShaderLib_offscreenRenderPass DEPENDS ${MTL_SHADER_LIB})

    set_target_properties(MTLShaderLib_offscreenRenderPass PROPERTIES FOLDER "exemples/offscreenRenderPass")

    file(GLOB MTL_SHADER_LIB_SRCS "${MTL_SHADER_DIR}/*.metal")
    file(GLOB MTL_SHADER_LIB_HEAD "${MTL_SHADER_DIR}/*.metal.h")
    target_sources(MTLShaderLib_offscreenRenderPass PRIVATE ${MTL_SHADER_LIB_SRCS} ${MTL_SHADER_LIB_HEAD})

    add_dependencies(offscreenRenderPass MTLShaderLib_offscreenRenderPass)
    

    get_target_property(EXE_INCLUDE_DIRS offscreenRenderPass INCLUDE_DIRECTORIES)
    foreach(dir IN LISTS EXE_INCLUDE_DIRS)
        list(APPEND MTL_SHADER_BUILD_FLAGS "-I${dir}")
    endforeach()
    list(APPEND MTL_SHADER_BUILD_FLAGS "-I${CMAKE_SOURCE_DIR}/dependencies/Math/include")
    list(APPEND MTL_SHADER_BUILD_FLAGS "-I${MTL_SHADER_DIR}")
    list(APPEND MTL_SHADER_BUILD_FLAGS "-I${CMAKE_SOURCE_DIR}/dependencies/UtilsCPP/include")

    if(CMAKE_GENERATOR STREQUAL "Xcode")
        list(APPEND MTL_SHADER_BUILD_FLAGS "-gline-tables-only" "-frecord-sources")
    endif()
    
    find_program(XCRUN_PATH xcrun REQUIRED)
    
    foreach(src IN LISTS MTL_SHADER_LIB_SRCS)
        string(REPLACE ".metal" ".air" air "${src}")
        string(REPLACE ${MTL_SHADER_DIR} ${CMAKE_CURRENT_BINARY_DIR} air "${air}")
        add_custom_command(OUTPUT "${air}" COMMAND ${XCRUN_PATH} -sdk macosx metal ${MTL_SHADER_BUILD_FLAGS} -o "${air}" -c "${src}" DEPENDS "${src}")
        list(APPEND MTL_SHADER_LIB_AIRS "${air}")
    endforeach()

    add_custom_command(OUTPUT ${MTL_SHADER_LIB} COMMAND ${XCRUN_PATH} -sdk macosx metallib ${MTL_SHADER_LINK_FLAGS} -o ${MTL_SHADER_LIB} ${MTL_SHADER_LIB_AIRS} DEPENDS ${MTL_SHADER_LIB_AIRS})
endif()