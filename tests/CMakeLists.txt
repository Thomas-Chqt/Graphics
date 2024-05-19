# ---------------------------------------------------
# CMakeLists.txt
#
# Author: Thomas Choquet <thomas.publique@icloud.com>
# Date: 2024/05/15 13:50:09
# ---------------------------------------------------

include(FetchContent)
include(GoogleTest)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        main
)

add_executable(Graphics_test)

file(GLOB_RECURSE GRAPHICS_TEST_SRC "*.cpp" "*.tpp" "*.hpp")
target_sources(Graphics_test PRIVATE ${GRAPHICS_TEST_SRC})

target_include_directories(Graphics_test PRIVATE ".")

FetchContent_MakeAvailable(googletest)
target_link_libraries(Graphics_test Graphics GTest::gtest)

gtest_discover_tests(Graphics_test)