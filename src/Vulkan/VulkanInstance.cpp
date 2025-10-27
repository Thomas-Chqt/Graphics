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

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

namespace
{
#if !defined(NDEBUG)
    constexpr auto kValidationLayers = std::to_array({"VK_LAYER_KHRONOS_validation"});
#endif

    std::vector<const char*> getRequiredExtensions()
    {
        std::vector<const char*> extensions;

#if defined(GFX_GLFW_ENABLED)
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount); // NOLINT
        if (glfwExtensionCount > 0)
            extensions.insert(extensions.end(), glfwExtensions, glfwExtensions + glfwExtensionCount); // NOLINT
#endif

#if !defined(NDEBUG)
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

        // Check if VK_EXT_layer_settings is available before adding it
        std::vector<vk::ExtensionProperties> availableExtensions = vk::enumerateInstanceExtensionProperties();
        bool layerSettingsAvailable = false;
        for (const auto& ext : availableExtensions) {
            if (strcmp(ext.extensionName, "VK_EXT_layer_settings") == 0) {
                layerSettingsAvailable = true;
                break;
            }
        }
        if (layerSettingsAvailable) {
            extensions.push_back("VK_EXT_layer_settings");
        }
#endif

#if defined(__APPLE__)
        extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
#endif

        return extensions;
    }

    template<size_t S>
    std::vector<const char*> getAvailableLayers(const std::array<const char*, S>& wantedLayers)
    {
        std::vector<vk::LayerProperties> availableLayers = vk::enumerateInstanceLayerProperties();
        std::vector<const char*> result;

        for (const char* wantedLayer : wantedLayers) {
            for (const vk::LayerProperties& availableLayer : availableLayers) {
                if (strcmp(wantedLayer, availableLayer.layerName) == 0) {
                    result.push_back(wantedLayer);
                    break;
                }
            }
        }
        return result;
    }
}

namespace gfx
{

VulkanInstance::VulkanInstance(const Instance::Descriptor& desc)
{
    VULKAN_HPP_DEFAULT_DISPATCHER.init();

    uint32_t instanceVersion = vk::enumerateInstanceVersion();
    if (VK_VERSION_MAJOR(instanceVersion) < 1 || VK_VERSION_MINOR(instanceVersion) < 2) {
        throw std::runtime_error("instance version is lower than 1.2");
    }

    auto applicationInfo = vk::ApplicationInfo{}
        .setPApplicationName(desc.appName.c_str())
        .setApplicationVersion(VK_MAKE_VERSION(desc.appVersion[0], desc.appVersion[1], desc.appVersion[2]))
        .setPEngineName(desc.engineName.c_str())
        .setEngineVersion(VK_MAKE_VERSION(desc.engineVersion[0], desc.engineVersion[1], desc.engineVersion[2]))
        .setApiVersion(VK_VERSION_MINOR(instanceVersion) >= 3 ? VK_API_VERSION_1_3 : VK_API_VERSION_1_2);

    std::vector<const char*> extensions = getRequiredExtensions();

    vk::InstanceCreateFlags flags = {};
#if defined(__APPLE__)
    flags |= vk::InstanceCreateFlags::BitsType::eEnumeratePortabilityKHR;
#endif

    auto instanceCreateInfo = vk::InstanceCreateInfo{}
        .setPApplicationInfo(&applicationInfo)
        .setPEnabledExtensionNames(extensions)
        .setFlags(flags);

#if !defined(NDEBUG)
    std::vector<const char*> enabledLayers = getAvailableLayers(kValidationLayers);
    instanceCreateInfo.setPEnabledLayerNames(enabledLayers);

    // Check if VK_EXT_layer_settings extension is available
    bool hasLayerSettingsExt = false;
    for (const char* ext : extensions) {
        if (strcmp(ext, "VK_EXT_layer_settings") == 0) {
            hasLayerSettingsExt = true;
            break;
        }
    }

    auto debugCreateInfo = vk::DebugUtilsMessengerCreateInfoEXT{}
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
            std::println(stderr, "{}", pCallbackData->pMessage);
            return vk::False;
        });

    // Only use layer settings if the extension is available
    auto validateSyncSetting = std::to_array({vk::True});
    const auto layerSettings = std::to_array<vk::LayerSettingEXT>({
        vk::LayerSettingEXT{}
            .setPLayerName("VK_LAYER_KHRONOS_validation")
            .setPSettingName("validate_sync") // enable syncronization validation
            .setType(vk::LayerSettingTypeEXT::eBool32)
            .setValues(validateSyncSetting),
    });
    auto layerSettingsCreateInfo = vk::LayerSettingsCreateInfoEXT{}
        .setSettings(layerSettings);

    if (hasLayerSettingsExt) {
        debugCreateInfo.setPNext(&layerSettingsCreateInfo);
    }

    instanceCreateInfo.setPNext(&debugCreateInfo);
#endif

    m_vkInstance = vk::createInstance(instanceCreateInfo);

    VULKAN_HPP_DEFAULT_DISPATCHER.init(m_vkInstance);

#if !defined(NDEBUG)
    m_debugMessenger = m_vkInstance.createDebugUtilsMessengerEXT(debugCreateInfo);
#endif

    m_physicalDevices = m_vkInstance.enumeratePhysicalDevices()
        | std::views::transform([](auto& d){ return VulkanPhysicalDevice(d); })
        | std::ranges::to<std::vector>();
}

#if defined(GFX_GLFW_ENABLED)
std::unique_ptr<Surface> VulkanInstance::createSurface(GLFWwindow* glfwWindow)
{
    return std::make_unique<VulkanSurface>(m_vkInstance, glfwWindow);
}
#endif

std::unique_ptr<Device> VulkanInstance::newDevice(const Device::Descriptor& desc)
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

    auto suitableDevices = m_physicalDevices | std::views::filter([&](auto d) { return d.isSuitable(vulkandeviceDescriptor); });
    if (suitableDevices.empty())
        throw std::runtime_error("no suitable device found");

    return std::make_unique<VulkanDevice>(this, &suitableDevices.front(), vulkandeviceDescriptor);
}

VulkanInstance::~VulkanInstance()
{
#if !defined(NDEBUG)
    m_vkInstance.destroyDebugUtilsMessengerEXT(m_debugMessenger);
#endif
    m_vkInstance.destroy();
}

} // namespace gfx
