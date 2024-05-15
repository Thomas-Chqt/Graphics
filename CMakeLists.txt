# ---------------------------------------------------
# CMakeLists.txt
#
# Author: Thomas Choquet <thomas.publique@icloud.com>
# Date: 2024/05/15 13:43:58
# ---------------------------------------------------

cmake_minimum_required(VERSION 3.23)

get_directory_property(hasParent PARENT_DIRECTORY)
if(NOT hasParent)
    project(Graphics LANGUAGES CXX)
endif()

enable_testing()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_SHARED_LIBS "Build using shared libraries" OFF)
option(GRAPHICS_BUILD_TESTS "Build utils cpp test executables" OFF)

if(BUILD_SHARED_LIBS)
    add_library(Graphics SHARED)
else()
    add_library(Graphics STATIC)
endif()

file(GLOB_RECURSE GRAPHICS_SRC "src/*.cpp" "src/*.tpp" "include/*.tpp" "src/*.hpp" "include/*.hpp")
target_sources(Graphics PRIVATE ${GRAPHICS_SRC})
target_include_directories(Graphics PRIVATE "src" PUBLIC "include")

if (ENABLE_LLVM_SOURCE_BASED_CODE_COVERAGE)
    target_compile_options(Graphics PRIVATE -fprofile-instr-generate -fcoverage-mapping)
    target_link_options(Graphics    PRIVATE -fprofile-instr-generate -fcoverage-mapping)
endif()

if (ENABLE_LLVM_FSANITIZER)
    target_compile_options(Graphics PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer -fno-optimize-sibling-calls)
    target_link_options(Graphics    PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer -fno-optimize-sibling-calls)
endif()

add_subdirectory(dependencies)

if (GRAPHICS_BUILD_TESTS)
    add_subdirectory(tests)
endif()