# ---------------------------------------------------
# CMakeLists.txt
#
# Author: Thomas Choquet <thomas.publique@icloud.com>
# Date: 2024/06/07 13:34:03
# ---------------------------------------------------

add_executable(setClearColor)

set_target_properties(setClearColor PROPERTIES FOLDER "exemples")

target_sources(setClearColor PRIVATE "main.cpp")

target_include_directories(setClearColor PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(setClearColor PRIVATE Graphics)