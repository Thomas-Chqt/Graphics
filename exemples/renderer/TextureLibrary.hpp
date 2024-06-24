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

#include "Graphics/GraphicAPI.hpp"
#include "Graphics/Texture.hpp"
#include "UtilsCPP/Dictionary.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "UtilsCPP/String.hpp"

class TextureLibrary
{
public:
    TextureLibrary(const TextureLibrary&) = delete;
    TextureLibrary(TextureLibrary&&)      = delete;

    static inline void init(const utils::SharedPtr<gfx::GraphicAPI>& api) { s_instance = utils::UniquePtr<TextureLibrary>(new TextureLibrary(api)); }
    static inline TextureLibrary& shared() { return *s_instance; }
    static inline void terminate() { s_instance.clear(); }

    const utils::SharedPtr<gfx::Texture>& textureFromFile(const utils::String& filePath);

    ~TextureLibrary() = default;

private:
    TextureLibrary(const utils::SharedPtr<gfx::GraphicAPI>& api);

    static utils::UniquePtr<TextureLibrary> s_instance;

    utils::SharedPtr<gfx::GraphicAPI> m_api;
    utils::Dictionary<utils::String, utils::SharedPtr<gfx::Texture>> m_textures;

public:
    TextureLibrary& operator = (const TextureLibrary&) = delete;
    TextureLibrary& operator = (TextureLibrary&&)      = delete;
};

#endif // TEXTURELIBRARY_HPP