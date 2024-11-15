# ---------------------------------------------------
# CMakeLists.txt
#
# Author: Thomas Choquet <semoir.dense-0h@icloud.com>
# Date: 2024/11/15 16:23:48
# ---------------------------------------------------

add_executable(NFDeChildWindow)

set_target_properties(NFDeChildWindow PROPERTIES
    CXX_STANDARD          11
    CXX_STANDARD_REQUIRED ON
    FOLDER                "examples"
)

file(GLOB EXE_SRCS "*.cpp" "*.hpp" "*.h")
target_sources(NFDeChildWindow PRIVATE ${EXE_SRCS})

target_include_directories(NFDeChildWindow PRIVATE "${CMAKE_CURRENT_LIST_DIR}")

target_link_libraries(NFDeChildWindow PRIVATE Graphics)