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
    #if defined (USING_METAL) && !defined (USING_OPENGL)
        struct Shader { utils::String metalFuncName; };
    #elif !defined (USING_METAL) && defined (USING_OPENGL)
        struct Shader { utils::String glslCode; };
    #elif defined (USING_METAL) && defined (USING_OPENGL)
        struct Shader { utils::String metalFuncName; utils::String glslCode; };
    #endif

public:
    static inline void init() { s_sharedInstance = utils::UniquePtr<ShaderLibrary>(new ShaderLibrary); }
    static inline ShaderLibrary& shared() { return *s_sharedInstance; }
    static inline void terminated() { s_sharedInstance.clear(); }

    #ifdef USING_METAL
        inline void setMetalShaderLibPath(const utils::String& libPath) { m_metalShaderLibPath = libPath; }
        inline const utils::String& getMetalShaderLibPath() { return m_metalShaderLibPath; }
    #endif

    #if defined (USING_METAL) && !defined (USING_OPENGL)
        inline void registerShader(const utils::String& name, const utils::String& metalFuncName)                                { m_shaders.insert(name, (Shader){ metalFuncName }); }
    #elif !defined (USING_METAL) && defined (USING_OPENGL)
        inline void registerShader(const utils::String& name, const utils::String& glslCode)                                     { m_shaders.insert(name, (Shader){ glslCode }); }
    #elif defined (USING_METAL) && defined (USING_OPENGL)
        inline void registerShader(const utils::String& name, const utils::String& metalFuncName, const utils::String& glslCode) { m_shaders.insert(name, (Shader){ metalFuncName, glslCode }); }
    #endif

    #ifdef USING_METAL
        inline const utils::String& getMetalShaderFuncName(const utils::String& name) { return m_shaders[name].metalFuncName; }
    #endif
    #ifdef USING_OPENGL
        inline const utils::String& getGlslCode(const utils::String& name) { return m_shaders[name].glslCode; }
    #endif

    ~ShaderLibrary() = default;

private:
    ShaderLibrary() = default;

    static utils::UniquePtr<ShaderLibrary> s_sharedInstance;

    #ifdef USING_METAL
        utils::String m_metalShaderLibPath;
    #endif
    utils::Dictionary<utils::String, Shader> m_shaders;
};

}

#endif // SHADERLIBRARY_HPP