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
#include "GraphicAPI/OpenGL/OpenGLSampler.hpp"
#include "GraphicAPI/OpenGL/OpenGLShader.hpp"
#include "GraphicAPI/OpenGL/OpenGLTexture.hpp"
#include "Graphics/Buffer.hpp"
#include "Graphics/Enums.hpp"
#include "Graphics/Error.hpp"
#include "Graphics/FrameBuffer.hpp"
#include "Graphics/GraphicPipeline.hpp"
#include "Graphics/Sampler.hpp"
#include "Graphics/Shader.hpp"
#include "Graphics/Texture.hpp"
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

#define GL_CALL(x) { x; GLenum __err__ = glGetError(); if (__err__ != GL_NO_ERROR) throw OpenGLCallError(__err__); }

namespace gfx
{

OpenGLGraphicAPI::OpenGLGraphicAPI(const utils::SharedPtr<Window>& window) : m_window(window.forceDynamicCast<OpenGLWindow>())
{    
    m_window->makeContextCurrent();
    GLenum err = glewInit();
    assert(err == GLEW_OK);
    GL_CALL(glEnable(GL_DEPTH_TEST))
}

utils::SharedPtr<Shader> OpenGLGraphicAPI::newShader(const Shader::Descriptor& descriptor) const
{
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

utils::SharedPtr<Sampler> OpenGLGraphicAPI::newSampler(const Sampler::Descriptor& descriptor) const
{
    return utils::makeShared<OpenGLSampler>(descriptor).staticCast<Sampler>();
}

utils::SharedPtr<FrameBuffer> OpenGLGraphicAPI::newFrameBuffer(const FrameBuffer::Descriptor& descriptor) const
{
    m_window->makeContextCurrent();
    return utils::makeShared<OpenGLFrameBuffer>(descriptor).staticCast<FrameBuffer>();
}

#ifdef GFX_BUILD_IMGUI
void OpenGLGraphicAPI::initImgui(ImGuiConfigFlags flags)
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

    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0))

    utils::uint32 width = 0;
    utils::uint32 height = 0;
    m_window->getFrameBufferSize(&width, &height);
    GL_CALL(glViewport(0, 0, width, height))

    if (m_nextPassLoadAction == LoadAction::clear)
    {
        GL_CALL(glClearColor(m_nextPassClearColor.r, m_nextPassClearColor.g,  m_nextPassClearColor.b, m_nextPassClearColor.a))
        GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT))
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
    
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer->frameBufferID()))
    GL_CALL(glViewport(0, 0, m_frameBuffer->glColorTexture()->width(), m_frameBuffer->glColorTexture()->height()))

    if (m_nextPassLoadAction == LoadAction::clear)
    {
        GL_CALL(glClearColor(m_nextPassClearColor.r, m_nextPassClearColor.g,  m_nextPassClearColor.b, m_nextPassClearColor.a))
        GL_CALL(glClear(GL_COLOR_BUFFER_BIT))
    }
}

void OpenGLGraphicAPI::useGraphicsPipeline(const utils::SharedPtr<GraphicPipeline>& graphicsPipeline)
{
    m_window->makeContextCurrent();

    m_graphicPipeline = graphicsPipeline.forceDynamicCast<OpenGLGraphicPipeline>();

    GL_CALL(glUseProgram(m_graphicPipeline->shaderProgramID()))
    if (m_graphicPipeline->blendOperation() != BlendOperation::blendingOff)
    {
        GL_CALL(glEnable(GL_BLEND))
        switch (m_graphicPipeline->blendOperation())
        {
        case BlendOperation::srcA_plus_1_minus_srcA:
            GL_CALL(glBlendEquation(GL_FUNC_ADD))
            GL_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA))
            break;
        case BlendOperation::one_minus_srcA_plus_srcA:
            GL_CALL(glBlendEquation(GL_FUNC_ADD))
            GL_CALL(glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA))
            break;
        case BlendOperation::blendingOff:
            UNREACHABLE
        }
    }
    else
        GL_CALL(glDisable(GL_BLEND))
    
    if (m_buffers.contain(0))
        m_graphicPipeline->enableVertexLayout();
}

void OpenGLGraphicAPI::setVertexBuffer(const utils::SharedPtr<Buffer>& buffer, utils::uint64 idx)
{
    m_buffers.get(idx) = buffer.forceDynamicCast<OpenGLBuffer>();
    
    if (idx == 0)
    {
        GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_buffers[idx]->bufferID()))
        
        if (m_graphicPipeline)
            m_graphicPipeline->enableVertexLayout();
    }
    else
    {
        GL_CALL(glBindBufferBase(GL_UNIFORM_BUFFER, (GLuint)idx, m_buffers[idx]->bufferID()));
    }
}

void OpenGLGraphicAPI::setFragmentBuffer(const utils::SharedPtr<Buffer>& buffer, utils::uint64 idx)
{
    GL_CALL(glBindBufferBase(GL_UNIFORM_BUFFER, (GLuint)idx, buffer.forceDynamicCast<OpenGLBuffer>()->bufferID()));
}

void OpenGLGraphicAPI::setFragmentTexture(const utils::SharedPtr<Texture>& texture, utils::uint64 idx)
{
    setFragmentTexture(texture, idx, newSampler(Sampler::Descriptor()), 0);
}

void OpenGLGraphicAPI::setFragmentTexture(const utils::SharedPtr<Texture>& texture, utils::uint64 idx, const utils::SharedPtr<Sampler>& sampler, utils::uint64)
{
    m_textures.get(idx) = texture.forceDynamicCast<OpenGLTexture>();

    GL_CALL(glActiveTexture(GL_TEXTURE0 + m_nextTextureUnit));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, m_textures[idx]->textureID()));

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, sampler.forceDynamicCast<OpenGLSampler>()->minFilter());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, sampler.forceDynamicCast<OpenGLSampler>()->magFilter());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, 	  sampler.forceDynamicCast<OpenGLSampler>()->sAddressMode());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     sampler.forceDynamicCast<OpenGLSampler>()->tAddressMode());

    GL_CALL(glUniform1i((GLint)idx, m_nextTextureUnit));
    m_nextTextureUnit++;
}

void OpenGLGraphicAPI::drawVertices(utils::uint32 start, utils::uint32 count)
{
    m_window->makeContextCurrent();

    GL_CALL(glDrawArrays(GL_TRIANGLES, start, count))
    m_nextTextureUnit = 0;
}

void OpenGLGraphicAPI::drawIndexedVertices(const utils::SharedPtr<Buffer>& buff)
{
    m_window->makeContextCurrent();
    
    m_indexBuffer = buff.forceDynamicCast<OpenGLBuffer>();

    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer->bufferID()))
    GL_CALL(glDrawElements(GL_TRIANGLES, (GLsizei)(m_indexBuffer->size() / sizeof(utils::uint32)), GL_UNSIGNED_INT, nullptr))
    m_nextTextureUnit = 0;
}

void OpenGLGraphicAPI::endRenderPass()
{
    #ifdef GFX_BUILD_IMGUI
    if (m_isImguiRenderPass)
    {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        m_isImguiRenderPass = false;
    }
    #endif

    m_indexBuffer.clear();
    m_textures.clear();
    m_buffers.clear();
    m_graphicPipeline.clear();
    m_frameBuffer.clear();
}

void OpenGLGraphicAPI::endFrame()
{
    #ifdef GFX_BUILD_IMGUI
    if (m_isImguiInit && (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) != 0)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
    #endif

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
