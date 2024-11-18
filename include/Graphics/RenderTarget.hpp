/*
 * ---------------------------------------------------
 * RenderTarget.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2024/08/31 14:59:02
 * ---------------------------------------------------
 */

#ifndef RENDERTARGET_HPP
#define RENDERTARGET_HPP

#include "UtilsCPP/Types.hpp"

namespace gfx
{

class RenderTarget
{
public:
    RenderTarget(const RenderTarget&) = delete;
    RenderTarget(RenderTarget&&)      = delete;

    virtual utils::uint32 width() = 0;
    virtual utils::uint32 height() = 0;
    
    virtual ~RenderTarget() = default;

protected:
    RenderTarget() = default;
    
public:
    RenderTarget& operator = (const RenderTarget&) = delete;
    RenderTarget& operator = (RenderTarget&&)      = delete;
};

}

#endif // RENDERTARGET_HPP