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

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <memory>
    #include <string>
    namespace ext = std;
#endif

#if defined(GFX_GLFW_ENABLED)
    class GLFWwindow;
#endif

namespace gfx
{

class Instance
{
public:
    struct Descriptor
    {
        ext::string appName;
        int appVersion[3];
        ext::string engineName;
        int engineVersion[3];
    };

public:
    Instance(const Instance&) = delete;
    Instance(Instance&&) = delete;

    static ext::unique_ptr<Instance> newInstance(const Descriptor&);

#if defined(GFX_BUILD_METAL)
    static ext::unique_ptr<Instance> newMetalInstance(const Descriptor&);
#endif
#if defined(GFX_BUILD_VULKAN)
    static ext::unique_ptr<Instance> newVulkanInstance(const Descriptor&);
#endif
    
#if defined(GFX_GLFW_ENABLED)
    virtual ext::unique_ptr<Surface> createSurface(GLFWwindow*) = 0;
#endif

    virtual ext::unique_ptr<Device> newDevice(const Device::Descriptor&) = 0;

    virtual ~Instance() = default;

protected:
    Instance() = default;

public:
    Instance& operator=(const Instance&) = delete;
    Instance& operator=(Instance&&) = delete;
};

} // namespace gfx

#endif // INSTANCE_HPP
