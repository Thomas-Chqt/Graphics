# ---------------------------------------------------
# CMakeLists.txt
#
# Author: Thomas Choquet <thomas.publique@icloud.com>
# Date: 2024/06/05 15:24:43
# ---------------------------------------------------

add_library(MockLibs STATIC)
set_target_properties(MockLibs PROPERTIES CXX_STANDARD 14 CXX_STANDARD_REQUIRED ON)

if(USED_WINDOW_LIBRARY STREQUAL "GLFW")
    file(GLOB MOCKLIBS_SRC "includes/GLFW/*")
    target_sources(MockLibs PRIVATE ${MOCKLIBS_SRC})
    file(GLOB MOCKLIBS_SRC "src/GLFW/*")
    target_sources(MockLibs PRIVATE ${MOCKLIBS_SRC})
endif()
if(ENABLE_METAL)
    file(GLOB MOCKLIBS_SRC "includes/AppKit/*")
    target_sources(MockLibs PRIVATE ${MOCKLIBS_SRC})
    file(GLOB MOCKLIBS_SRC "src/AppKit/*")
    target_sources(MockLibs PRIVATE ${MOCKLIBS_SRC})
    file(GLOB MOCKLIBS_SRC "includes/QuartzCore/*")
    target_sources(MockLibs PRIVATE ${MOCKLIBS_SRC})
    file(GLOB MOCKLIBS_SRC "src/QuartzCore/*")
    target_sources(MockLibs PRIVATE ${MOCKLIBS_SRC})
    file(GLOB MOCKLIBS_SRC "includes/Metal/*")
    target_sources(MockLibs PRIVATE ${MOCKLIBS_SRC})
    file(GLOB MOCKLIBS_SRC "src/Metal/*")
    target_sources(MockLibs PRIVATE ${MOCKLIBS_SRC})
endif()
if(ENABLE_OPENGL)
    file(GLOB MOCKLIBS_SRC "includes/GL/*")
    target_sources(MockLibs PRIVATE ${MOCKLIBS_SRC})
    file(GLOB MOCKLIBS_SRC "src/GL/*")
    target_sources(MockLibs PRIVATE ${MOCKLIBS_SRC})
endif()

target_include_directories(MockLibs PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/includes")

target_link_libraries(MockLibs PUBLIC "-framework Cocoa")