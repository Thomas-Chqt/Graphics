# ---------------------------------------------------
# CMakeLists.txt
#
# Author: Thomas Choquet <thomas.publique@icloud.com>
# Date: 2024/05/15 13:50:09
# ---------------------------------------------------

include(FetchContent)
include(GoogleTest)


add_executable(Graphics_test)

set_target_properties(Graphics_test PROPERTIES CXX_STANDARD 14 CXX_STANDARD_REQUIRED ON)


file(GLOB GRAPHICS_TEST_SRC "*.cpp" "*.tpp" "*.hpp")
target_sources(Graphics_test PRIVATE ${GRAPHICS_TEST_SRC})

target_include_directories(Graphics_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})


target_link_libraries(Graphics_test PRIVATE Graphics)

if (ENABLE_IMGUI)
    add_subdirectory("imgui")
    target_link_libraries(Graphics_test PRIVATE imgui)
endif()

set(BUILD_GMOCK OFF)
FetchContent_Declare(googletest GIT_REPOSITORY https://github.com/google/googletest.git GIT_TAG main)
FetchContent_MakeAvailable(googletest)
target_link_libraries(Graphics_test PRIVATE GTest::gtest)

add_subdirectory("Shaders")
if(ENABLE_METAL)
    target_compile_definitions(Graphics_test PRIVATE "MTL_SHADER_LIB=\"${MTL_SHADER_LIB}\"")
    add_dependencies(Graphics_test gfx_tests_MetalShaderLibrary)
endif()
if(ENABLE_OPENGL)
    target_compile_definitions(Graphics_test PRIVATE "OPENGL_SHADER_DIR=\"${OPENGL_SHADER_DIR}\"")
endif()


gtest_discover_tests(Graphics_test WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})