/*
 * ---------------------------------------------------
 * OpenGLGraphicAPI.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/19 14:42:19
 * ---------------------------------------------------
 */

#include "GraphicAPI/OpenGL/OpenGLGraphicAPI.hpp"
#include "GraphicAPI/OpenGL/OpenGLBuffer.hpp"
#include "GraphicAPI/OpenGL/OpenGLFrameBuffer.hpp"
#include "GraphicAPI/OpenGL/OpenGLGraphicPipeline.hpp"
#include "GraphicAPI/OpenGL/OpenGLShader.hpp"
#include "GraphicAPI/OpenGL/OpenGLTexture.hpp"
#include "Graphics/Buffer.hpp"
#include "Graphics/Enums.hpp"
#include "Graphics/Error.hpp"
#include "Graphics/FrameBuffer.hpp"
#include "Graphics/GraphicPipeline.hpp"
#include "Graphics/Shader.hpp"
#include "Graphics/Texture.hpp"
#include "Math/Vector.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "Graphics/Window.hpp"
#include <GL/glew.h>
#include <cassert>
#include "UtilsCPP/Types.hpp"
#include "Window/OpenGLWindow.hpp"
#include "UtilsCPP/Macros.hpp"

#ifdef GFX_BUILD_IMGUI
    #include "imgui/imgui.h"
    #include "imguiBackends/imgui_impl_opengl3.h"
#endif

#define GL_CALL(x) { x; GLenum __err__; if ((__err__ = glGetError()) != GL_NO_ERROR) throw OpenGLCallError(__err__); }

namespace gfx
{

OpenGLGraphicAPI::OpenGLGraphicAPI(const utils::SharedPtr<Window>& window) : m_window(window.forceDynamicCast<OpenGLWindow>())
{    
    m_window->makeContextCurrent();
    GLenum err = glewInit();
    assert(err == GLEW_OK);
    GL_CALL(glEnable(GL_DEPTH_TEST));
}

utils::SharedPtr<Shader> OpenGLGraphicAPI::newShader(const Shader::MetalShaderDescriptor& metalShaderDescriptor, const Shader::OpenGLShaderDescriptor& descriptor) const
{
    (void)metalShaderDescriptor;
    m_window->makeContextCurrent();
    return utils::makeShared<OpenGLShader>(descriptor).staticCast<Shader>();
}

utils::SharedPtr<GraphicPipeline> OpenGLGraphicAPI::newGraphicsPipeline(const GraphicPipeline::Descriptor& descriptor) const
{
    m_window->makeContextCurrent();
    return utils::makeShared<OpenGLGraphicPipeline>(descriptor).staticCast<GraphicPipeline>();
}

utils::SharedPtr<Buffer> OpenGLGraphicAPI::newBuffer(const Buffer::Descriptor& descriptor) const
{
    m_window->makeContextCurrent();
    return utils::makeShared<OpenGLBuffer>(descriptor).staticCast<Buffer>();
}

utils::SharedPtr<Texture> OpenGLGraphicAPI::newTexture(const Texture::Descriptor& descriptor) const
{
    m_window->makeContextCurrent();
    return utils::makeShared<OpenGLTexture>(descriptor).staticCast<Texture>();
}

utils::SharedPtr<FrameBuffer> OpenGLGraphicAPI::newFrameBuffer(const utils::SharedPtr<Texture>& colorTexture) const
{
    m_window->makeContextCurrent();
    return utils::makeShared<OpenGLFrameBuffer>(colorTexture).staticCast<FrameBuffer>();
}

#ifdef GFX_BUILD_IMGUI
void OpenGLGraphicAPI::initImGui(ImGuiConfigFlags flags)
{
    ImGui::CreateContext();
    
    ImGui::GetIO().ConfigFlags = flags;

    m_window->imGuiInit();
    #ifdef __APPLE__
        ImGui_ImplOpenGL3_Init("#version 150");
    #else
        ImGui_ImplOpenGL3_Init("#version 130");
    #endif

    m_isImguiInit = true;
}
#endif

void OpenGLGraphicAPI::beginRenderPass()
{
    m_window->makeContextCurrent();

    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));

    utils::uint32 width, height;
    m_window->getFrameBufferSize(&width, &height);
    GL_CALL(glViewport(0, 0, width, height));

    if (m_nextPassLoadAction == LoadAction::clear)
    {
        GL_CALL(glClearColor(m_nextPassClearColor.r, m_nextPassClearColor.g,  m_nextPassClearColor.b, m_nextPassClearColor.a));
        GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    }
}

