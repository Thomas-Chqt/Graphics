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

#include <utility>

#include "UtilsCPP/Error.hpp"
#include "UtilsCPP/String.hpp"
#include "UtilsCPP/Types.hpp"

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
    ERR_DESC(m_description)
private:
    utils::String m_description = "fail to initialize GLFW. " + GLFWError::s_lastErrorDesc;
};
#endif // GFX_USING_GLFW

#ifdef GFX_BUILD_METAL
struct MetalError : public GFXError {};

struct RenderCommandEncoderCreationError : public MetalError {
    ERR_DESC("RenderCommandEncoder creation failed")
};

struct MTLLibraryCreationError : public MetalError {
    ERR_DESC("MTLLibrary creation failed")
};

struct MTLFunctionCreationError : public MetalError {
    ERR_DESC("MTLFunction creation failed")
};

struct MTLRenderPipelineStateCreationError : public MetalError {
    ERR_DESC("MTLRenderPipelineState creation failed")
};

struct DepthStencilStateCreationError : public MetalError {
    ERR_DESC("MTLDepthStencilState creation failed")
};

struct MTLBufferCreationError : public MetalError {
    ERR_DESC("MTLBuffer creation failed")
};

struct MTLTextureCreationError : public MetalError {
    ERR_DESC("MTLTexture creation failed")
};
#endif

#ifdef GFX_BUILD_OPENGL
struct OpenGLError : public GFXError {};

struct OpenGLShaderCompileError : public OpenGLError
{
public:
    inline explicit OpenGLShaderCompileError(utils::String desc) : m_description(std::move(desc)) {}
    ERR_DESC(m_description)

private:
    utils::String m_description;
};

struct OpenGLShaderLinkError : public OpenGLError
{
public:
    inline explicit OpenGLShaderLinkError(utils::String desc) : m_description(std::move(desc)) {}
    ERR_DESC(m_description)

private:
    utils::String m_description;
};

struct OpenGLCallError : public OpenGLError
{
public:
    inline explicit OpenGLCallError(utils::uint32 code) : m_code(code)
    {
        switch (m_code)
        {
        default:
            m_description = utils::String::fromUInt(m_code);
        }        
    }
    
    ERR_DESC(m_description)

private:
    utils::String m_description;
    utils::uint32 m_code;
};
#endif

}

#endif // GFX_ERROR_HPP