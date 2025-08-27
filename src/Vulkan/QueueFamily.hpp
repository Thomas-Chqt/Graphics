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

namespace gfx
{

class VulkanPhysicalDevice;

struct QueueFamily : public vk::QueueFamilyProperties
{
public:
    using vk::QueueFamilyProperties::QueueFamilyProperties;

    QueueFamily() = default;
    QueueFamily(const vk::QueueFamilyProperties&);

    uint32_t index = 0;
    const VulkanPhysicalDevice* device = nullptr;

    bool hasCapabilities(const QueueCapabilities&) const;
};

}

#endif // QUEUEFAMILY_HPP
