# ---------------------------------------------------
# CMakeLists.txt
#
# Author: Thomas Choquet <thomas.publique@icloud.com>
# Date: 2024/05/22 16:59:43
# ---------------------------------------------------

file(GLOB FILES "*.cpp" "*.hpp")
list(APPEND GRAPHICS_SRC ${FILES})

if (GFX_BUILD_METAL)
    file(GLOB_RECURSE FILES "Metal/*.cpp" "Metal/*.mm" "Metal/*.hpp")
    list(APPEND GRAPHICS_SRC ${FILES})
endif()

if (GFX_BUILD_VULKAN)
    file(GLOB_RECURSE FILES "Vulkan/*.cpp" "Vulkan/*.mm" "Metal/*.hpp")
    list(APPEND GRAPHICS_SRC ${FILES})
endif()

target_sources(Graphics PRIVATE ${GRAPHICS_SRC})
