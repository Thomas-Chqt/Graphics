#include "GlfwInstanceExtension.hpp"

#include <span>
#include <string_view>
#include <vector>

extern "C" const char** glfwGetRequiredInstanceExtensions(uint32_t*);

namespace gfx::glfw
{

std::span<const std::string_view> GlfwInstanceExtension::getRequiredVulkanInstanceExtensions() const
{
    static const std::vector<std::string_view> extensions = [] {
        uint32_t extensionCount = 0;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&extensionCount);
        if (glfwExtensions == nullptr || extensionCount == 0)
            throw std::runtime_error("unable to query GLFW Vulkan instance extensions");

        std::vector<std::string_view> result;
        result.reserve(extensionCount);
        for (uint32_t i = 0; i < extensionCount; ++i)
            result.emplace_back(glfwExtensions[i]);
        return result;
    }();

    return extensions;
}

} // namespace gfx::glfw
