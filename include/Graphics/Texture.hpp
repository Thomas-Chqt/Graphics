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

namespace gfx
{

class Texture
{
public:
    Texture(const Texture&) = delete;
    Texture(Texture&&)      = delete;

    ~Texture() = default;

protected:
    Texture() = default;

public:
    Texture& operator = (const Texture&) = delete;
    Texture& operator = (Texture&&)      = delete;
};

}

#endif // TEXTURE_HPP