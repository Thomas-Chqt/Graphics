/*
 * ---------------------------------------------------
 * TextureLibrary.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/24 16:25:35
 * ---------------------------------------------------
 */

#ifndef TEXTURELIBRARY_HPP
# define TEXTURELIBRARY_HPP

#include "Graphics/Texture.hpp"
#include "UtilsCPP/Dictionary.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "UtilsCPP/String.hpp"

class TextureLibrary
{
public:
    TextureLibrary(const TextureLibrary&) = delete;
    TextureLibrary(TextureLibrary&&)      = delete;

    static inline void init() { s_instance = utils::UniquePtr<TextureLibrary>(new TextureLibrary()); }
    static inline TextureLibrary& shared() { return *s_instance; }
    static inline void terminate() { s_instance.clear(); }

    inline utils::SharedPtr<gfx::Texture>& newTexture(const utils::String& name) { m_textures.insert(name, utils::SharedPtr<gfx::Texture>()); return m_textures[name]; }
    inline bool contain(const utils::String& name) { return m_textures.contain(name); }
    inline utils::SharedPtr<gfx::Texture>& getTexture(const utils::String& name) { return m_textures[name]; }

    ~TextureLibrary() = default;

private:
    TextureLibrary() = default;

    static utils::UniquePtr<TextureLibrary> s_instance;

    utils::Dictionary<utils::String, utils::SharedPtr<gfx::Texture>> m_textures;

public:
    TextureLibrary& operator = (const TextureLibrary&) = delete;
    TextureLibrary& operator = (TextureLibrary&&)      = delete;
};

#endif // TEXTURELIBRARY_HPP