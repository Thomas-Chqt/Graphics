# ---------------------------------------------------
# CMakeLists.txt
#
# Author: Thomas Choquet <thomas.publique@icloud.com>
# Date: 2024/06/14 23:37:19
# ---------------------------------------------------

if(GFX_ENABLE_METAL)
    set(MTL_SHADER_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
    set(MTL_SHADER_LIB "${CMAKE_CURRENT_BINARY_DIR}/MTLShaderLib.metallib")
endif()
if(GFX_ENABLE_OPENGL)
    set(OPENGL_SHADER_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
endif()
set(RESSOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(camera)

set_target_properties(camera PROPERTIES 
    CXX_STANDARD          11
    CXX_STANDARD_REQUIRED ON
)

if(GFX_ENABLE_METAL)
    set_target_properties(camera PROPERTIES FOLDER "exemples/camera")
else()
    set_target_properties(camera PROPERTIES FOLDER "exemples")
endif()

file(GLOB EXE_SRCS "*.cpp" "*.hpp" "*.h")
target_sources(camera PRIVATE ${EXE_SRCS})

target_include_directories(camera PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

if(GFX_ENABLE_METAL)
    target_compile_definitions(camera PRIVATE "MTL_SHADER_LIB=\"${MTL_SHADER_LIB}\"")
endif()
if(GFX_ENABLE_OPENGL)
    target_compile_definitions(camera PRIVATE "OPENGL_SHADER_DIR=\"${OPENGL_SHADER_DIR}\"")
endif()
target_compile_definitions(camera PRIVATE "RESSOURCES_DIR=\"${RESSOURCES_DIR}\"")

target_link_libraries(camera PRIVATE Graphics)

if(GFX_ENABLE_METAL)
    add_custom_target(MTLShaderLib_camera DEPENDS ${MTL_SHADER_LIB})

    set_target_properties(MTLShaderLib_camera PROPERTIES FOLDER "exemples/camera")

    file(GLOB MTL_SHADER_LIB_SRCS "${MTL_SHADER_DIR}/*.metal")
    target_sources(MTLShaderLib_camera PRIVATE ${MTL_SHADER_LIB_SRCS})

    add_dependencies(camera MTLShaderLib_camera)
    

    get_target_property(EXE_INCLUDE_DIRS camera INCLUDE_DIRECTORIES)
    foreach(dir IN LISTS EXE_INCLUDE_DIRS)
        list(APPEND MTL_SHADER_BUILD_FLAGS "-I${dir}")
    endforeach()
    list(APPEND MTL_SHADER_BUILD_FLAGS "-I${CMAKE_SOURCE_DIR}/dependencies/Math/include")
    
    find_program(XCRUN_PATH xcrun REQUIRED)
    
    foreach(src IN LISTS MTL_SHADER_LIB_SRCS)
        string(REPLACE ".metal" ".air" air "${src}")
        string(REPLACE ${MTL_SHADER_DIR} ${CMAKE_CURRENT_BINARY_DIR} air "${air}")
        add_custom_command(OUTPUT "${air}" COMMAND ${XCRUN_PATH} -sdk macosx metal ${MTL_SHADER_BUILD_FLAGS} -o "${air}" -c "${src}" DEPENDS "${src}")
        list(APPEND MTL_SHADER_LIB_AIRS "${air}")
    endforeach()

    add_custom_command(OUTPUT ${MTL_SHADER_LIB} COMMAND ${XCRUN_PATH} -sdk macosx metallib -o ${MTL_SHADER_LIB} "${MTL_SHADER_LIB_AIRS}" DEPENDS "${MTL_SHADER_LIB_AIRS}")
endif()