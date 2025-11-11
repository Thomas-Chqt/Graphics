/*
 * ---------------------------------------------------
 * ParameterBlockLayout.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/11/10 22:02:05
 * ---------------------------------------------------
 */

#ifndef PARAMETERBLOCKLAYOUT_HPP
#define PARAMETERBLOCKLAYOUT_HPP

#include "Graphics/Enums.hpp"

#include <vector>

namespace gfx
{

struct ParameterBlockBinding
{
    BindingType type = BindingType::uniformBuffer;
    BindingUsages usages = BindingUsage::vertexRead | BindingUsage::fragmentRead;
};

class ParameterBlockLayout
{
public:
    struct Descriptor
    {
        std::vector<ParameterBlockBinding> bindings;
    };

public:
    ParameterBlockLayout(const ParameterBlockLayout&) = delete;
    ParameterBlockLayout(ParameterBlockLayout&&) = delete;

    virtual ~ParameterBlockLayout() = default;

protected:
    ParameterBlockLayout() = default;

public:
    ParameterBlockLayout& operator=(const ParameterBlockLayout&) = delete;
    ParameterBlockLayout& operator=(ParameterBlockLayout&&) = delete;
};

} // namespace gfx

#endif // PARAMETERBLOCKLAYOUT_HPP
