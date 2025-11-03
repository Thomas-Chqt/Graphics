/*
 * ---------------------------------------------------
 * Instance.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/05/27 10:33:53
 * ---------------------------------------------------
 */

#ifndef INSTANCE_HPP
#define INSTANCE_HPP

#include "Graphics/Device.hpp"
#include "Graphics/Surface.hpp"

#include <memory>
#include <string>
#include <array>

#if defined(GFX_GLFW_ENABLED)
    struct GLFWwindow;
#endif

namespace gfx
{

class Instance
{
public:
    struct Descriptor
    {
        std::string appName;
        std::array<int, 3> appVersion;
        std::string engineName;
        std::array<int, 3> engineVersion;
    };

public:
    Instance(const Instance&) = delete;
    Instance(Instance&&) = delete;

    static std::unique_ptr<Instance> newInstance(const Descriptor&);

#if defined(GFX_BUILD_METAL)
    static std::unique_ptr<Instance> newMetalInstance(const Descriptor&);
#endif
#if defined(GFX_BUILD_VULKAN)
    static std::unique_ptr<Instance> newVulkanInstance(const Descriptor&);
#endif

#if defined(GFX_GLFW_ENABLED)
    virtual std::unique_ptr<Surface> createSurface(GLFWwindow*) = 0;
#endif

    virtual std::unique_ptr<Device> newDevice(const Device::Descriptor&) = 0;

    virtual ~Instance() = default;

protected:
    Instance() = default;

public:
    Instance& operator=(const Instance&) = delete;
    Instance& operator=(Instance&&) = delete;
};

} // namespace gfx

#endif // INSTANCE_HPP
