/*
 * ---------------------------------------------------
 * VulkanInstance.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/05/27 15:04:36
 * ---------------------------------------------------
 */

#ifndef VULKANINSTANCE_HPP
#define VULKANINSTANCE_HPP

#include "Graphics/Instance.hpp"
#include <vulkan/vulkan.hpp>

#if defined(GFX_USE_UTILSCPP)
    #include "UtilsCPP/memory.hpp"
    namespace ext = utl;
#else
    #include <memory>
    namespace ext = std;
#endif

namespace gfx
{

class VulkanInstance : public Instance
{
public:
    VulkanInstance() = delete;
    VulkanInstance(const VulkanInstance&) = delete;
    VulkanInstance(VulkanInstance&&) = delete;

    VulkanInstance(const Instance::Descriptor&);

    const ext::vector<Device::Info> listAvailableDevices() override;
    ext::unique_ptr<Device> newDevice(const Device::Descriptor&) override;

    ~VulkanInstance();

private:
    vk::Instance m_vkInstance;

public:
    VulkanInstance& operator=(const VulkanInstance&) = delete;
    VulkanInstance& operator=(VulkanInstance&&) = delete;
};

} // namespace gfx

#endif // VULKANINSTANCE_HPP
