/*
 * ---------------------------------------------------
 * gfx_glfw.mm
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2026/05/09
 * ---------------------------------------------------
 */

struct GLFWwindow;

#include "gfx_glfw/gfx_glfw.hpp"
#include "Graphics/Instance.hpp"

#include "Metal/MetalInstance.hpp"
#include "Metal/MetalSurface.hpp"
#if defined(GFX_BUILD_VULKAN)
# include "Vulkan/VulkanInstance.hpp"
# include "Vulkan/VulkanSurface.hpp"
#endif

extern "C" id glfwGetCocoaWindow(GLFWwindow*);
extern "C" const char** glfwGetRequiredInstanceExtensions(uint32_t*);
#if defined(GFX_BUILD_VULKAN)
extern "C" VkResult glfwCreateWindowSurface(VkInstance, GLFWwindow*, const VkAllocationCallbacks*, VkSurfaceKHR*);
#endif

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
    if (dynamic_cast<MetalInstance*>(&instance) != nullptr) { @autoreleasepool
    {
        CAMetalLayer* layer = [CAMetalLayer layer];
        layer.displaySyncEnabled = YES;

        NSWindow* nswindow = glfwGetCocoaWindow(window);
        nswindow.contentView.layer = layer;
        nswindow.contentView.wantsLayer = YES;

        return std::make_unique<MetalSurface>(layer);
    }}

#if defined(GFX_BUILD_VULKAN)
    if (auto* vulkanInstance = dynamic_cast<VulkanInstance*>(&instance))
    {
        VkSurfaceKHR surface = VK_NULL_HANDLE;
        if (glfwCreateWindowSurface(static_cast<VkInstance>(vulkanInstance->vkInstance()), window, nullptr, &surface) != VK_SUCCESS)
            throw std::runtime_error("unable to create the surface");
        return std::make_unique<VulkanSurface>(const_cast<vk::Instance&>(vulkanInstance->vkInstance()), surface);
    }
#endif

    throw std::runtime_error("unsupported gfx::Instance backend for GLFW surface creation");
}

} // namespace gfx::glfw
