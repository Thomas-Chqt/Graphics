# ---------------------------------------------------
# CMakeLists.txt
#
# Author: Thomas Choquet <semoir.dense-0h@icloud.com>
# Date: 2025/07/12 14:01:42
# ---------------------------------------------------

include(${ADD_SHADER_LIB_PATH})

add_executable(multiBuffer)

set(SHADER_TARGETS)

if (GFX_BUILD_METAL)
    list(APPEND SHADER_TARGETS "metal")
endif()
if (GFX_BUILD_VULKAN)
    list(APPEND SHADER_TARGETS "spirv")
endif()

add_shader_lib(multiBuffer_shader1
    SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/shader1.slang"
    TARGETS ${SHADER_TARGETS}
)
set_target_properties(multiBuffer_shader1 PROPERTIES FOLDER "examples/multiBuffer/shaders")
get_target_property(SHADER1_SLIB multiBuffer_shader1 SLIB_OUTPUT)

add_shader_lib(multiBuffer_shader2
    SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/shader2.slang"
    TARGETS ${SHADER_TARGETS}
)
set_target_properties(multiBuffer_shader2 PROPERTIES FOLDER "examples/multiBuffer/shaders")
get_target_property(SHADER2_SLIB multiBuffer_shader2 SLIB_OUTPUT)

add_shader_lib(multiBuffer_shader3
    SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/shader3.slang"
    TARGETS ${SHADER_TARGETS}
)
set_target_properties(multiBuffer_shader3 PROPERTIES FOLDER "examples/multiBuffer/shaders")
get_target_property(SHADER3_SLIB multiBuffer_shader3 SLIB_OUTPUT)

add_shader_lib(multiBuffer_shader4
    SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/shader4.slang"
    TARGETS ${SHADER_TARGETS}
)
set_target_properties(multiBuffer_shader4 PROPERTIES FOLDER "examples/multiBuffer/shaders")
get_target_property(SHADER4_SLIB multiBuffer_shader4 SLIB_OUTPUT)

if (APPLE)
    enable_language(OBJC)
endif()

set_target_properties(multiBuffer PROPERTIES
    FOLDER         "examples/multiBuffer"
    ENABLE_EXPORTS ON
)

file(GLOB EXE_SRCS "*.cpp" "*.hpp")
target_sources(multiBuffer PRIVATE ${EXE_SRCS})
if (WIN32)
    target_sources(multiBuffer PRIVATE multiBuffer.def)
endif()

target_include_directories(multiBuffer PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

target_compile_definitions(multiBuffer PRIVATE "SHADER1_SLIB=\"${SHADER1_SLIB}\"")
target_compile_definitions(multiBuffer PRIVATE "SHADER2_SLIB=\"${SHADER2_SLIB}\"")
target_compile_definitions(multiBuffer PRIVATE "SHADER3_SLIB=\"${SHADER3_SLIB}\"")
target_compile_definitions(multiBuffer PRIVATE "SHADER4_SLIB=\"${SHADER4_SLIB}\"")
if(CMAKE_GENERATOR STREQUAL "Xcode")
    target_compile_definitions(multiBuffer PRIVATE "__XCODE__")
endif()

FetchContent_Declare(glfw3
    GIT_REPOSITORY    https://github.com/glfw/glfw.git
    GIT_TAG           3.4
    GIT_SHALLOW       1
    GIT_PROGRESS      TRUE
    FIND_PACKAGE_ARGS
)
set(GLFW_BUILD_TESTS    OFF)
set(GLFW_BUILD_DOCS     OFF)
set(GLFW_INSTALL        OFF)
set(GLFW_BUILD_EXAMPLES OFF)
if(NOT GLFW_BUILD_WAYLAND)
    set(GLFW_BUILD_WAYLAND OFF)
endif()
FetchContent_MakeAvailable(glfw3)

get_target_property(GLFW_FOLDER glfw FOLDER)
set_target_properties(glfw PROPERTIES FOLDER "dependencies/${GLFW_FOLDER}")
if (TARGET update_mappings)
    get_target_property(UPDATE_MAP_FOLDER update_mappings FOLDER)
    set_target_properties(update_mappings PROPERTIES FOLDER "dependencies/${UPDATE_MAP_FOLDER}")
endif()

target_link_libraries(multiBuffer PRIVATE Graphics glfw)
add_dependencies(multiBuffer multiBuffer_shader1 multiBuffer_shader2 multiBuffer_shader3 multiBuffer_shader4)
