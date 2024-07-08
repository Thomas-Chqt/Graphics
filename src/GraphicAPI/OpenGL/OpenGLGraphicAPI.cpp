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
#include "Graphics/Error.hpp"
#include "Graphics/FrameBuffer.hpp"
#include "Graphics/GraphicPipeline.hpp"
#include "Graphics/IndexBuffer.hpp"
#include "Graphics/Texture.hpp"
#include "Graphics/VertexBuffer.hpp"
#include "Math/Vector.hpp"
#include "UtilsCPP/Array.hpp"
#include "UtilsCPP/RuntimeError.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "Graphics/Window.hpp"
#include <GL/glew.h>
#include <cassert>
#include "UtilsCPP/String.hpp"
#include "UtilsCPP/StructLayout.hpp"
#include "UtilsCPP/Types.hpp"
#include "GraphicAPI/OpenGL/OpenGLIndexBuffer.hpp"
#include "UtilsCPP/UniquePtr.hpp"
#ifdef GFX_IMGUI_ENABLED
    #include "imguiBackends/imgui_impl_opengl3.h"
#endif

#define GL_CALL(x) { x; GLenum __err__; if ((__err__ = glGetError()) != GL_NO_ERROR) throw OpenGLCallError(__err__); }

using utils::SharedPtr;
using utils::UniquePtr;
using utils::uint32;
using utils::uint64;
using utils::Array;

