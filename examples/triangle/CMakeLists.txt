# ---------------------------------------------------
# CMakeLists.txt
#
# Author: Thomas Choquet <thomas.publique@icloud.com>
# Date: 2024/06/06 16:59:39
# ---------------------------------------------------

add_executable(triangle)

if (APPLE)
    enable_language(OBJC)
endif()

set_target_properties(triangle PROPERTIES FOLDER "examples/triangle")

file(GLOB EXE_SRCS "*.cpp" "*.hpp")
target_sources(triangle PRIVATE ${EXE_SRCS})

target_include_directories(triangle PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

FetchContent_Declare(glfw3
    GIT_REPOSITORY    https://github.com/glfw/glfw.git
    GIT_TAG           3.4
    GIT_SHALLOW       1
    GIT_PROGRESS      TRUE
    FIND_PACKAGE_ARGS
)
set(GLFW_BUILD_TESTS    OFF)
set(GLFW_BUILD_DOCS     OFF)
set(GLFW_INSTALL        OFF)
set(GLFW_BUILD_EXAMPLES OFF)
if(NOT GLFW_BUILD_WAYLAND)
    set(GLFW_BUILD_WAYLAND OFF)
endif()
FetchContent_MakeAvailable(glfw3)

get_target_property(GLFW_FOLDER glfw FOLDER)
set_target_properties(glfw PROPERTIES FOLDER "dependencies/${GLFW_FOLDER}")
if (TARGET update_mappings)
    get_target_property(UPDATE_MAP_FOLDER update_mappings FOLDER)
    set_target_properties(update_mappings PROPERTIES FOLDER "dependencies/${UPDATE_MAP_FOLDER}")
endif()

target_link_libraries(triangle PRIVATE Graphics glfw)
