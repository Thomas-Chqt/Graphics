# ---------------------------------------------------
# CMakeLists.txt
#
# Author: Thomas Choquet <semoir.dense-0h@icloud.com>
# Date: 2025/07/14 21:59:16
# ---------------------------------------------------

include(FetchContent)

find_package(slang QUIET)
if (NOT slang_FOUND)
    find_path(SLANG_INCLUDE_DIR slang.h HINTS ENV VULKAN_SDK PATH_SUFFIXES "include/slang" REQUIRED)
    find_library(SLANG_LIB slang HINTS ENV VULKAN_SDK PATH_SUFFIXES "lib" REQUIRED)
endif()

if (NOT slang_FOUND AND (SLANG_INCLUDE_DIR-NOTFOUND OR SLANG_LIB-NOTFOUND))
    message(FATAL_ERROR "slang not found")
endif()

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

# Remove undefined behavior sanitizer for gfxsc only (target-specific)
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(gfxsc PRIVATE -fno-sanitize=undefined)
    target_link_options(gfxsc PRIVATE -fno-sanitize=undefined)
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(gfxsc PRIVATE -fno-sanitize=undefined)
    target_link_options(gfxsc PRIVATE -fno-sanitize=undefined)
endif()

if (NOT SLANG_INCLUDE_DIR-NOTFOUND)
    target_include_directories(gfxsc PRIVATE ${SLANG_INCLUDE_DIR})
endif()

if (GFX_BUILD_METAL)
    target_compile_definitions(gfxsc PUBLIC "GFX_BUILD_METAL")
endif()

if (GFX_BUILD_VULKAN)
    target_compile_definitions(gfxsc PUBLIC "GFX_BUILD_VULKAN")
endif()

if (slang_FOUND)
    target_link_libraries(gfxsc PRIVATE slang::slang)
elseif(NOT SLANG_LIB-NOTFOUND)
    target_link_libraries(gfxsc PRIVATE ${SLANG_LIB})
endif()

target_link_libraries(gfxsc PRIVATE argparse)
