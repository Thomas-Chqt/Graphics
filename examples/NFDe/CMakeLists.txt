# ---------------------------------------------------
# CMakeLists.txt
#
# Author: Thomas Choquet <semoir.dense-0h@icloud.com>
# Date: 2024/10/15 14:16:37
# ---------------------------------------------------

add_executable(NFDe)

set_target_properties(NFDe PROPERTIES 
    CXX_STANDARD          11
    CXX_STANDARD_REQUIRED ON
    FOLDER                "examples"
)

file(GLOB EXE_SRCS "*.cpp" "*.hpp" "*.h")
target_sources(NFDe PRIVATE ${EXE_SRCS})

target_include_directories(NFDe PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(NFDe PRIVATE Graphics)