include(FetchContent)

FetchContent_Declare(gml
    GIT_REPOSITORY    https://github.com/g-truc/glm.git
    GIT_TAG           1.0.1
    GIT_SHALLOW       1
    GIT_PROGRESS      TRUE
    FIND_PACKAGE_ARGS NAMES glm
)
FetchContent_MakeAvailable(gml)
if (gml_SOURCE_DIR)
    set_target_properties(glm PROPERTIES FOLDER "dependencies")
endif()

set(SHADER_TARGET_LIST)
if (GFX_BUILD_METAL)
    list(APPEND SHADER_TARGET_LIST metal)
endif()
if (GFX_BUILD_VULKAN)
    list(APPEND SHADER_TARGET_LIST spirv)
endif()
list(JOIN SHADER_TARGET_LIST "," SHADER_TARGETS)

set(SHADER_SLIB "${CMAKE_CURRENT_BINARY_DIR}/shader.slib")

add_custom_command(
    OUTPUT ${SHADER_SLIB}
    COMMAND $<TARGET_FILE:gfxsc> -t ${SHADER_TARGETS} -o ${SHADER_SLIB} "${CMAKE_CURRENT_SOURCE_DIR}/compute.slang"
    DEPENDS gfxsc "${CMAKE_CURRENT_SOURCE_DIR}/compute.slang" "${CMAKE_CURRENT_SOURCE_DIR}/shader.h"
    COMMENT "Building compute shader"
    VERBATIM
)
add_custom_target(compute_shader ALL DEPENDS ${SHADER_SLIB})
set_target_properties(compute_shader PROPERTIES FOLDER "examples/compute")

add_executable(compute compute.cpp)

set_target_properties(compute PROPERTIES FOLDER "examples/compute")

if(APPLE AND GFX_BUILD_VULKAN AND Vulkan_FOUND)
    get_filename_component(GFX_VULKAN_RUNTIME_DIRECTORY "${Vulkan_LIBRARY}" DIRECTORY)
    set_property(TARGET compute APPEND PROPERTY BUILD_RPATH "${GFX_VULKAN_RUNTIME_DIRECTORY}")
endif()

target_compile_features(compute PRIVATE cxx_std_23)

if(MSVC)
    target_compile_options(compute PRIVATE /W4)
else()
    target_compile_options(compute PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(compute PRIVATE $<$<CXX_COMPILER_ID:GNU>:-Wno-missing-field-initializers>)
endif()

target_compile_definitions(compute PRIVATE "SHADER_SLIB=\"${SHADER_SLIB}\"")

target_link_libraries(compute PRIVATE Graphics glm::glm)

add_dependencies(compute compute_shader)
