/*
 * ---------------------------------------------------
 * MetalParameterBlockLayout.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/11/12 08:05:55
 * ---------------------------------------------------
 */

#include "Metal/MetalParameterBlockLayout.hpp"

namespace gfx
{

MetalParameterBlockLayout::MetalParameterBlockLayout(const ParameterBlockLayout::Descriptor& desc)
    : m_bindings(desc.bindings)
{
}

}
