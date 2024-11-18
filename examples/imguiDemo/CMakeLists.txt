# ---------------------------------------------------
# CMakeLists.txt
#
# Author: Thomas Choquet <thomas.publique@icloud.com>
# Date: 2024/06/07 15:02:06
# ---------------------------------------------------

add_executable(imguiDemo)

set_target_properties(imguiDemo PROPERTIES 
    CXX_STANDARD          17
    CXX_STANDARD_REQUIRED ON
    FOLDER                "examples"
)

target_sources(imguiDemo PRIVATE "main.cpp")

target_include_directories(imguiDemo PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(imguiDemo PRIVATE Graphics)
