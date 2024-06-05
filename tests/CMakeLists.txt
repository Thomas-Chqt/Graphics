# ---------------------------------------------------
# CMakeLists.txt
#
# Author: Thomas Choquet <thomas.publique@icloud.com>
# Date: 2024/06/04 12:47:23
# ---------------------------------------------------

include(FetchContent)
include(GoogleTest)

add_executable(GFX_tests)
set_target_properties(GFX_tests PROPERTIES CXX_STANDARD 14 CXX_STANDARD_REQUIRED ON)

file(GLOB GFX_TESTS_SRC "*.cpp" "*.tpp" "*.hpp" "*.h")
target_sources(GFX_tests PRIVATE ${GFX_TESTS_SRC})

target_include_directories(GFX_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_link_options(GFX_tests PRIVATE "-Wl,-undefined,dynamic_lookup")
endif()

# if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # target_link_options(GFX_tests PRIVATE "-Wl,--unresolved-symbols=ignore-in-object-files")
#     
# elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
#     target_link_options(my_executable PRIVATE "/FORCE")
# endif()

target_link_libraries(GFX_tests PRIVATE Graphics)

FetchContent_Declare(googletest GIT_REPOSITORY https://github.com/google/googletest.git GIT_TAG main)
FetchContent_MakeAvailable(googletest)
target_link_libraries(GFX_tests PRIVATE GTest::gtest_main GTest::gmock)

add_subdirectory(MockLibs)
if (ENABLE_METAL)
    target_compile_definitions(MockLibs PRIVATE METAL_ENABLED) 
endif()
if (ENABLE_OPENGL)
    target_compile_definitions(MockLibs PRIVATE OPENGL_ENABLED)
endif()

target_link_libraries(GFX_tests PRIVATE MockLibs)

gtest_discover_tests(GFX_tests WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})