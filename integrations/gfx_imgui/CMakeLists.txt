add_library(GraphicsImGui STATIC)

target_compile_features(GraphicsImGui PUBLIC cxx_std_23)

target_include_directories(GraphicsImGui
    PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/src"
        "${CMAKE_SOURCE_DIR}/src"
        "${CMAKE_SOURCE_DIR}/include"
    PUBLIC
        "include")

target_precompile_headers(GraphicsImGui REUSE_FROM Graphics)

target_sources(GraphicsImGui PRIVATE "src/imconfig.h" "src/imgui.h")
if (GFX_BUILD_METAL)
    target_sources(GraphicsImGui PRIVATE "src/gfx_imgui.mm")
else()
    target_sources(GraphicsImGui PRIVATE "src/gfx_imgui.cpp")
endif()
if (GFX_BUILD_METAL)
    target_sources(GraphicsImGui PRIVATE "src/gfx_imgui.mm" "src/imgui_impl_metal.h" "src/imgui_impl_metal.mm")
endif()
if (GFX_BUILD_VULKAN)
    target_sources(GraphicsImGui PRIVATE "src/imgui_impl_vulkan.h" "src/imgui_impl_vulkan.cpp")
endif()

if (GFX_BUILD_VULKAN)
    target_compile_definitions(GraphicsImGui PRIVATE "GFX_BUILD_VULKAN")
    target_compile_definitions(GraphicsImGui PRIVATE "IMGUI_IMPL_VULKAN_NO_PROTOTYPES")
    target_compile_definitions(GraphicsImGui PRIVATE "VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1")
    target_compile_definitions(GraphicsImGui PRIVATE "VULKAN_HPP_NO_CONSTRUCTORS=1")
    target_compile_definitions(GraphicsImGui PRIVATE "VULKAN_HPP_HANDLES_MOVE_EXCHANGE=1")
    set_source_files_properties("${CMAKE_CURRENT_LIST_DIR}/src/imgui_impl_vulkan.cpp" PROPERTIES SKIP_PRECOMPILE_HEADERS ON)
endif()

if (GFX_BUILD_METAL)
    target_compile_definitions(GraphicsImGui PRIVATE "GFX_BUILD_METAL")
    target_compile_options(GraphicsImGui PRIVATE "-fobjc-arc")
endif()

if (GFX_BUILD_VULKAN)
    target_link_libraries(GraphicsImGui PRIVATE Vulkan::Headers VulkanMemoryAllocator)
endif()
