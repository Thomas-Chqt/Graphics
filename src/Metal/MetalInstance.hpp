/*
 * ---------------------------------------------------
 * MetalInstance.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/05/27 11:50:59
 * ---------------------------------------------------
 */

#ifndef METALINSTANCE_HPP
#define METALINSTANCE_HPP

#include "Graphics/Instance.hpp"
#include "Graphics/Surface.hpp"
#include "Graphics/PhysicalDevice.hpp"
#include "Graphics/Device.hpp"

#include "Metal/MetalPhysicalDevice.hpp"

#if defined(GFX_USE_UTILSCPP)
    #include "UtilsCPP/memory.hpp"
    namespace ext = utl;
#else
    #include <memory>
    #include <vector>
    namespace ext = std;
#endif

#if defined(GFX_GLFW_ENABLED)
    class GLFWwindow;
#endif

#ifdef __OBJC__
    #import <Metal/Metal.h>
#else
    template<typename T>
    using id = T*;
    class MTLDevice;
#endif // __OBJC__

namespace gfx
{

class MetalInstance : public Instance
{
public:
    MetalInstance() = delete;
    MetalInstance(const MetalInstance&) = delete;
    MetalInstance(MetalInstance&&) = delete;

    MetalInstance(const Instance::Descriptor&);

#if defined(GFX_GLFW_ENABLED)
    ext::unique_ptr<Surface> createSurface(GLFWwindow*) override;
#endif

    const ext::vector<PhysicalDevice*> listPhysicalDevices() override;

    ext::unique_ptr<Device> newDevice(const Device::Descriptor&, const PhysicalDevice* = nullptr) override;

    ~MetalInstance() = default;

private:
    MetalPhysicalDevice* findSuitableDevice(const Device::Descriptor&);

    const ext::string m_appName;
    const int m_appVersion[3];
    const ext::string m_engineName;
    const int m_engineVersion[3];

    ext::vector<ext::unique_ptr<MetalPhysicalDevice>> m_physicalDevices;

public:
    MetalInstance& operator=(const MetalInstance&) = delete;
    MetalInstance& operator=(MetalInstance&&) = delete;
};

} // namespace gfx

#endif // METALINSTANCE_HPP
