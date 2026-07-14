include(FetchContent)

FetchContent_Declare(gfx_tracy_headers
    GIT_REPOSITORY https://github.com/wolfpld/tracy.git
    GIT_TAG        v0.13.0
    GIT_SHALLOW    1
    GIT_PROGRESS   TRUE
    SOURCE_SUBDIR  _headers_only_
)
FetchContent_MakeAvailable(gfx_tracy_headers)

add_library(GraphicsTracy STATIC)

target_compile_features(GraphicsTracy PUBLIC cxx_std_23)

target_sources(GraphicsTracy PRIVATE "include/gfx_tracy/gfx_tracy.hpp")
if (GFX_BUILD_METAL)
    target_sources(GraphicsTracy PRIVATE "src/gfx_tracy.mm")
else()
    target_sources(GraphicsTracy PRIVATE "src/gfx_tracy.cpp")
endif()
if (GFX_BUILD_VULKAN)
    target_sources(GraphicsTracy PRIVATE "src/gfx_tracy_vulkan.hpp")
endif()

target_include_directories(GraphicsTracy
    PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/src"
        "${PROJECT_SOURCE_DIR}/src"
        "${PROJECT_SOURCE_DIR}/include"
    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/include"
)
target_include_directories(GraphicsTracy SYSTEM PRIVATE "${gfx_tracy_headers_SOURCE_DIR}/public")

target_precompile_headers(GraphicsTracy REUSE_FROM Graphics)

target_compile_definitions(GraphicsTracy PRIVATE "TRACY_ENABLE")
if (GFX_BUILD_METAL)
    target_compile_definitions(GraphicsTracy PRIVATE "GFX_BUILD_METAL")
    target_compile_options(GraphicsTracy PRIVATE "-fobjc-arc")
endif()
if (GFX_BUILD_VULKAN)
    target_compile_definitions(GraphicsTracy PRIVATE "GFX_BUILD_VULKAN")
    target_compile_definitions(GraphicsTracy PRIVATE "VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1")
    target_compile_definitions(GraphicsTracy PRIVATE "VULKAN_HPP_NO_CONSTRUCTORS=1")
    target_compile_definitions(GraphicsTracy PRIVATE "VULKAN_HPP_HANDLES_MOVE_EXCHANGE=1")

    target_compile_definitions(GraphicsTracy PRIVATE "TRACY_VK_USE_SYMBOL_TABLE")
endif()
target_compile_definitions(GraphicsTracy PRIVATE "GFX_TRACY_IMPL")

if (GFX_BUILD_METAL)
    target_link_libraries(GraphicsTracy PRIVATE ${METAL_FRAMEWORK} ${QUARTZ_CORE_FRAMEWORK} ${FOUNDATION_FRAMEWORK})
endif()
if (GFX_BUILD_VULKAN)
    target_link_libraries(GraphicsTracy PRIVATE Vulkan::Headers VulkanMemoryAllocator)
endif()
