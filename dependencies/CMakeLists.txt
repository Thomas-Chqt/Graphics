# ---------------------------------------------------
# CMakeLists.txt
#
# Author: Thomas Choquet <thomas.publique@icloud.com>
# Date: 2024/05/15 13:46:51
# ---------------------------------------------------

if(NOT TARGET UtilsCPP)
    execute_process(COMMAND git submodule update --init ${CMAKE_SOURCE_DIR}/dependencies/UtilsCPP)
    add_subdirectory(UtilsCPP)
endif()
target_link_libraries(Graphics PUBLIC UtilsCPP)

if(NOT TARGET Logger)
    execute_process(COMMAND git submodule update --init ${CMAKE_SOURCE_DIR}/dependencies/Logger)
    add_subdirectory(Logger)
endif()
target_link_libraries(Graphics PUBLIC Logger)