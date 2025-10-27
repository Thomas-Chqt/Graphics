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

#if defined(GFX_USE_UTILSCPP)
    namespace ext = utl;
#else
    #include <cstddef>
    #include <vector>
    namespace ext = std;
#endif

namespace gfx
{

struct VertexAttribute
{
    VertexAttributeFormat format;
    size_t offset;
    // bufferIndex
};

struct VertexLayout
{
    size_t stride;
    // stepFunction
    // stepRate
    std::vector<VertexAttribute> attributes;
};

}

#endif // VERTEXLAYOUT_HPP