#ifdef GFX_BUILD_IMGUI
void OpenGLGraphicAPI::beginImguiRenderPass()
{
    beginRenderPass();

    ImGui_ImplOpenGL3_NewFrame();
    m_window->imGuiNewFrame();
    ImGui::NewFrame();

    m_isImguiRenderPass = true;
}
#endif

void OpenGLGraphicAPI::beginRenderPass(const utils::SharedPtr<FrameBuffer>& fBuff)
{
    m_window->makeContextCurrent();

    m_frameBuffer = fBuff.forceDynamicCast<OpenGLFrameBuffer>();
    
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer->frameBufferID()));
    GL_CALL(glViewport(0, 0, m_frameBuffer->glColorTexture()->width(), m_frameBuffer->glColorTexture()->height()));

    if (m_nextPassLoadAction == LoadAction::clear)
    {
        GL_CALL(glClearColor(m_nextPassClearColor.r, m_nextPassClearColor.g,  m_nextPassClearColor.b, m_nextPassClearColor.a));
        GL_CALL(glClear(GL_COLOR_BUFFER_BIT));
    }
}

void OpenGLGraphicAPI::useGraphicsPipeline(const utils::SharedPtr<GraphicPipeline>& graphicsPipeline)
{
    m_window->makeContextCurrent();

    m_graphicPipeline = graphicsPipeline.forceDynamicCast<OpenGLGraphicPipeline>();

    GL_CALL(glUseProgram(m_graphicPipeline->shaderProgramID()));
    if (m_graphicPipeline->blendOperation() != BlendOperation::blendingOff)
    {
        GL_CALL(glEnable(GL_BLEND));
        switch (m_graphicPipeline->blendOperation())
        {
        case BlendOperation::srcA_plus_1_minus_srcA:
            GL_CALL(glBlendEquation(GL_FUNC_ADD));
            GL_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
            break;
        case BlendOperation::one_minus_srcA_plus_srcA:
            GL_CALL(glBlendEquation(GL_FUNC_ADD));
            GL_CALL(glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA));
            break;
        case BlendOperation::blendingOff:
            UNREACHABLE
        }
    }
    else
        GL_CALL(glDisable(GL_BLEND));
    
    if (m_vertextBuffer)
        m_graphicPipeline->enableVertexLayout();
}

void OpenGLGraphicAPI::useVertexBuffer(const utils::SharedPtr<Buffer>& vertexBuffer)
{
    m_window->makeContextCurrent();

    m_vertextBuffer = vertexBuffer.forceDynamicCast<OpenGLBuffer>();

    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_vertextBuffer->bufferID()));
    
    if (m_graphicPipeline)
        m_graphicPipeline->enableVertexLayout();
}

void OpenGLGraphicAPI::drawVertices(utils::uint32 start, utils::uint32 count)
{
    m_window->makeContextCurrent();

    GL_CALL(glDrawArrays(GL_TRIANGLES, start, count));
}

void OpenGLGraphicAPI::drawIndexedVertices(const utils::SharedPtr<Buffer>& buff)
{
    m_window->makeContextCurrent();
    
    m_indexBuffer = buff.forceDynamicCast<OpenGLBuffer>();

    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer->bufferID()));
    GL_CALL(glDrawElements(GL_TRIANGLES, (GLsizei)(m_indexBuffer->size() / sizeof(utils::uint32)), GL_UNSIGNED_INT, 0));
}

void OpenGLGraphicAPI::endRenderPass()
{
    if (m_isImguiRenderPass)
    {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        m_isImguiRenderPass = false;
    }

    m_indexBuffer.clear();
    m_vertextBuffer.clear();
    m_graphicPipeline.clear();
    m_frameBuffer.clear();
}

void OpenGLGraphicAPI::endFrame()
{
    if (m_isImguiInit && (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable))
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }

    m_window->swapBuffer();
}

#ifdef GFX_BUILD_IMGUI
void OpenGLGraphicAPI::terminateImGui()
{
    ImGui_ImplOpenGL3_Shutdown();
    m_window->imGuiShutdown();
    ImGui::DestroyContext();

    m_isImguiInit = false;
}
#endif

}