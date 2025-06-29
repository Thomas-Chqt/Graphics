/*
 * ---------------------------------------------------
 * VulkanInstance.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/05/28 06:24:05
 * ---------------------------------------------------
 */

#define VULKAN_HPP_NO_CONSTRUCTORS
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1

#include "Graphics/Device.hpp"
#include "Graphics/Instance.hpp"

#include "Vulkan/VulkanInstance.hpp"
#include "Vulkan/VulkanSurface.hpp"
#include "Vulkan/VulkanPhysicalDevice.hpp"
#include "Vulkan/VulkanDevice.hpp"

#include <vulkan/vulkan.hpp>

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

#if defined(GFX_USE_UTILSCPP)
    #include "UtilsCPP/memory.hpp"
    namespace ext = utl;
#else
    #include <memory>
    #include <vector>
    #include <array>
    #include <iostream>
    #include <stdint.h>
    #include <stdexcept>
    #include <cassert>
    #include <cstddef>
    #include <cstdint>
    #include <ranges>
    namespace ext = std;
#endif

#if defined(GFX_GLFW_ENABLED)
    #include <dlLoad/dlLoad.h>
    class GLFWwindow;
    #define glfwGetRequiredInstanceExtensions ((const char** (*)(uint32_t* count))::getSym(DL_DEFAULT, "glfwGetRequiredInstanceExtensions"))
#endif

#if !defined (NDEBUG)
    constexpr bool enableValidationLayers = true;
#else
    constexpr bool enableValidationLayers = false;
#endif

namespace gfx
{

ext::vector<const char*> getRequiredExtensions(bool enableValidationLayers)
{
    ext::vector<const char*> extensions;

#if defined(GFX_GLFW_ENABLED)
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    if (glfwExtensionCount > 0)
        extensions.insert(extensions.end(), glfwExtensions, glfwExtensions + glfwExtensionCount);
#endif

    if (enableValidationLayers)
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

#if defined(__APPLE__)
    extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
#endif

    return extensions;
}

template<size_t S>
bool hasLayers(const ext::array<const char*, S>& wantedLayers)
{
    std::vector<vk::LayerProperties> availableLayers = vk::enumerateInstanceLayerProperties();

    for (const char* wantedLayer : wantedLayers)
    {
        bool layerFound = false;

        for (const vk::LayerProperties& availableLayer : availableLayers)
        {
            if (strcmp(wantedLayer, availableLayer.layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }

        if (!layerFound)
        {
            return false;
        }
    }

    return true;
}

VulkanInstance::VulkanInstance(const Instance::Descriptor& desc)
{
    VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);
    
    uint32_t instanceVersion = vk::enumerateInstanceVersion();
    if (VK_VERSION_MAJOR(instanceVersion) < 1 || VK_VERSION_MINOR(instanceVersion) < 2) {
        throw ext::runtime_error("instance version is lower than 1.2");
    }

    vk::ApplicationInfo appInfo = {
        .pApplicationName = desc.appName.c_str(),
        .applicationVersion = VK_MAKE_VERSION(desc.appVersion[0],
                                              desc.appVersion[1],
                                              desc.appVersion[2]),
        .pEngineName = desc.engineName.c_str(),
        .engineVersion = VK_MAKE_VERSION(desc.engineVersion[0],
                                         desc.engineVersion[1],
                                         desc.engineVersion[2]),
        .apiVersion = VK_VERSION_MINOR(instanceVersion) >= 3 ? VK_API_VERSION_1_3 : VK_API_VERSION_1_2 
    };

    ext::vector<const char*> extensions = getRequiredExtensions(enableValidationLayers);

    vk::InstanceCreateInfo instanceInfo = { .pApplicationInfo = &appInfo };

    instanceInfo.setPEnabledExtensionNames(extensions);

#if defined(__APPLE__)
    instanceInfo.flags |= vk::InstanceCreateFlags::BitsType::eEnumeratePortabilityKHR;
#endif

    vk::DebugUtilsMessengerCreateInfoEXT debugCreateInfo;
    if constexpr (enableValidationLayers)
    {
        constexpr ext::array<const char*, 1> layers = {"VK_LAYER_KHRONOS_validation"};
        if (hasLayers(layers) == false)
            throw ext::runtime_error("required layer not present");
        instanceInfo.setPEnabledLayerNames(layers);
        debugCreateInfo = {
            .messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
                               vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                               vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
            .messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                           vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
                           vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
            .pfnUserCallback = [](vk::DebugUtilsMessageSeverityFlagBitsEXT, vk::DebugUtilsMessageTypeFlagsEXT,
                                  const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) -> vk::Bool32 {
                ext::cerr << "validation layer: " << pCallbackData->pMessage << ext::endl;
                return VK_FALSE;
            }
        };
        instanceInfo.setPNext(&debugCreateInfo);
    }

    m_vkInstance = vk::createInstance(instanceInfo);

    VULKAN_HPP_DEFAULT_DISPATCHER.init(m_vkInstance);

    if constexpr (enableValidationLayers)
        m_debugMessenger = m_vkInstance.createDebugUtilsMessengerEXT(debugCreateInfo);
}

#if defined(GFX_GLFW_ENABLED)
ext::unique_ptr<Surface> VulkanInstance::createSurface(GLFWwindow* glfwWindow)
{
    return ext::make_unique<VulkanSurface>(m_vkInstance, glfwWindow);
}
#endif

ext::unique_ptr<Device> VulkanInstance::newDevice(const Device::Descriptor& desc)
{
    VulkanDevice::Descriptor vulkandeviceDescriptor = {
        .deviceDescriptor = &desc,
    };

#if defined(__APPLE__)
    vulkandeviceDescriptor.deviceExtensions.push_back("VK_KHR_portability_subset");
#endif
    if (desc.queueCaps.present.empty() == false)
        vulkandeviceDescriptor.deviceExtensions.push_back(vk::KHRSwapchainExtensionName);

    ext::vector<VulkanPhysicalDevice*> physicalDevices = findSuitableDevices(vulkandeviceDescriptor);
    if (physicalDevices.empty())
        throw ext::runtime_error("no suitable device found");

    return ext::make_unique<VulkanDevice>(*physicalDevices.front(), vulkandeviceDescriptor);
}

VulkanInstance::~VulkanInstance()
{
    if constexpr (enableValidationLayers)
        m_vkInstance.destroyDebugUtilsMessengerEXT(m_debugMessenger);
    m_vkInstance.destroy();
}
    
ext::vector<VulkanPhysicalDevice*> VulkanInstance::findSuitableDevices(const VulkanDevice::Descriptor& desc)
{
    auto physicalDevices = m_vkInstance.enumeratePhysicalDevices()
        | ext::views::transform([](auto& d){ return VulkanPhysicalDevice(d); });

    m_physicalDevices = ext::vector<VulkanPhysicalDevice>(physicalDevices.begin(), physicalDevices.end());
    
    auto suitableDevices = m_physicalDevices
        | ext::views::filter([&desc](auto d) { return d.isSuitable(desc); })
        | ext::views::transform([](auto& d) { return &d; });

    return ext::vector<VulkanPhysicalDevice*>(suitableDevices.begin(), suitableDevices.end());
}

} // namespace gfx
