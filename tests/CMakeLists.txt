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

file(GLOB GRAPHICS_TEST_SRC "*.cpp" "*.tpp" "*.hpp")
target_sources(Graphics_test PRIVATE ${GRAPHICS_TEST_SRC})

target_include_directories(Graphics_test PRIVATE ".")

if (ENABLE_METAL)
    add_subdirectory(Shaders)
endif()
    
if (ENABLE_OPENGL)
    set(OPENGL_SHADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Shaders/OpenGL)
    target_compile_definitions(Graphics_test PRIVATE "OPENGL_SHADER_DIR=\"${OPENGL_SHADER_DIR}\"")
endif()

if (ENABLE_IMGUI)
    add_subdirectory(imgui)
endif()

set(BUILD_GMOCK OFF)
FetchContent_MakeAvailable(googletest)
target_link_libraries(Graphics_test imgui Graphics GTest::gtest)

gtest_discover_tests(Graphics_test)