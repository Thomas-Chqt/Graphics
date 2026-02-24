# ---------------------------------------------------
# CMakeLists.txt
#
# Author: Thomas Choquet <semoir.dense-0h@icloud.com>
# ---------------------------------------------------

include(FetchContent)
include(GoogleTest)


add_executable(gfx_test)

# test file only require 11, will be higher when linking to something that need higher
target_compile_features(gfx_test PUBLIC cxx_std_11)
set_target_properties(gfx_test PROPERTIES FOLDER "tests")

file(GLOB_RECURSE SRC "*.cpp")
target_sources(gfx_test PRIVATE ${SRC})

target_include_directories(gfx_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

FetchContent_Declare(googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        main
    GIT_SHALLOW    v1.15.2
)
set(BUILD_GMOCK OFF)
set(INSTALL_GTEST OFF)
FetchContent_MakeAvailable(googletest)
set_target_properties(gtest PROPERTIES FOLDER "dependencies")
set_target_properties(gtest_main PROPERTIES FOLDER "dependencies")

target_link_libraries(gfx_test PRIVATE Graphics GTest::gtest_main)

gtest_discover_tests(gfx_test)
