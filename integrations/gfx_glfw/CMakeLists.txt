add_library(GraphicsGLFW STATIC)

target_compile_features(GraphicsGLFW PUBLIC cxx_std_23)

target_include_directories(GraphicsGLFW
    PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/src"
        "${CMAKE_SOURCE_DIR}/src"
        "${CMAKE_SOURCE_DIR}/include"
    PUBLIC
        "include")

target_precompile_headers(GraphicsGLFW REUSE_FROM Graphics)

if (GFX_BUILD_METAL)
    target_sources(GraphicsGLFW PRIVATE "src/gfx_glfw.mm")
else()
    target_sources(GraphicsGLFW PRIVATE "src/gfx_glfw.cpp")
endif()

if (GFX_BUILD_METAL)
    target_compile_options(GraphicsGLFW PRIVATE "-fobjc-arc")
endif()
if (GFX_BUILD_VULKAN)
    target_compile_definitions(GraphicsGLFW PRIVATE "VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1")
    target_compile_definitions(GraphicsGLFW PRIVATE "VULKAN_HPP_NO_CONSTRUCTORS=1")
    target_compile_definitions(GraphicsGLFW PRIVATE "VULKAN_HPP_HANDLES_MOVE_EXCHANGE=1")
endif()

if (GFX_BUILD_VULKAN)
    target_link_libraries(GraphicsGLFW PRIVATE Vulkan::Headers VulkanMemoryAllocator)
endif()
