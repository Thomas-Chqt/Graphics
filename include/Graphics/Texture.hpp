/*
 * ---------------------------------------------------
 * Texture.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/22 14:01:14
 * ---------------------------------------------------
 */

#ifndef TEXTURE_HPP
# define TEXTURE_HPP

#include "Enums.hpp"
#include "UtilsCPP/Types.hpp"

namespace gfx
{

class Texture
{
public:
    struct Descriptor
    {
    public:
        Descriptor() = default;
        inline Descriptor(utils::uint32 w, utils::uint32 h) : width(w), height(h) {}
    public:
        utils::uint32 width;
        utils::uint32 height;
        gfx::ColorPixelFormat pixelFormat = ColorPixelFormat::RGBA;
    };

public:
    Texture(const Texture&) = delete;
    Texture(Texture&&)      = delete;

    virtual utils::uint32 width() = 0;
    virtual utils::uint32 height() = 0;

    virtual void replaceRegion(utils::uint32 offsetX, utils::uint32 offsetY, utils::uint32 width, utils::uint32 height, const void* bytes) = 0;
    inline void setBytes(const void* ptr) { replaceRegion(0, 0, width(), height(), ptr); }

    virtual ~Texture() = default;

protected:
    Texture() = default;

public:
    Texture& operator = (const Texture&) = delete;
    Texture& operator = (Texture&&)      = delete;
};

}

#endif // TEXTURE_HPP