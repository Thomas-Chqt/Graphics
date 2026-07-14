include(FetchContent)

FetchContent_Declare(gfx_imgui_headers
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        v1.92.8-docking
    GIT_SHALLOW    1
    GIT_PROGRESS   TRUE
    SOURCE_SUBDIR  _headers_only_
)
FetchContent_MakeAvailable(gfx_imgui_headers)

add_library(GraphicsImGui STATIC)

target_compile_features(GraphicsImGui PUBLIC cxx_std_23)

if (GFX_BUILD_METAL)
    target_sources(GraphicsImGui PRIVATE "src/gfx_imgui.mm")
else()
    target_sources(GraphicsImGui PRIVATE "src/gfx_imgui.cpp")
endif()
if (GFX_BUILD_METAL)
    target_sources(GraphicsImGui PRIVATE "src/imgui_impl_metal.h" "src/imgui_impl_metal.mm")
endif()
if (GFX_BUILD_VULKAN)
    target_sources(GraphicsImGui PRIVATE "src/imgui_impl_vulkan.h" "src/imgui_impl_vulkan.cpp")
endif()

target_include_directories(GraphicsImGui
    PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/src"
        "${PROJECT_SOURCE_DIR}/src"
        "${PROJECT_SOURCE_DIR}/include"
    PUBLIC
        "include"
)
target_include_directories(GraphicsImGui SYSTEM PRIVATE "${gfx_imgui_headers_SOURCE_DIR}")

target_precompile_headers(GraphicsImGui REUSE_FROM Graphics)

if (GFX_BUILD_METAL)
    target_compile_definitions(GraphicsImGui PRIVATE "GFX_BUILD_METAL")
    target_compile_options(GraphicsImGui PRIVATE "-fobjc-arc")
endif()
if (GFX_BUILD_VULKAN)
    target_compile_definitions(GraphicsImGui PRIVATE "GFX_BUILD_VULKAN")
    target_compile_definitions(GraphicsImGui PRIVATE "VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1")
    target_compile_definitions(GraphicsImGui PRIVATE "VULKAN_HPP_NO_CONSTRUCTORS=1")
    target_compile_definitions(GraphicsImGui PRIVATE "VULKAN_HPP_HANDLES_MOVE_EXCHANGE=1")

    target_compile_definitions(GraphicsImGui PRIVATE "IMGUI_IMPL_VULKAN_NO_PROTOTYPES")
    set_source_files_properties("${CMAKE_CURRENT_LIST_DIR}/src/imgui_impl_vulkan.cpp" PROPERTIES SKIP_PRECOMPILE_HEADERS ON)
endif()

if (GFX_BUILD_METAL)
    target_link_libraries(GraphicsImGui PRIVATE ${METAL_FRAMEWORK} ${QUARTZ_CORE_FRAMEWORK} ${FOUNDATION_FRAMEWORK})
endif()
if (GFX_BUILD_VULKAN)
    target_link_libraries(GraphicsImGui PRIVATE Vulkan::Headers VulkanMemoryAllocator)
endif()
