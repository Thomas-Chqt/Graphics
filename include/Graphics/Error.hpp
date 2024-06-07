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

}

#endif // GFX_ERROR_HPP