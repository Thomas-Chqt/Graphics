/*
 * ---------------------------------------------------
 * VulkanEnums.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/02 07:26:22
 * ---------------------------------------------------
 */

#ifndef VULKANENUMS_HPP
#define VULKANENUMS_HPP

#include "Graphics/Enums.hpp"
#include <vulkan/vulkan.hpp>

namespace gfx
{

vk::Format toVkFormat(PixelFormat);
PixelFormat toPixelFormat(vk::Format fmt);

vk::ColorSpaceKHR toVkColorSpaceKHR(PixelFormat);

vk::PresentModeKHR toVkPresentModeKHR(PresentMode);
PresentMode toPresentMode(vk::PresentModeKHR pmd);

vk::AttachmentLoadOp toVkAttachmentLoadOp(LoadAction);

}

#endif // VULKANENUMS_HPP
