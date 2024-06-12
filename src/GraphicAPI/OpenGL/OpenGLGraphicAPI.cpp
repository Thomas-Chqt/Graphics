/*
 * ---------------------------------------------------
 * OpenGLGraphicAPI.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/19 14:42:19
 * ---------------------------------------------------
 */

#include "GraphicAPI/OpenGL/OpenGLGraphicAPI.hpp"
#include "GraphicAPI/OpenGL/OpenGLFrameBuffer.hpp"
#include "GraphicAPI/OpenGL/OpenGLGraphicPipeline.hpp"
#include "GraphicAPI/OpenGL/OpenGLTexture.hpp"
#include "GraphicAPI/OpenGL/OpenGLVertexBuffer.hpp"
#include "Graphics/Enums.hpp"
#include "Graphics/FrameBuffer.hpp"
#include "Graphics/GraphicPipeline.hpp"
#include "Graphics/IndexBuffer.hpp"
#include "Graphics/Texture.hpp"
#include "Graphics/VertexBuffer.hpp"
#include "UtilsCPP/Array.hpp"
#include "UtilsCPP/RuntimeError.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "Graphics/Window.hpp"
#include <GL/glew.h>
#include <cassert>
#include "UtilsCPP/String.hpp"
#include "UtilsCPP/Types.hpp"
#include "GraphicAPI/OpenGL/OpenGLIndexBuffer.hpp"
#include "UtilsCPP/UniquePtr.hpp"
#ifdef GFX_IMGUI_ENABLED
    #include "imgui/imgui_impl_opengl3.h"
#endif

using utils::SharedPtr;
using utils::UniquePtr;
using utils::uint32;
using utils::uint64;
using utils::Array;

