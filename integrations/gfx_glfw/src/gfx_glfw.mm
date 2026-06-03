/*
 * ---------------------------------------------------
 * gfx_glfw.mm
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2026/05/09
 * ---------------------------------------------------
 */

#include "gfx_glfw/gfx_glfw.hpp"
#include "Graphics/Instance.hpp"

#include "Metal/MetalInstance.hpp"
#include "Metal/MetalSurface.hpp"

#if defined(GFX_BUILD_VULKAN)
    #include "GlfwInstanceExtension.hpp"
    #include "Vulkan/VulkanSurface.hpp"
#endif

extern "C" id glfwGetCocoaWindow(GLFWwindow*);

#if defined(GFX_BUILD_VULKAN)
    extern "C" VkResult glfwCreateWindowSurface(VkInstance, GLFWwindow*, const VkAllocationCallbacks*, VkSurfaceKHR*);
#endif

namespace gfx::glfw
{

const InstanceExtension* getInstanceExtension()
{
    #if defined(GFX_BUILD_VULKAN)

    static const GlfwInstanceExtension extension;
    return &extension;

    #else

    return nullptr;

    #endif
}

std::unique_ptr<Surface> createSurface(Instance& instance, GLFWwindow* window)
{
    if (dynamic_cast<MetalInstance*>(&instance) != nullptr)
    {
        @autoreleasepool
        {
            CAMetalLayer* layer = [CAMetalLayer layer];
            layer.displaySyncEnabled = YES;

            NSWindow* nswindow = glfwGetCocoaWindow(window);
            nswindow.contentView.layer = layer;
            nswindow.contentView.wantsLayer = YES;

            return std::make_unique<MetalSurface>(layer);
        }
    }

    #if defined(GFX_BUILD_VULKAN)
    if (auto* vulkanInstance = dynamic_cast<VulkanInstance*>(&instance))
    {
        VkSurfaceKHR surface = VK_NULL_HANDLE;
        if (glfwCreateWindowSurface(static_cast<VkInstance>(vulkanInstance->vkInstance()), window, nullptr, &surface) != VK_SUCCESS)
            throw std::runtime_error("unable to create the surface");
        return std::make_unique<VulkanSurface>(vulkanInstance->vkInstance(), surface);
    }
    #endif

    throw std::runtime_error("unsupported gfx::Instance backend for GLFW surface creation");
}

} // namespace gfx::glfw
