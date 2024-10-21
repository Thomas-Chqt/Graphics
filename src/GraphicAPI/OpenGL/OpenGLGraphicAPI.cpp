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
#include "UtilsCPP/RuntimeError.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "Graphics/Window.hpp"
#include <glad/glad.h>
#include <cassert>
#include "UtilsCPP/Types.hpp"
#include "Window/OpenGLWindow.hpp"
#include "UtilsCPP/Macros.hpp"

#ifdef GFX_BUILD_IMGUI
    #include <imgui.h>
    #include <imgui_impl_opengl3.h>
#endif

#define GL_CALL(x) { x; GLenum __err__ = glGetError(); if (__err__ != GL_NO_ERROR) throw OpenGLCallError(__err__); }

namespace gfx
{

OpenGLGraphicAPI::OpenGLGraphicAPI(const utils::SharedPtr<Window>& window) : m_window(window.forceDynamicCast<OpenGLWindow>())
{    
    m_window->makeContextCurrent();
    GL_CALL(glEnable(GL_DEPTH_TEST))
    GL_CALL(glDepthFunc(GL_LEQUAL))
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

void OpenGLGraphicAPI::beginRenderPass(const utils::SharedPtr<FrameBuffer>& fBuff)
{
    m_window->makeContextCurrent();

    m_frameBuffer = fBuff.forceDynamicCast<OpenGLFrameBuffer>();

    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer->frameBufferID()))

    GLenum fBuffStatus;
    GL_CALL(fBuffStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER));
    if (fBuffStatus != GL_FRAMEBUFFER_COMPLETE)
        throw utils::RuntimeError("framebuffer not complete");
    
    GL_CALL(glViewport(0, 0, m_frameBuffer->glColorTexture()->width(), m_frameBuffer->glColorTexture()->height()))

    if (m_nextPassLoadAction == LoadAction::clear)
    {
        GL_CALL(glClearColor(m_nextPassClearColor.r, m_nextPassClearColor.g,  m_nextPassClearColor.b, m_nextPassClearColor.a))
        GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT))
    }
}

void OpenGLGraphicAPI::beginRenderPass(const utils::SharedPtr<RenderTarget>& rt)
{
    if (auto win = rt.dynamicCast<Window>())
        beginRenderPass();
    else if (auto fb = rt.dynamicCast<FrameBuffer>())
        beginRenderPass(fb);
    else
        UNREACHABLE;
}

#ifdef GFX_BUILD_IMGUI
void OpenGLGraphicAPI::beginImguiRenderPass()
{
    beginRenderPass();

    ImGui_ImplOpenGL3_NewFrame();
    m_window->imGuiNewFrame();
    ImGui::NewFrame();

    m_isImguiRenderPass = true;
    m_isImguiFrame = true;
}
#endif

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
    m_window->makeContextCurrent();

    if (m_buffers.contain(idx))
        m_buffers[idx] = buffer.forceDynamicCast<OpenGLBuffer>();
    else
        m_buffers.insert(idx, buffer.forceDynamicCast<OpenGLBuffer>());
    
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
    m_window->makeContextCurrent();

    GL_CALL(glBindBufferBase(GL_UNIFORM_BUFFER, (GLuint)idx, buffer.forceDynamicCast<OpenGLBuffer>()->bufferID()));
}

void OpenGLGraphicAPI::setVertexUniform(const math::mat4x4& mat, utils::uint64 idx)
{
    assert(m_graphicPipeline);

    GLfloat glMatrix[4][4];
    glMatrix[0][0] = mat[0][0];
    glMatrix[0][1] = mat[0][1];
    glMatrix[0][2] = mat[0][2];
    glMatrix[0][3] = mat[0][3];

    glMatrix[1][0] = mat[1][0];
    glMatrix[1][1] = mat[1][1];
    glMatrix[1][2] = mat[1][2];
    glMatrix[1][3] = mat[1][3];

    glMatrix[2][0] = mat[2][0];
    glMatrix[2][1] = mat[2][1];
    glMatrix[2][2] = mat[2][2];
    glMatrix[2][3] = mat[2][3];

    glMatrix[3][0] = mat[3][0];
    glMatrix[3][1] = mat[3][1];
    glMatrix[3][2] = mat[3][2];
    glMatrix[3][3] = mat[3][3];

    GL_CALL(glUniformMatrix4fv(idx, 1, GL_FALSE, &glMatrix[0][0]))
}

void OpenGLGraphicAPI::setFragmentTexture(const utils::SharedPtr<Texture>& texture, utils::uint64 idx)
{
    setFragmentTexture(texture, idx, newSampler(Sampler::Descriptor()), 0);
}

void OpenGLGraphicAPI::setFragmentTexture(const utils::SharedPtr<Texture>& texture, utils::uint64 idx, const utils::SharedPtr<Sampler>& sampler, utils::uint64)
{
    m_window->makeContextCurrent();

    utils::SharedPtr<OpenGLTexture> glTexture = texture.forceDynamicCast<OpenGLTexture>();
    utils::SharedPtr<OpenGLSampler> glSampler = sampler.forceDynamicCast<OpenGLSampler>();

    if (m_textures.contain(idx))
        m_textures[idx] = glTexture;
    else
        m_textures.insert(idx, glTexture);

    GL_CALL(glActiveTexture(GL_TEXTURE0 + m_nextTextureUnit));
    GL_CALL(glBindTexture(glTexture->textureType(), glTexture->textureID()));

    GL_CALL(glTexParameteri(glTexture->textureType(), GL_TEXTURE_MIN_FILTER, glSampler->minFilter()));
    GL_CALL(glTexParameteri(glTexture->textureType(), GL_TEXTURE_MAG_FILTER, glSampler->magFilter()));
    GL_CALL(glTexParameteri(glTexture->textureType(), GL_TEXTURE_WRAP_S, 	 glSampler->sAddressMode()));
    GL_CALL(glTexParameteri(glTexture->textureType(), GL_TEXTURE_WRAP_T,     glSampler->tAddressMode()));
    if (glTexture->textureType() == GL_TEXTURE_CUBE_MAP)
        GL_CALL(glTexParameteri(glTexture->textureType(), GL_TEXTURE_WRAP_R, glSampler->rAddressMode()));

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
    if (m_isImguiFrame && (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) != 0)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        m_isImguiFrame = false;
    }
    #endif

    GL_CALL(glFinish());
    m_window->swapBuffer();
}

#ifdef GFX_BUILD_IMGUI
void OpenGLGraphicAPI::terminateImGui()
{
    ImGui_ImplOpenGL3_Shutdown();
    m_window->imGuiShutdown();
    ImGui::DestroyContext();
}
#endif

}
