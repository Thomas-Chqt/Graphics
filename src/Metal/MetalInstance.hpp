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
namespace ext = std;
#endif

namespace gfx
{

class MetalInstance : public Instance
{
public:
    MetalInstance() = delete;
    MetalInstance(const MetalInstance&) = delete;
    MetalInstance(MetalInstance&&) = delete;

    MetalInstance(const Instance::Descriptor&);

    ext::unique_ptr<Device> newDevice(const Device::Descriptor&) override;

    ~MetalInstance() = default;

private:

public:
    MetalInstance& operator=(const MetalInstance&) = delete;
    MetalInstance& operator=(MetalInstance&&) = delete;
};

} // namespace gfx

#endif // METALINSTANCE_HPP
