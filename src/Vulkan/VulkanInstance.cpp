/*
 * ---------------------------------------------------
 * VulkanInstance.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/05/28 06:24:05
 * ---------------------------------------------------
 */

#define VULKAN_HPP_NO_CONSTRUCTORS

#include "Graphics/Device.hpp"
#include "Graphics/Instance.hpp"

#include "Vulkan/VulkanInstance.hpp"
#include "Vulkan/VulkanSurface.hpp"
#include "Vulkan/VulkanPhysicalDevice.hpp"
#include "Vulkan/VulkanDevice.hpp"

#include <vulkan/vulkan.hpp>

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
    assert(glfwExtensions);
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

    vk::ApplicationInfo appInfo = {
        .pApplicationName = desc.appName.c_str(),
        .applicationVersion = VK_MAKE_VERSION(desc.appVersion[0],
                                              desc.appVersion[1],
                                              desc.appVersion[2]),
        .pEngineName = desc.engineName.c_str(),
        .engineVersion = VK_MAKE_VERSION(desc.engineVersion[0],
                                         desc.engineVersion[1],
                                         desc.engineVersion[2]),
        .apiVersion = VK_API_VERSION_1_1
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

    if constexpr (enableValidationLayers)
    {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_vkInstance, "vkCreateDebugUtilsMessengerEXT");
        if (func == nullptr || func(m_vkInstance, debugCreateInfo, nullptr, &m_debugMessenger) != VK_SUCCESS)
            throw ext::runtime_error("failed to set up debug messenger!");
    }

    ext::vector<vk::PhysicalDevice> vkPhyDevices = m_vkInstance.enumeratePhysicalDevices();
    for (auto vkPhyDevice : vkPhyDevices)
        m_physicalDevices.push_back(std::make_unique<VulkanPhysicalDevice>(vkPhyDevice));
}

#if defined(GFX_GLFW_ENABLED)
ext::unique_ptr<Surface> VulkanInstance::createSurface(GLFWwindow* glfwWindow)
{
    return ext::make_unique<VulkanSurface>(m_vkInstance, glfwWindow);
}
#endif

const ext::vector<PhysicalDevice*> VulkanInstance::listPhysicalDevices()
{
    ext::vector<PhysicalDevice*> ret;
    for (auto& phyDevice : m_physicalDevices)
        ret.push_back(phyDevice.get());
    return ret;
}

ext::unique_ptr<Device> VulkanInstance::newDevice(const Device::Descriptor& desc, const PhysicalDevice* phyDevice)
{
    const VulkanPhysicalDevice* vkPhyDevice;
    if (phyDevice != nullptr)
    {
        if (phyDevice->isSuitable(desc) == false)
            throw ext::runtime_error("device not suitable");
        vkPhyDevice = dynamic_cast<const VulkanPhysicalDevice*>(phyDevice);
    }
    else
    {
        vkPhyDevice = findSuitableDevice(desc);
    }
    assert(vkPhyDevice);
    return ext::make_unique<VulkanDevice>(*vkPhyDevice, desc);
}

VulkanInstance::~VulkanInstance()
{
    if constexpr (enableValidationLayers)
    {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_vkInstance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr)
            func(m_vkInstance, m_debugMessenger, nullptr);
    }
    m_vkInstance.destroy();
}
    
VulkanPhysicalDevice* VulkanInstance::findSuitableDevice(const Device::Descriptor& desc)
{
    // TODO : better device selection
    ext::vector<PhysicalDevice*> phyDevices = listPhysicalDevices();
    for (auto& phyDevice : phyDevices)
    {
        if (phyDevice->isSuitable(desc) == false)
            continue;
        return dynamic_cast<VulkanPhysicalDevice*>(phyDevice);
    }
    return nullptr;
}

} // namespace gfx
