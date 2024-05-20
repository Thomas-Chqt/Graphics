/*
 * ---------------------------------------------------
 * OpenGLGraphicAPI.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/19 14:42:19
 * ---------------------------------------------------
 */

#include "GraphicAPI/OpenGL/OpenGLGraphicAPI.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "Graphics/Window.hpp"
#include <GL/glew.h>
#include <cassert>
#include "Logger/Logger.hpp"

using utils::SharedPtr;

namespace gfx
{

SharedPtr<GraphicAPI> GraphicAPI::newOpenGLGraphicAPI(const SharedPtr<Window>& renderTarget)
{
    return SharedPtr<GraphicAPI>(new OpenGLGraphicAPI(renderTarget));
}

void OpenGLGraphicAPI::setRenderTarget(const utils::SharedPtr<Window>& renderTarget)
{
    m_renderTarget = renderTarget;
    m_renderTarget->useOpenGL();
    GLenum err = glewInit();
    assert(err == GLEW_OK);

    logDebug << "OpenGLGraphicAPI render target set to window " << renderTarget << std::endl;
}

void OpenGLGraphicAPI::setClearColor(float r, float g, float b, float a)
{
    m_clearColor[0] = r;
    m_clearColor[1] = g;
    m_clearColor[2] = b;
    m_clearColor[3] = a;
}

void OpenGLGraphicAPI::beginFrame()
{
    m_renderTarget->makeOpenGlContextCurrent();
    glClearColor(m_clearColor[0], m_clearColor[1],  m_clearColor[2], m_clearColor[3]);
    glClear(GL_COLOR_BUFFER_BIT);
}

void OpenGLGraphicAPI::endFrame()
{
    m_renderTarget->openGLSwapBuffer();
}

OpenGLGraphicAPI::OpenGLGraphicAPI(const utils::SharedPtr<Window>& renderTarget)
{
    if (renderTarget)
        setRenderTarget(renderTarget);
    logDebug << "OpenGLGraphicAPI created" << std::endl;
}

}