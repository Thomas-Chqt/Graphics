/*
 * ---------------------------------------------------
 * MetalParameterBlockLayout.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/11/12 08:04:06
 * ---------------------------------------------------
 */

#ifndef METALPARAMETERBLOCKLAYOUT_HPP
#define METALPARAMETERBLOCKLAYOUT_HPP

#include "Graphics/ParameterBlockLayout.hpp"

namespace gfx
{

class MetalParameterBlockLayout : public ParameterBlockLayout
{
public:
    MetalParameterBlockLayout() = delete;
    MetalParameterBlockLayout(const MetalParameterBlockLayout&) = delete;
    MetalParameterBlockLayout(MetalParameterBlockLayout&&) = delete;

    MetalParameterBlockLayout(const ParameterBlockLayout::Descriptor&);

    inline const std::vector<ParameterBlockBinding>& bindings() const override { return m_bindings; };

    ~MetalParameterBlockLayout() override = default;

private:
    std::vector<ParameterBlockBinding> m_bindings;

public:
    MetalParameterBlockLayout& operator=(const MetalParameterBlockLayout&) = delete;
    MetalParameterBlockLayout& operator=(MetalParameterBlockLayout&&) = delete;
};

}

#endif // METALPARAMETERBLOCKLAYOUT_HPP
