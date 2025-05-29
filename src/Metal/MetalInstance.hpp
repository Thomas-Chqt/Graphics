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

#if defined(GFX_USE_UTILSCPP)
    #include "UtilsCPP/memory.hpp"
    namespace ext = utl;
#else
    #include <memory>
    #include <vector>
    namespace ext = std;
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

    const ext::vector<Device::Info> listAvailableDevices() override;

    ext::unique_ptr<Device> newDevice(const Device::Descriptor&) override;

    ~MetalInstance() = default;

private:
    const ext::string m_appName;
    const int m_appVersion[3];
    const ext::string m_engineName;
    const int m_engineVersion[3];

    ext::vector<id<MTLDevice>> m_mtlDevices;

public:
    MetalInstance& operator=(const MetalInstance&) = delete;
    MetalInstance& operator=(MetalInstance&&) = delete;
};

} // namespace gfx

#endif // METALINSTANCE_HPP
