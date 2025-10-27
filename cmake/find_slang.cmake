# ---------------------------------------------------
# find_slang.cmake
#
# Find or download prebuilt Slang shader compiler
#
# This module creates a `slang` target that can be used
# for shader compilation.
#
# Author: Thomas Choquet <thomas.publique@icloud.com>
# Date: 2025/10/22
# ---------------------------------------------------

include(FetchContent)

function(find_slang)
    set(SLANG_SEARCH_INCLUDE_PATHS /usr/include /usr/local/include /opt/include /opt/local/include $ENV{VULKAN_SDK}/include $ENV{PROGRAMFILES}/slang/include $ENV{HOME}/.local/include)
    set(SLANG_SEARCH_LIB_PATHS /usr/lib /usr/local/lib /opt/lib /opt/local/lib $ENV{VULKAN_SDK}/lib $ENV{PROGRAMFILES}/slang/lib $ENV{HOME}/.local/lib)
    set(SLANG_SEARCH_BIN_PATHS $ENV{PATH} $ENV{VULKAN_SDK}/bin $ENV{PROGRAMFILES}/slang/bin $ENV{HOME}/.local/bin)

    if(NOT GFX_SLANG_FORCE_DOWNLOAD)
        find_path(SLANG_INCLUDE_DIR NAMES slang.h PATHS ${SLANG_SEARCH_INCLUDE_PATHS} PATH_SUFFIXES slang)
        if(SLANG_INCLUDE_DIR)
            find_file(SLANG_COM_PTR NAMES slang-com-ptr.h PATHS ${SLANG_INCLUDE_DIR})
            if (NOT SLANG_COM_PTR)
                unset(SLANG_INCLUDE_DIR CACHE)
            endif()
        endif()
        find_library(SLANG_LIB NAMES slang PATHS ${SLANG_SEARCH_LIB_PATHS})
        if (WIN32)
            find_file(SLANG_DLL NAMES slang.dll PATHS ${SLANG_SEARCH_BIN_PATHS})
            find_file(SLANG_GLSLANG_DLL NAMES slang-glslang.dll PATHS ${SLANG_SEARCH_BIN_PATHS})
        endif()
    endif()

    if(NOT SLANG_INCLUDE_DIR OR NOT SLANG_LIB)
        if(APPLE)
            set(SLANG_PLATFORM "macos")
        elseif(WIN32)
            set(SLANG_PLATFORM "windows")
        elseif(UNIX)
            set(SLANG_PLATFORM "linux")
        else()
            message(FATAL_ERROR "Unsupported platform for Slang prebuilt binaries")
        endif()

        if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm64|aarch64")
            string(APPEND SLANG_PLATFORM "-aarch64")
        else()
            string(APPEND SLANG_PLATFORM "-x86_64")
        endif()

        FetchContent_Declare(slang URL "https://github.com/shader-slang/slang/releases/download/v2025.19.1/slang-2025.19.1-${SLANG_PLATFORM}.zip" DOWNLOAD_EXTRACT_TIMESTAMP TRUE)
        FetchContent_MakeAvailable(slang)

        find_path(SLANG_INCLUDE_DIR NAMES slang.h PATHS ${slang_SOURCE_DIR}/include PATH_SUFFIXES slang REQUIRED)
        find_file(SLANG_COM_PTR NAMES slang-com-ptr.h PATHS ${SLANG_INCLUDE_DIR} REQUIRED)
        find_library(SLANG_LIB NAMES slang PATHS ${slang_SOURCE_DIR}/lib REQUIRED)
        if (WIN32)
            find_file(SLANG_DLL NAMES slang.dll PATHS ${slang_SOURCE_DIR}/bin REQUIRED)
            find_file(SLANG_GLSLANG_DLL NAMES slang-glslang.dll PATHS ${slang_SOURCE_DIR}/bin REQUIRED)
        endif()
    endif()

    message(STATUS "slang headers found: ${SLANG_INCLUDE_DIR}")
    message(STATUS "slang library found: ${SLANG_LIB}")

    add_library(slang::slang SHARED IMPORTED GLOBAL)
    target_include_directories(slang::slang INTERFACE ${SLANG_INCLUDE_DIR})
    target_link_libraries(slang::slang INTERFACE ${SLANG_LIB})
    set_target_properties(slang::slang PROPERTIES IMPORTED_IMPLIB ${SLANG_LIB})
    if (WIN32)
        set_target_properties(slang::slang PROPERTIES IMPORTED_LOCATION ${SLANG_DLL})
        if(SLANG_GLSLANG_DLL)
            set_target_properties(slang::slang PROPERTIES SLANG_GLSLANG_DLL_PATH ${SLANG_GLSLANG_DLL})
        endif()
    endif()
endfunction()