namespace gfx
{

OpenGLGraphicAPI::OpenGLGraphicAPI(const utils::SharedPtr<Window>& renderTarget)
{
    if (SharedPtr<OpenGLWindow> glWindow = renderTarget.dynamicCast<OpenGLWindow>())
        m_window = glWindow;
    else
        throw utils::RuntimeError("Window is not OpenGLWindow");
    
    m_window->makeContextCurrent();
    GLenum err = glewInit();
    assert(err == GLEW_OK);

    m_screenFrameBuffer = utils::SharedPtr<OpenGLScreenFrameBuffer>(new OpenGLScreenFrameBuffer(m_window));
    m_nextPassTarget = m_screenFrameBuffer.dynamicCast<OpenGLFrameBuffer>();
}

#ifdef GFX_IMGUI_ENABLED
void OpenGLGraphicAPI::useForImGui(ImGuiConfigFlags flags)
{
    assert(s_imguiEnabledAPI == nullptr && "Im gui is already using a graphic api object");
    assert(m_window && "Render target need to be set before initializing imgui");
    
    ImGui::CreateContext();
    
    ImGui::GetIO().ConfigFlags = flags;

    m_window->imGuiInit();
    #ifdef __APPLE__
        ImGui_ImplOpenGL3_Init("#version 150");
    #else
        ImGui_ImplOpenGL3_Init("#version 130");
    #endif
    s_imguiEnabledAPI = this;
}
#endif

SharedPtr<VertexBuffer> OpenGLGraphicAPI::newVertexBuffer(void* data, utils::uint64 count, utils::uint32 vertexSize, const utils::Array<VertexBuffer::LayoutElement>& layout) const
{
    return SharedPtr<VertexBuffer>(new OpenGLVertexBuffer(data, count * vertexSize, layout));
}

SharedPtr<GraphicPipeline> OpenGLGraphicAPI::newGraphicsPipeline(const GraphicPipeline::Descriptor& desc) const
{
    return SharedPtr<GraphicPipeline>(new OpenGLGraphicPipeline(desc));
}

SharedPtr<IndexBuffer> OpenGLGraphicAPI::newIndexBuffer(const Array<uint32>& indices) const
{
    return SharedPtr<IndexBuffer>(new OpenGLIndexBuffer(indices));
}

SharedPtr<Texture> OpenGLGraphicAPI::newTexture(const Texture::Descriptor& desc) const
{
    return SharedPtr<Texture>(new OpenGLTexture(desc));
}

SharedPtr<FrameBuffer> OpenGLGraphicAPI::newFrameBuffer(const FrameBuffer::Descriptor& desc) const
{
    Texture::Descriptor colorTextureDescriptor;
    colorTextureDescriptor.width = desc.width;
    colorTextureDescriptor.height = desc.height;
    colorTextureDescriptor.pixelFormat = desc.pixelFormat;
    return SharedPtr<FrameBuffer>(new OpenGLTextureFrameBuffer(OpenGLTexture(colorTextureDescriptor)));
}

void OpenGLGraphicAPI::setRenderTarget(const utils::SharedPtr<FrameBuffer>& fBuff)
{
    if (utils::SharedPtr<OpenGLFrameBuffer> glFrameBuffer = fBuff.dynamicCast<OpenGLFrameBuffer>())
        m_nextPassTarget = glFrameBuffer;
    else
        throw utils::RuntimeError("FrameBuffer is not OpenGLFrameBuffer");
}

void OpenGLGraphicAPI::beginFrame()
{
    m_window->makeContextCurrent();
    
    #ifdef GFX_IMGUI_ENABLED
    if (s_imguiEnabledAPI == this)
    {
        ImGui_ImplOpenGL3_NewFrame();
        m_window->imGuiNewFrame();
        ImGui::NewFrame();
    }
    #endif

    beginRenderPass();
}

void OpenGLGraphicAPI::useGraphicsPipeline(const SharedPtr<GraphicPipeline>& graphicsPipeline)
{
    if (utils::SharedPtr<OpenGLGraphicPipeline> glGraphicsPipeline = graphicsPipeline.dynamicCast<OpenGLGraphicPipeline>())
    {
        glUseProgram(glGraphicsPipeline->shaderProgramID());
        m_passObjects.append(UniquePtr<utils::SharedPtrBase>(new SharedPtr<GraphicPipeline>(graphicsPipeline)));
    }
    else
        throw utils::RuntimeError("GraphicPipeline is not OpenGLGraphicPipeline");
}

void OpenGLGraphicAPI::useVertexBuffer(const utils::SharedPtr<VertexBuffer>& vertexBuffer)
{
    if (utils::SharedPtr<OpenGLVertexBuffer> glVertexBuffer = vertexBuffer.dynamicCast<OpenGLVertexBuffer>())
    {
        glBindVertexArray(glVertexBuffer->vertexArrayID());
        m_passObjects.append(UniquePtr<utils::SharedPtrBase>(new SharedPtr<VertexBuffer>(vertexBuffer)));
    }
    else
        throw utils::RuntimeError("VertexBuffer is not OpenGLVertexBuffer");
}

void OpenGLGraphicAPI::setVertexUniform(utils::uint32 index, const math::vec4f& vec)
{
    glUniform4f(index, vec.x, vec.y, vec.z, vec.w);
}

void OpenGLGraphicAPI::setVertexUniform(utils::uint32 index, const math::mat4x4& mat)
{
    glUniformMatrix4fv(index, 1, GL_FALSE, reinterpret_cast<const GLfloat*>(&mat));
}

void OpenGLGraphicAPI::setVertexUniform(utils::uint32 index, const math::vec2f& vec)
{
    glUniform2f(index, vec.x, vec.y);
}

void OpenGLGraphicAPI::setVertexUniform(utils::uint32 index, const math::mat3x3& mat)
{
    float glmat[] = {
        mat[0].x, mat[0].y, mat[0].z,
        mat[1].x, mat[1].y, mat[1].z,
        mat[2].x, mat[2].y, mat[2].z
    };

    glUniformMatrix3fv(index, 1, GL_FALSE, reinterpret_cast<const GLfloat*>(&glmat));
}

void OpenGLGraphicAPI::setFragmentUniform(utils::uint32 index, const math::vec4f& vec)
{
    glUniform4f(index, vec.x, vec.y, vec.z, vec.w);
}

void OpenGLGraphicAPI::drawVertices(uint32 start, uint32 count)
{
    glDrawArrays(GL_TRIANGLES, start, count);
}

void OpenGLGraphicAPI::drawIndexedVertices(const utils::SharedPtr<IndexBuffer>& indexBuffer)
{
    if (SharedPtr<OpenGLIndexBuffer> glIndexBuffer = indexBuffer.dynamicCast<OpenGLIndexBuffer>())
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glIndexBuffer->indexBufferID());
        glDrawElements(GL_TRIANGLES, glIndexBuffer->indexCount(), GL_UNSIGNED_INT, 0);
        m_passObjects.append(UniquePtr<utils::SharedPtrBase>(new SharedPtr<IndexBuffer>(indexBuffer)));
    }
    else
        throw utils::RuntimeError("IndexBuffer is not OpenGLIndexBuffer");
}

