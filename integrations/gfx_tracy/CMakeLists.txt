add_library(GraphicsTracy STATIC)

target_compile_features(GraphicsTracy PUBLIC cxx_std_23)

target_include_directories(GraphicsTracy
    PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/src"
        "${PROJECT_SOURCE_DIR}/src"
        "${PROJECT_SOURCE_DIR}/include"
    PUBLIC
        "include")

target_precompile_headers(GraphicsTracy REUSE_FROM Graphics)

target_sources(GraphicsTracy PRIVATE
    "include/gfx_tracy/gfx_tracy.hpp"
    "src/gfx_tracy.cpp"
    "src/gfx_tracy_private.hpp")

if (GFX_BUILD_METAL)
    target_sources(GraphicsTracy PRIVATE "src/gfx_tracy_metal.mm")
endif()
if (GFX_BUILD_VULKAN)
    target_sources(GraphicsTracy PRIVATE "src/gfx_tracy_vulkan.cpp")
endif()

target_compile_definitions(GraphicsTracy PUBLIC "GFX_BUILD_TRACY_INTEGRATION")

if (GFX_BUILD_METAL)
    target_compile_definitions(GraphicsTracy PRIVATE "GFX_BUILD_METAL")
    target_compile_options(GraphicsTracy PRIVATE "-fobjc-arc")
endif()
if (GFX_BUILD_VULKAN)
    target_compile_definitions(GraphicsTracy PRIVATE "GFX_BUILD_VULKAN")
    target_compile_definitions(GraphicsTracy PRIVATE "TRACY_VK_USE_SYMBOL_TABLE")
    target_compile_definitions(GraphicsTracy PRIVATE "VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1")
    target_compile_definitions(GraphicsTracy PRIVATE "VULKAN_HPP_NO_CONSTRUCTORS=1")
    target_compile_definitions(GraphicsTracy PRIVATE "VULKAN_HPP_HANDLES_MOVE_EXCHANGE=1")
endif()

target_link_libraries(GraphicsTracy PUBLIC TracyClient)

if (GFX_BUILD_METAL)
    target_link_libraries(GraphicsTracy PRIVATE ${METAL_FRAMEWORK} ${QUARTZ_CORE_FRAMEWORK} ${FOUNDATION_FRAMEWORK})
endif()
if (GFX_BUILD_VULKAN)
    target_link_libraries(GraphicsTracy PRIVATE Vulkan::Headers VulkanMemoryAllocator)
endif()
