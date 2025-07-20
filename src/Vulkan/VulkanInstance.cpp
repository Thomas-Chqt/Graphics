/*
 * ---------------------------------------------------
 * VulkanInstance.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/05/28 06:24:05
 * ---------------------------------------------------
 */

#include "Graphics/Device.hpp"
#include "Graphics/Instance.hpp"

#include "Vulkan/VulkanInstance.hpp"
#include "Vulkan/VulkanSurface.hpp"
#include "Vulkan/VulkanPhysicalDevice.hpp"
#include "Vulkan/VulkanDevice.hpp"

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_structs.hpp>

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
    struct GLFWwindow;
    #define glfwGetRequiredInstanceExtensions ((const char** (*)(uint32_t* count))::getSym(DL_DEFAULT, "glfwGetRequiredInstanceExtensions"))
#endif

namespace gfx
{

ext::vector<const char*> getRequiredExtensions()
{
    ext::vector<const char*> extensions;

#if defined(GFX_GLFW_ENABLED)
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    if (glfwExtensionCount > 0)
        extensions.insert(extensions.end(), glfwExtensions, glfwExtensions + glfwExtensionCount);
#endif

#if !defined (NDEBUG)
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    extensions.push_back("VK_EXT_layer_settings");
#endif

#if defined(__APPLE__)
    extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
#endif

    return extensions;
}

template<size_t S>
bool hasLayers(const ext::array<const char*, S>& wantedLayers)
{
    std::vector<vk::LayerProperties> availableLayers = vk::enumerateInstanceLayerProperties();
    for (const char* wantedLayer : wantedLayers) {
        bool layerFound = false;
        for (const vk::LayerProperties& availableLayer : availableLayers) {
            if (strcmp(wantedLayer, availableLayer.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound)
            return false;
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

    auto applicationInfo = vk::ApplicationInfo{}
        .setPApplicationName(desc.appName.c_str())
        .setApplicationVersion(VK_MAKE_VERSION(desc.appVersion[0], desc.appVersion[1], desc.appVersion[2]))
        .setPEngineName(desc.engineName.c_str())
        .setEngineVersion(VK_MAKE_VERSION(desc.engineVersion[0], desc.engineVersion[1], desc.engineVersion[2]))
        .setApiVersion(VK_VERSION_MINOR(instanceVersion) >= 3 ? VK_API_VERSION_1_3 : VK_API_VERSION_1_2);

    ext::vector<const char*> extensions = getRequiredExtensions();

    vk::InstanceCreateFlags flags = {};
#if defined(__APPLE__)
    flags |= vk::InstanceCreateFlags::BitsType::eEnumeratePortabilityKHR;
#endif

    auto instanceCreateInfo = vk::InstanceCreateInfo{}
        .setPApplicationInfo(&applicationInfo)
        .setPEnabledExtensionNames(extensions)
        .setFlags(flags);

#if !defined (NDEBUG)
    constexpr auto layers =  ext::to_array({"VK_LAYER_KHRONOS_validation"});
    if (hasLayers(layers) == false)
        throw ext::runtime_error("required layer not present");
    instanceCreateInfo.setPEnabledLayerNames(layers);
    
    auto validateSyncSetting = ext::to_array({vk::True});

    const auto layerSettings = ext::to_array<vk::LayerSettingEXT>({
        vk::LayerSettingEXT{}
            .setPLayerName("VK_LAYER_KHRONOS_validation")
            .setPSettingName("validate_sync") // enable syncronization validation
            .setType(vk::LayerSettingTypeEXT::eBool32)
            .setValues(validateSyncSetting),
    });

    auto layerSettingsCreateInfo = vk::LayerSettingsCreateInfoEXT{}
        .setSettings(layerSettings);

    auto debugCreateInfo = vk::DebugUtilsMessengerCreateInfoEXT{}
        .setPNext(&layerSettingsCreateInfo)
        .setMessageSeverity(
            //vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
            //vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eError
        )
        .setMessageType(
            vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
            vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
            vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance
        )
        .setPfnUserCallback([](vk::DebugUtilsMessageSeverityFlagBitsEXT, vk::DebugUtilsMessageTypeFlagsEXT, const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData, void*) -> vk::Bool32 {
            ext::cerr << pCallbackData->pMessage << ext::endl;
            return vk::False;
        });

    instanceCreateInfo.setPNext(&debugCreateInfo);
#endif

    m_vkInstance = vk::createInstance(instanceCreateInfo);

    VULKAN_HPP_DEFAULT_DISPATCHER.init(m_vkInstance);

#if !defined (NDEBUG)
    m_debugMessenger = m_vkInstance.createDebugUtilsMessengerEXT(debugCreateInfo);
#endif

    m_physicalDevices = m_vkInstance.enumeratePhysicalDevices()
        | ext::views::transform([](auto& d){ return VulkanPhysicalDevice(d); })
        | ext::ranges::to<ext::vector>();
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
        .deviceExtensions = {
            VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
            vk::KHRSynchronization2ExtensionName,
        }
    };

#if defined(__APPLE__)
    vulkandeviceDescriptor.deviceExtensions.push_back("VK_KHR_portability_subset");
#endif

    if (desc.queueCaps.present.empty() == false)
        vulkandeviceDescriptor.deviceExtensions.push_back(vk::KHRSwapchainExtensionName);

    auto suitableDevices = m_physicalDevices | ext::views::filter([&](auto d) { return d.isSuitable(vulkandeviceDescriptor); });
    if (suitableDevices.empty())
        throw ext::runtime_error("no suitable device found");

    return ext::make_unique<VulkanDevice>(&suitableDevices.front(), vulkandeviceDescriptor);
}

VulkanInstance::~VulkanInstance()
{
#if !defined (NDEBUG)
    m_vkInstance.destroyDebugUtilsMessengerEXT(m_debugMessenger);
#endif
    m_vkInstance.destroy();
}

} // namespace gfx
