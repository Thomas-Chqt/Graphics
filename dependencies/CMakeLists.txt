# ---------------------------------------------------
# CMakeLists.txt
#
# Author: Thomas Choquet <thomas.publique@icloud.com>
# Date: 2024/05/15 13:46:51
# ---------------------------------------------------

if(NOT TARGET UtilsCPP)
    execute_process(COMMAND git submodule update --init ${CMAKE_CURRENT_SOURCE_DIR}/UtilsCPP)
    add_subdirectory(UtilsCPP)
endif()
target_link_libraries(Graphics PUBLIC UtilsCPP)

if(NOT TARGET Logger)
    execute_process(COMMAND git submodule update --init ${CMAKE_CURRENT_SOURCE_DIR}/Logger)
    add_subdirectory(Logger)
endif()
target_link_libraries(Graphics PUBLIC Logger)

if(NOT TARGET Math)
    execute_process(COMMAND git submodule update --init ${CMAKE_CURRENT_SOURCE_DIR}/Math)
    add_subdirectory(Math)
endif()
target_link_libraries(Graphics PUBLIC Math)