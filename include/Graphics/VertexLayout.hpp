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

#include "UtilsCPP/Array.hpp"
#include "UtilsCPP/Types.hpp"
#include "Enums.hpp"

namespace gfx
{

struct VertexAttribute
{
    VertexAttributeFormat format;
    utils::uint64 offset;
};

struct VertexLayout
{
    utils::uint32 stride;
    utils::Array<VertexAttribute> attributes;
};

}

#endif // VERTEXLAYOUT_HPP