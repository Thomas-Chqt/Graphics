/*
 * ---------------------------------------------------
 * VertexLayout.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/07/08 17:58:35
 * ---------------------------------------------------
 */

#ifndef VERTEXLAYOUT_HPP
# define VERTEXLAYOUT_HPP

#include "Graphics/Enums.hpp"

#include <cstddef>
#include <vector>

namespace gfx
{

struct VertexAttribute
{
    VertexAttributeFormat format;
    size_t offset;
    // bufferIndex
    auto operator<=>(const VertexAttribute&) const = default;
};

struct VertexLayout
{
    size_t stride;
    // stepFunction
    // stepRate
    std::vector<VertexAttribute> attributes;

    auto operator<=>(const VertexLayout&) const = default;
};

}

#endif // VERTEXLAYOUT_HPP
