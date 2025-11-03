/*
 * ---------------------------------------------------
 * Drawable.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/21 15:27:28
 * ---------------------------------------------------
 */

#ifndef DRAWABLE_HPP
#define DRAWABLE_HPP

#include "Graphics/Texture.hpp"

#include <memory>

namespace gfx
{

class Drawable
{
public:
    Drawable(const Drawable&) = delete;
    Drawable(Drawable&&) = delete;

    virtual std::shared_ptr<Texture> texture() const = 0;

    virtual ~Drawable() = default;

protected:
    Drawable() = default;

public:
    Drawable& operator=(const Drawable&) = delete;
    Drawable& operator=(Drawable&&) = delete;
};

} // namespace gfx

#endif // DRAWABLE_HPP
