# ---------------------------------------------------
# CMakeLists.txt
#
# Author: Thomas Choquet <semoir.dense-0h@icloud.com>
# Date: 2025/07/14 21:59:16
# ---------------------------------------------------

include(FetchContent)
include(find_slang)

find_slang()

FetchContent_Declare(argparse
    GIT_REPOSITORY    https://github.com/p-ranav/argparse.git
    GIT_TAG           v3.1
    GIT_SHALLOW       1
    GIT_PROGRESS      TRUE
    FIND_PACKAGE_ARGS
)
FetchContent_MakeAvailable(argparse)

add_executable(gfxsc)

set_target_properties(gfxsc PROPERTIES FOLDER "tools")

target_compile_features(gfxsc PUBLIC cxx_std_23)

if(MSVC)
    target_compile_options(gfxsc PRIVATE /W4)
else()
    target_compile_options(gfxsc PRIVATE -Wall -Wextra -Wpedantic)
endif()

target_sources(gfxsc PRIVATE "gfxsc.cpp")

target_include_directories(gfxsc PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(gfxsc PRIVATE -fno-sanitize=undefined)
    target_link_options(gfxsc PRIVATE -fno-sanitize=undefined)
endif()

if (GFX_BUILD_METAL)
    target_compile_definitions(gfxsc PUBLIC "GFX_BUILD_METAL")
endif()

if (GFX_BUILD_VULKAN)
    target_compile_definitions(gfxsc PUBLIC "GFX_BUILD_VULKAN")
endif()

target_link_libraries(gfxsc PRIVATE slang::slang argparse::argparse)

if (WIN32)
    add_custom_command(TARGET gfxsc POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_RUNTIME_DLLS:gfxsc> $<TARGET_FILE_DIR:gfxsc>
    )
    get_target_property(SLANG_GLSLANG_DLL slang::slang SLANG_GLSLANG_DLL_PATH)
    if(SLANG_GLSLANG_DLL)
        add_custom_command(TARGET gfxsc POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SLANG_GLSLANG_DLL} $<TARGET_FILE_DIR:gfxsc>
        )
    endif()
endif()
