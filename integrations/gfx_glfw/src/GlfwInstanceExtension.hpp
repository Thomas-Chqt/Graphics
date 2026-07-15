#pragma once

#include "Vulkan/VulkanInstance.hpp"

#include <span>
#include <string_view>

namespace gfx::glfw
{

class GlfwInstanceExtension : public InstanceExtension
{
public:
    std::span<const std::string_view> getRequiredVulkanInstanceExtensions() const override;
};

} // namespace gfx::glfw
