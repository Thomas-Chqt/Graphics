# ---------------------------------------------------
# CMakeLists.txt
#
# Author: Thomas Choquet <thomas.publique@icloud.com>
# Date: 2024/05/20 18:00:16
# ---------------------------------------------------

if (ENABLE_OPENGL)
    set(OPENGL_SHADER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/OpenGL" PARENT_SCOPE)
endif()

if(ENABLE_METAL)
    get_target_property(GRAPHICS_TEST_INCLUDE_DIRS Graphics_test INCLUDE_DIRECTORIES) 
    foreach(dir IN LISTS GRAPHICS_TEST_INCLUDE_DIRS)
        list(APPEND MTL_SHADER_BUILD_FLAGS "-I${dir}")
    endforeach()
    # if(CMAKE_GENERATOR STREQUAL Xcode)
        # list(APPEND MTL_SHADER_BUILD_FLAGS "")
    # endif()

    set(MTL_SHADER_LIB        "${CMAKE_CURRENT_BINARY_DIR}/MetalShaderLibrary.metallib")
    set(MTL_SHADER_LIB         ${MTL_SHADER_LIB} PARENT_SCOPE)
    set(MTL_SHADER_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/Metal")
    set(MTL_SHADER_BUILD_DIR  "${CMAKE_CURRENT_BINARY_DIR}")
        
    file(GLOB_RECURSE MTL_SHADER_LIB_SRCS "${MTL_SHADER_SOURCE_DIR}/*.metal")
    
    find_program(XCRUN_PATH xcrun REQUIRED)
    
    foreach(src IN LISTS MTL_SHADER_LIB_SRCS)
        string(REPLACE ".metal" ".air" air "${src}")
        string(REPLACE ${MTL_SHADER_SOURCE_DIR} ${MTL_SHADER_BUILD_DIR} air "${air}")
        
        add_custom_command(OUTPUT "${air}" COMMAND ${XCRUN_PATH} -sdk macosx metal ${MTL_SHADER_BUILD_FLAGS} -o "${air}" -c "${src}" DEPENDS "${src}")
        
        list(APPEND MTL_SHADER_LIB_AIRS "${air}")
    endforeach()
    
    add_custom_command(OUTPUT ${MTL_SHADER_LIB} COMMAND ${XCRUN_PATH} -sdk macosx metallib -o ${MTL_SHADER_LIB} ${MTL_SHADER_LIB_AIRS} DEPENDS ${MTL_SHADER_LIB_AIRS})
    
    add_custom_target(gfx_tests_MetalShaderLibrary DEPENDS ${MTL_SHADER_LIB} SOURCES ${MTL_SHADER_LIB_SRCS})
endif()