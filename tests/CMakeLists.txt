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

if (ENABLE_LLVM_SOURCE_BASED_CODE_COVERAGE)
    target_compile_options(Graphics_test PRIVATE -fprofile-instr-generate -fcoverage-mapping)
    target_link_options(Graphics_test PRIVATE -fprofile-instr-generate -fcoverage-mapping)
endif()

if (ENABLE_LLVM_FSANITIZER)
    target_compile_options(Graphics_test PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer -fno-optimize-sibling-calls)
    target_link_options(Graphics_test    PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer -fno-optimize-sibling-calls)
endif()


target_link_libraries(Graphics_test PRIVATE Graphics)

FetchContent_MakeAvailable(googletest)
target_link_libraries(Graphics_test PRIVATE GTest::gtest_main)

gtest_discover_tests(Graphics_test)