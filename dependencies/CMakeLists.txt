# ---------------------------------------------------
# CMakeLists.txt
#
# Author: Thomas Choquet <thomas.publique@icloud.com>
# Date: 2024/05/15 13:46:51
# ---------------------------------------------------

if(NOT TARGET UtilsCPP)
    find_program(GIT_PATH git REQUIRED)
    execute_process(COMMAND ${GIT_PATH} submodule update --init UtilsCPP WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    set(UTILSCPP_BUILD_TESTS OFF)
    set(UTILSCPP_INSTALL ${GFX_INSTALL})
    add_subdirectory(UtilsCPP)
    set_target_properties(UtilsCPP PROPERTIES FOLDER "dependencies")
endif()
target_link_libraries(Graphics PUBLIC UtilsCPP)

if(NOT TARGET Math)
    find_program(GIT_PATH git REQUIRED)
    execute_process(COMMAND ${GIT_PATH} submodule update --init Math WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    set(MATH_BUILD_TESTS OFF)
    set(MATH_INSTALL ${GFX_INSTALL})
    add_subdirectory(Math)
    set_target_properties(Math PROPERTIES FOLDER "dependencies")
endif()
target_link_libraries(Graphics PUBLIC Math)