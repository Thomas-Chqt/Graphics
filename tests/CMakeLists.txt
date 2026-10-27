# ---------------------------------------------------
# CMakeLists.txt
#
# Author: Thomas Choquet <semoir.dense-0h@icloud.com>
# ---------------------------------------------------

include(FetchContent)
include(GoogleTest)


add_executable(gfx_test)

target_compile_features(gfx_test PUBLIC cxx_std_23)
set_target_properties(gfx_test PROPERTIES FOLDER "tests")

file(GLOB_RECURSE SRC "*.cpp")
list(REMOVE_ITEM SRC "${CMAKE_CURRENT_SOURCE_DIR}/test_tracy.cpp")
target_sources(gfx_test PRIVATE ${SRC})

target_include_directories(gfx_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

FetchContent_Declare(googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.15.2
    GIT_SHALLOW    1
)
set(BUILD_GMOCK OFF)
set(INSTALL_GTEST OFF)
FetchContent_MakeAvailable(googletest)
set_target_properties(gtest PROPERTIES FOLDER "dependencies")
set_target_properties(gtest_main PROPERTIES FOLDER "dependencies")

target_link_libraries(gfx_test PRIVATE Graphics GTest::gtest_main)
if(TARGET GraphicsTracy)
    target_link_libraries(gfx_test PRIVATE GraphicsTracy)
endif()

gtest_discover_tests(gfx_test)

if(TARGET GraphicsTracy AND (GFX_BUILD_METAL OR (GFX_BUILD_VULKAN AND NOT APPLE)))
    FetchContent_GetProperties(gfx_tracy_headers)
    find_package(Threads REQUIRED)
    add_executable(gfx_tracy_test test_tracy.cpp "${gfx_tracy_headers_SOURCE_DIR}/public/TracyClient.cpp")
    target_compile_features(gfx_tracy_test PRIVATE cxx_std_23)
    target_compile_definitions(gfx_tracy_test PRIVATE TRACY_ENABLE)
    target_include_directories(gfx_tracy_test SYSTEM PRIVATE "${gfx_tracy_headers_SOURCE_DIR}/public")
    target_link_libraries(gfx_tracy_test PRIVATE Graphics GraphicsTracy GTest::gtest_main Threads::Threads ${CMAKE_DL_LIBS})
    if(WIN32)
        target_link_libraries(gfx_tracy_test PRIVATE ws2_32 dbghelp)
    endif()
    gtest_discover_tests(gfx_tracy_test)
endif()