namespace gfx
{

OpenGLGraphicAPI::OpenGLGraphicAPI(const utils::SharedPtr<Window>& window)
{
    if (SharedPtr<OpenGLWindow> glWindow = window.dynamicCast<OpenGLWindow>())
        m_window = glWindow;
    else
        throw utils::RuntimeError("Window is not OpenGLWindow");
    
    m_window->makeContextCurrent();
    GLenum err = glewInit();
    assert(err == GLEW_OK);

    GL_CALL(glEnable(GL_DEPTH_TEST));
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

SharedPtr<VertexBuffer> OpenGLGraphicAPI::newVertexBuffer(const void* data, utils::uint64 count, const utils::StructLayout& layout) const
{
    return SharedPtr<VertexBuffer>(new OpenGLVertexBuffer(data, count, layout));
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

SharedPtr<FrameBuffer> OpenGLGraphicAPI::newFrameBuffer(const utils::SharedPtr<Texture>& colorTexture) const
{
    return SharedPtr<FrameBuffer>(new OpenGLFrameBuffer(colorTexture));
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
}

void OpenGLGraphicAPI::beginOnScreenRenderPass()
{
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

void OpenGLGraphicAPI::beginOffScreenRenderPass(const utils::SharedPtr<FrameBuffer>& fBuff)
{
    if (utils::SharedPtr<OpenGLFrameBuffer> glFrameBuffer = fBuff.dynamicCast<OpenGLFrameBuffer>())
    {
        GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, glFrameBuffer->frameBufferID()));
        GL_CALL(glViewport(0, 0, glFrameBuffer->glColorTexture()->width(), glFrameBuffer->glColorTexture()->height()));
    }
    else
        throw utils::RuntimeError("FrameBuffer is not OpenGLFrameBuffer");

    if (m_nextPassLoadAction == LoadAction::clear)
    {
        GL_CALL(glClearColor(m_nextPassClearColor.r, m_nextPassClearColor.g,  m_nextPassClearColor.b, m_nextPassClearColor.a));
        GL_CALL(glClear(GL_COLOR_BUFFER_BIT));
    }
}

void OpenGLGraphicAPI::useGraphicsPipeline(const SharedPtr<GraphicPipeline>& graphicsPipeline)
{
    if (utils::SharedPtr<OpenGLGraphicPipeline> glGraphicsPipeline = graphicsPipeline.dynamicCast<OpenGLGraphicPipeline>())
    {
        GL_CALL(glUseProgram(glGraphicsPipeline->shaderProgramID()));
        m_boundPipeline = graphicsPipeline;
        if (glGraphicsPipeline->blendOperation() != BlendOperation::blendingOff)
        {
            GL_CALL(glEnable(GL_BLEND));
            switch (glGraphicsPipeline->blendOperation())
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
                #if defined(_MSC_VER) && !defined(__clang__) // MSVC
                    __assume(false);
                #else // GCC, Clang
                    __builtin_unreachable();
                #endif
            }
        }
        else
            GL_CALL(glDisable(GL_BLEND));
    }
    else
        throw utils::RuntimeError("GraphicPipeline is not OpenGLGraphicPipeline");
}

void OpenGLGraphicAPI::useVertexBuffer(const utils::SharedPtr<VertexBuffer>& vertexBuffer)
{
    if (utils::SharedPtr<OpenGLVertexBuffer> glVertexBuffer = vertexBuffer.dynamicCast<OpenGLVertexBuffer>())
    {
        GL_CALL(glBindVertexArray(glVertexBuffer->vertexArrayID()));
        m_passObjects.append(UniquePtr<utils::SharedPtrBase>(new SharedPtr<VertexBuffer>(vertexBuffer)));
    }
    else
        throw utils::RuntimeError("VertexBuffer is not OpenGLVertexBuffer");
}

void OpenGLGraphicAPI::setVertexUniform(const utils::String& name, const math::vec4f& vec)
{
    GL_CALL(glUniform4f(m_boundPipeline->findVertexUniformIndex(name), vec.x, vec.y, vec.z, vec.w));
}

void OpenGLGraphicAPI::setVertexUniform(const utils::String& name, const math::mat4x4& mat)
{
    GL_CALL(glUniformMatrix4fv(m_boundPipeline->findVertexUniformIndex(name), 1, GL_FALSE, reinterpret_cast<const GLfloat*>(&mat)));
}

void OpenGLGraphicAPI::setVertexUniform(const utils::String& name, const math::vec2f& vec)
{
    GL_CALL(glUniform2f(m_boundPipeline->findVertexUniformIndex(name), vec.x, vec.y));
}

void OpenGLGraphicAPI::setVertexUniform(const utils::String& name, const math::mat3x3& mat)
{
    float glmat[] = {
        mat[0].x, mat[0].y, mat[0].z,
        mat[1].x, mat[1].y, mat[1].z,
        mat[2].x, mat[2].y, mat[2].z
    };

    GL_CALL(glUniformMatrix3fv(m_boundPipeline->findVertexUniformIndex(name), 1, GL_FALSE, reinterpret_cast<const GLfloat*>(&glmat)));
}

void OpenGLGraphicAPI::setFragmentUniform(const utils::String& name, utils::uint32 val)
{
    GL_CALL(glUniform1ui(m_boundPipeline->findFragmentUniformIndex(name), val));
}

void OpenGLGraphicAPI::setFragmentUniform(const utils::String& name, float f)
{
    GL_CALL(glUniform1f(m_boundPipeline->findFragmentUniformIndex(name), f));
}

void OpenGLGraphicAPI::setFragmentUniform(const utils::String& name, const math::vec3f& vec)
{
    GL_CALL(glUniform3f(m_boundPipeline->findFragmentUniformIndex(name), vec.x, vec.y, vec.z));
}

void OpenGLGraphicAPI::setFragmentUniform(const utils::String& name, const math::vec4f& vec)
{
    GL_CALL(glUniform4f(m_boundPipeline->findFragmentUniformIndex(name), vec.x, vec.y, vec.z, vec.w));
}

void OpenGLGraphicAPI::setFragmentUniform(const utils::String& name, const void* data, const utils::StructLayout& layout)
{
    for (auto element : layout)
    {
        switch (element.typeId)
        {
        case TYPEID_FLOAT:
            setFragmentUniform(name + "." + element.name, *((float*)((char*)data + element.offset)));
            break;
        case TYPEID_VEC2F:
            // setFragmentUniform(pipeline.findFragmentUniformIndex(name + "." + element.name), *((math::vec2f*)data));
            break;
        case TYPEID_VEC3F:
            setFragmentUniform(name + "." + element.name, *((math::vec3f*)((char*)data + element.offset)));
            break;
        case TYPEID_UINT:
            setFragmentUniform(name + "." + element.name, *((utils::uint32*)((char*)data + element.offset)));
            break;
        }
    }
}

void OpenGLGraphicAPI::setFragmentUniform(const utils::String& name, const void* data, utils::uint32 len, const utils::StructLayout& layout)
{
    for (utils::uint32 i = 0; i < len; i++)
        setFragmentUniform(name + "[" + utils::String::fromUInt(i) + "]", ((char*)data) + i * utils::sizeOf(layout), layout);
}

void OpenGLGraphicAPI::setFragmentTexture(const utils::String& name, const utils::SharedPtr<Texture>& texture)
{
    if (SharedPtr<OpenGLTexture> glTexture = texture.dynamicCast<OpenGLTexture>())
    {
        GL_CALL(glActiveTexture(GL_TEXTURE0 + m_nextTextureUnit));
        GL_CALL(glUniform1i(m_boundPipeline->findFragmentUniformIndex(name), m_nextTextureUnit++));
        GL_CALL(glBindTexture(GL_TEXTURE_2D, glTexture->textureID()));
        m_passObjects.append(UniquePtr<utils::SharedPtrBase>(new SharedPtr<Texture>(texture)));
    }
    else
        throw utils::RuntimeError("Texture is not OpenGLTexture");
}

void OpenGLGraphicAPI::drawVertices(uint32 start, uint32 count)
{
    GL_CALL(glDrawArrays(GL_TRIANGLES, start, count));
    m_nextTextureUnit = 0;
}

void OpenGLGraphicAPI::drawIndexedVertices(const utils::SharedPtr<IndexBuffer>& indexBuffer)
{
    if (SharedPtr<OpenGLIndexBuffer> glIndexBuffer = indexBuffer.dynamicCast<OpenGLIndexBuffer>())
    {
        GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glIndexBuffer->indexBufferID()));
        GL_CALL(glDrawElements(GL_TRIANGLES, (int)glIndexBuffer->indexCount(), GL_UNSIGNED_INT, 0));
        m_passObjects.append(UniquePtr<utils::SharedPtrBase>(new SharedPtr<IndexBuffer>(indexBuffer)));
        m_nextTextureUnit = 0;
    }
    else
        throw utils::RuntimeError("IndexBuffer is not OpenGLIndexBuffer");
}

void OpenGLGraphicAPI::endOnScreenRenderPass()
{
    m_boundPipeline.clear();
    m_passObjects.clear();
}

void OpenGLGraphicAPI::endOffScreeRenderPass()
{
    m_boundPipeline.clear();
    m_passObjects.clear();
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

}