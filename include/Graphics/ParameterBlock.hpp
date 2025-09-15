/*
 * ---------------------------------------------------
 * ParameterBlock.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/08/10 10:02:57
 * ---------------------------------------------------
 */

#ifndef PARAMETERBLOCK_HPP
#define PARAMETERBLOCK_HPP

#include "Graphics/Enums.hpp"
#include "Graphics/Buffer.hpp"

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <vector>
    #include <cstdint>
    #include <memory>
    namespace ext = std;
#endif

namespace gfx
{

class ParameterBlock
{
public:
    struct Binding
    {
        BindingType type = BindingType::uniformBuffer;
        BindingUsages usages = BindingUsage::vertexRead | BindingUsage::fragmentRead;

        bool operator<(const Binding& rhs) const noexcept { return type != rhs.type ? type < rhs.type : usages < rhs.usages; }
    };

    struct Layout
    {
        ext::vector<ParameterBlock::Binding> bindings;

        bool operator<(const Layout& rhs) const noexcept { return bindings < rhs.bindings; }
    };

public:
    ParameterBlock(const ParameterBlock&) = delete;

    virtual void setBinding(uint32_t idx, const ext::shared_ptr<Buffer>&) = 0;

    virtual ~ParameterBlock() = default;

protected:
    ParameterBlock(ParameterBlock&&) = default;
    ParameterBlock() = default;

public:
    ParameterBlock& operator=(const ParameterBlock&) = delete;

protected:
    ParameterBlock& operator=(ParameterBlock&&) = default;
};

}

#endif // PARAMETERBLOCK_HPP
