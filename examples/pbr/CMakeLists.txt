include(FetchContent)

## GLFW ##########################################################

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
if (APPLE)
    enable_language(OBJC)
endif()
FetchContent_MakeAvailable(glfw3)
if (glfw3_SOURCE_DIR)
    if (TARGET update_mappings)
        set_target_properties(glfw PROPERTIES FOLDER "dependencies/GLFW3")
        set_target_properties(update_mappings PROPERTIES FOLDER "dependencies/GLFW3")
    else()
        set_target_properties(glfw PROPERTIES FOLDER "dependencies")
    endif()
endif()

## GLM ###########################################################

FetchContent_Declare(gml
    GIT_REPOSITORY    https://github.com/g-truc/glm.git
    GIT_TAG           1.0.1
    GIT_SHALLOW       1
    GIT_PROGRESS      TRUE
    FIND_PACKAGE_ARGS NAMES glm
)
FetchContent_MakeAvailable(gml)
if (gml_SOURCE_DIR)
    set_target_properties(glm PROPERTIES FOLDER "dependencies")
endif()

## IMGUI #########################################################

FetchContent_Declare(imgui
    GIT_REPOSITORY    https://github.com/Thomas-Chqt/imgui.git
    GIT_TAG           9a22f710b92e7ab31d526e4948cdf8c6c6cce85c
    GIT_SHALLOW       0
    GIT_PROGRESS      TRUE
    FIND_PACKAGE_ARGS
)
set(IM_BUILD_GLFW ON)
FetchContent_MakeAvailable(imgui)
target_compile_definitions(imgui PRIVATE "GLFW_INCLUDE_NONE")
target_link_libraries(imgui PUBLIC glfw)
if (imgui_SOURCE_DIR)
    set_target_properties(imgui PROPERTIES FOLDER "dependencies")
endif()

## STB_IMAGE ###############################################################

FetchContent_Declare(stb_image
    GIT_REPOSITORY    https://github.com/Thomas-Chqt/stb_image.git
    GIT_TAG           a09b799a2ba95c14f511493db4ea59811b2fcc97
    GIT_SHALLOW       0
    GIT_PROGRESS      TRUE
    FIND_PACKAGE_ARGS
)
FetchContent_MakeAvailable(stb_image)
if (stb_image_SOURCE_DIR)
    set_target_properties(stb_image PROPERTIES FOLDER "dependencies")
endif()

##################################################################

add_subdirectory(shaders)

add_executable(pbr)

if (APPLE)
    enable_language(OBJC)
endif()

set_target_properties(pbr PROPERTIES FOLDER "examples/pbr")

target_compile_features(pbr PRIVATE cxx_std_23)

if(MSVC)
    target_compile_options(pbr PRIVATE /W4)
else()
    target_compile_options(pbr PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(pbr PRIVATE $<$<CXX_COMPILER_ID:GNU>:-Wno-missing-field-initializers>)
endif()

file(GLOB EXE_SRCS "*.cpp" "*.hpp")
target_sources(pbr PRIVATE ${EXE_SRCS})

target_include_directories(pbr PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

target_compile_definitions(pbr PRIVATE "SHADER_DIR=\"${SHADER_DIR}\"")
target_compile_definitions(pbr PRIVATE "RESOURCE_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/resources\"")
if(CMAKE_GENERATOR STREQUAL "Xcode")
    target_compile_definitions(pbr PRIVATE "__XCODE__")
endif()
target_compile_definitions(pbr PRIVATE "GLFW_INCLUDE_NONE")

target_link_libraries(pbr PRIVATE Graphics GraphicsGLFW GraphicsImGui glm::glm imgui stb_image)
add_dependencies(pbr pbr_shaders)

if(APPLE AND NOT CMAKE_GENERATOR STREQUAL "Xcode")
    set(CODESIGN_IDENTITY "-" CACHE STRING "Codesigning identity")

    set(PBR_ENTITLEMENTS_FILE "${CMAKE_CURRENT_SOURCE_DIR}/pbr.entitlements")
    if(EXISTS "${PBR_ENTITLEMENTS_FILE}")
        add_custom_command(TARGET pbr POST_BUILD
            COMMAND /bin/sh -c "codesign --sign \"${CODESIGN_IDENTITY}\" --force --entitlements \"${PBR_ENTITLEMENTS_FILE}\" \"$<TARGET_FILE:pbr>\" >/dev/null 2>&1"
            COMMENT "Codesigning pbr (with entitlements) [identity: ${CODESIGN_IDENTITY}]"
            VERBATIM
        )
    else()
        add_custom_command(TARGET pbr POST_BUILD
            COMMAND /bin/sh -c "codesign --sign \"${CODESIGN_IDENTITY}\" --force \"$<TARGET_FILE:pbr>\" >/dev/null 2>&1"
            COMMENT "Codesigning pbr [identity: ${CODESIGN_IDENTITY}]"
            VERBATIM
        )
    endif()
endif()
