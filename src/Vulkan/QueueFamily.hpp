/*
 * ---------------------------------------------------
 * QueueFamily.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/13 16:35:34
 * ---------------------------------------------------
 */

#ifndef QUEUEFAMILY_HPP
#define QUEUEFAMILY_HPP

#include "Graphics/QueueCapabilities.hpp"

#include <vulkan/vulkan.hpp>

#if defined(GFX_USE_UTILSCPP)
#else
    #include <cstdint>
#endif

namespace gfx
{

class VulkanPhysicalDevice;

struct QueueFamily : public vk::QueueFamilyProperties
{
public:
    using vk::QueueFamilyProperties::QueueFamilyProperties;

    uint32_t index;
    const VulkanPhysicalDevice* device;

    bool hasCapabilities(const QueueCapabilities&) const;
};

}

#endif // QUEUEFAMILY_HPP
