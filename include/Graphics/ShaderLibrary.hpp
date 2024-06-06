/*
 * ---------------------------------------------------
 * ShaderLibrary.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/21 12:40:17
 * ---------------------------------------------------
 */

#ifndef SHADERLIBRARY_HPP
# define SHADERLIBRARY_HPP

#include "UtilsCPP/Dictionary.hpp"
#include "UtilsCPP/UniquePtr.hpp"
#include "UtilsCPP/String.hpp"

namespace gfx
{

class ShaderLibrary
{
private:
    #if defined (GFX_METAL_ENABLED) && !defined (GFX_OPENGL_ENABLED)
        struct Shader { utils::String metalFuncName; };
    #elif !defined (GFX_METAL_ENABLED) && defined (GFX_OPENGL_ENABLED)
        struct Shader { utils::String glslCode; };
    #elif defined (GFX_METAL_ENABLED) && defined (GFX_OPENGL_ENABLED)
        struct Shader { utils::String metalFuncName; utils::String glslCode; };
    #endif

public:
    static inline void init() { s_sharedInstance = utils::UniquePtr<ShaderLibrary>(new ShaderLibrary); }
    static inline ShaderLibrary& shared() { return *s_sharedInstance; }
    static inline void terminated() { s_sharedInstance.clear(); }

    #ifdef GFX_METAL_ENABLED
        inline void setMetalShaderLibPath(const utils::String& libPath) { m_metalShaderLibPath = libPath; }
        inline const utils::String& getMetalShaderLibPath() { return m_metalShaderLibPath; }
    #endif

    #if defined (GFX_METAL_ENABLED) && !defined (GFX_OPENGL_ENABLED)
        inline void registerShader(const utils::String& name, const utils::String& metalFuncName)                                { m_shaders.insert(name, Shader{ metalFuncName }); }
    #elif !defined (GFX_METAL_ENABLED) && defined (GFX_OPENGL_ENABLED)
        inline void registerShader(const utils::String& name, const utils::String& glslCode)                                     { m_shaders.insert(name, Shader{ glslCode }); }
    #elif defined (GFX_METAL_ENABLED) && defined (GFX_OPENGL_ENABLED)
        inline void registerShader(const utils::String& name, const utils::String& metalFuncName, const utils::String& glslCode) { m_shaders.insert(name, Shader{ metalFuncName, glslCode }); }
    #endif

    #ifdef GFX_METAL_ENABLED
        inline const utils::String& getMetalShaderFuncName(const utils::String& name) { return m_shaders[name].metalFuncName; }
    #endif
    #ifdef GFX_OPENGL_ENABLED
        inline const utils::String& getGlslCode(const utils::String& name) { return m_shaders[name].glslCode; }
    #endif

    ~ShaderLibrary() = default;

private:
    ShaderLibrary() = default;

    static utils::UniquePtr<ShaderLibrary> s_sharedInstance;

    #ifdef GFX_METAL_ENABLED
        utils::String m_metalShaderLibPath;
    #endif
    utils::Dictionary<utils::String, Shader> m_shaders;
};

}

#endif // SHADERLIBRARY_HPP