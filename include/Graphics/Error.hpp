/*
 * ---------------------------------------------------
 * Error.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/04 17:00:13
 * ---------------------------------------------------
 */

#ifndef GFX_ERROR_HPP
# define GFX_ERROR_HPP

#include "UtilsCPP/Error.hpp"
#include "UtilsCPP/String.hpp"

#define ERR_DESC inline const char* description() const override

namespace gfx
{

struct GFXError : public utils::Error {};

#ifdef GFX_USING_GLFW
struct GLFWError : public GFXError
{
    static int s_lastErrorCode;
    static utils::String s_lastErrorDesc;
};
struct GLFWInitError : public GFXError
{
    ERR_DESC { return m_description; };
private:
    utils::String m_description = "fail to initialize GLFW. " + GLFWError::s_lastErrorDesc;
};
#endif // GLFW_ENABLE

#ifdef GFX_METAL_ENABLED
struct MetalError : public GFXError {};

struct RenderCommandEncoderCreationError : public MetalError {
    ERR_DESC { return "RenderCommandEncoder creation failed"; }
};

struct MTLLibraryCreationError : public MetalError {
    ERR_DESC { return "MTLLibrary creation failed"; }
};

struct MTLFunctionCreationError : public MetalError {
    ERR_DESC { return "MTLFunction creation failed"; }
};

struct MTLRenderPipelineStateCreationError : public MetalError {
    ERR_DESC { return "MTLRenderPipelineState creation failed"; }
};

struct MetalShaderLibNotInitError : public MetalError {
    ERR_DESC { return "Metal shader library is not initialized"; }
};
#endif

#ifdef GFX_OPENGL_ENABLED
struct OpenGLError : public GFXError {};

struct OpenGLShaderCompileError : public OpenGLError
{
public:
    inline OpenGLShaderCompileError(const utils::String& desc) : m_description(desc) {}
    ERR_DESC { return m_description; }

private:
    utils::String m_description;
};

struct OpenGLShaderLinkError : public OpenGLError
{
public:
    inline OpenGLShaderLinkError(const utils::String& desc) : m_description(desc) {}
    ERR_DESC { return m_description; }

private:
    utils::String m_description;
};

#endif

}

#endif // GFX_ERROR_HPP