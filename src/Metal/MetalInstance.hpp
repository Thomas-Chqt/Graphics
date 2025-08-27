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
#include "Graphics/Device.hpp"

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

    ext::unique_ptr<Device> newDevice(const Device::Descriptor&) override;

    ~MetalInstance() override;

private:
    NSAutoreleasePool* m_autoReleasePool;
    ext::vector<id<MTLDevice>> m_devices;

public:
    MetalInstance& operator=(const MetalInstance&) = delete;
    MetalInstance& operator=(MetalInstance&&) = delete;
};

} // namespace gfx

#endif // METALINSTANCE_HPP