void OpenGLGraphicAPI::setFragmentTexture(utils::uint32 index, const utils::SharedPtr<Texture>& texture)
{
    if (SharedPtr<OpenGLTexture> glTexture = texture.dynamicCast<OpenGLTexture>())
    {
        glActiveTexture(GL_TEXTURE0 + m_nextTextureUnit);
        glUniform1i(index, m_nextTextureUnit++);
        glBindTexture(GL_TEXTURE_2D, glTexture->textureID());
        m_passObjects.append(UniquePtr<utils::SharedPtrBase>(new SharedPtr<Texture>(texture)));
    }
    else
        throw utils::RuntimeError("Texture is not OpenGLTexture");
}

void OpenGLGraphicAPI::setFragmentTexture(utils::uint32 index, const utils::SharedPtr<FrameBuffer>& fBuff)
{
    if (SharedPtr<OpenGLTextureFrameBuffer> glFrameBuff = fBuff.dynamicCast<OpenGLTextureFrameBuffer>())
    {
        glActiveTexture(GL_TEXTURE0 + m_nextTextureUnit);
        glUniform1i(index, m_nextTextureUnit++);
        glBindTexture(GL_TEXTURE_2D, glFrameBuff->colorTexture().textureID());
        m_passObjects.append(UniquePtr<utils::SharedPtrBase>(new SharedPtr<FrameBuffer>(fBuff)));
    }
    else
        throw utils::RuntimeError("Texture is not OpenGLTexture");

}

void OpenGLGraphicAPI::nextRenderPass()
{
    endRenderPass();
    beginRenderPass();
}

void OpenGLGraphicAPI::endFrame()
{
    #ifdef GFX_IMGUI_ENABLED
    if (s_imguiEnabledAPI == this)
    {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }
    }
    #endif

    m_window->swapBuffer();
}

OpenGLGraphicAPI::~OpenGLGraphicAPI()
{
    #ifdef GFX_IMGUI_ENABLED
    if (s_imguiEnabledAPI == this)
    {
        ImGui_ImplOpenGL3_Shutdown();
        m_window->imGuiShutdown();
        ImGui::DestroyContext();
        s_imguiEnabledAPI = nullptr;
    }
    #endif
}

void OpenGLGraphicAPI::beginRenderPass()
{
    m_nextPassTarget->useForRendering();

    if (m_nextPassLoadAction == LoadAction::clear)
    {
        glClearColor(m_nextPassClearColor.r, m_nextPassClearColor.g,  m_nextPassClearColor.b, m_nextPassClearColor.a);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    m_nextTextureUnit = 0;
}

void OpenGLGraphicAPI::endRenderPass()
{
    m_passObjects.clear();
}

}