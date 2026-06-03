/*
 * ---------------------------------------------------
 * gfx_glfw.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2026/05/09
 * ---------------------------------------------------
 */

struct GLFWwindow;

#include "gfx_glfw/gfx_glfw.hpp"
#include "Graphics/Instance.hpp"

#include "Vulkan/VulkanInstance.hpp"
#include "Vulkan/VulkanSurface.hpp"

extern "C" const char** glfwGetRequiredInstanceExtensions(uint32_t*);
extern "C" VkResult glfwCreateWindowSurface(VkInstance, GLFWwindow*, const VkAllocationCallbacks*, VkSurfaceKHR*);

namespace gfx::glfw
{

namespace
{

class GlfwInstanceExtension : public InstanceExtension
{
public:
    std::span<const std::string_view> getRequiredVulkanInstanceExtensions() const override
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
};

} // namespace

const InstanceExtension* newInstanceExtension()
{
    static const GlfwInstanceExtension extension;
    return &extension;
}

std::unique_ptr<Surface> createSurface(Instance& instance, GLFWwindow* window)
{
    if (auto* vulkanInstance = dynamic_cast<VulkanInstance*>(&instance))
    {
        VkSurfaceKHR surface = VK_NULL_HANDLE;
        if (glfwCreateWindowSurface(static_cast<VkInstance>(vulkanInstance->vkInstance()), window, nullptr, &surface) != VK_SUCCESS)
            throw std::runtime_error("unable to create the surface");
        return std::make_unique<VulkanSurface>(const_cast<vk::Instance&>(vulkanInstance->vkInstance()), surface);
    }

    throw std::runtime_error("unsupported gfx::Instance backend for GLFW surface creation");
}

} // namespace gfx::glfw
