/*
 * ---------------------------------------------------
 * OpenGLGraphicAPI.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/19 14:42:19
 * ---------------------------------------------------
 */

#include "GraphicAPI/OpenGL/OpenGLGraphicAPI.hpp"
#include "GraphicAPI/OpenGL/OpenGLGraphicPipeline.hpp"
#include "GraphicAPI/OpenGL/OpenGLVertexBuffer.hpp"
#include "Graphics/GraphicPipeline.hpp"
#include "Graphics/IndexBuffer.hpp"
#include "Graphics/VertexBuffer.hpp"
#include "UtilsCPP/Array.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "Graphics/Window.hpp"
#include <GL/glew.h>
#include <cassert>
#include "Logger/Logger.hpp"
#include "UtilsCPP/String.hpp"
#include "UtilsCPP/Types.hpp"
#include "GraphicAPI/OpenGL/OpenGLIndexBuffer.hpp"

#ifdef IMGUI_ENABLED
    class ImDrawData;
    class ImGuiContext;
    class ImFontAtlas;

    bool ImGui_ImplOpenGL3_Init(const char* glsl_version = nullptr);
    void ImGui_ImplOpenGL3_Shutdown();
    void ImGui_ImplOpenGL3_NewFrame();
    void ImGui_ImplOpenGL3_RenderDrawData(ImDrawData* draw_data);

    namespace ImGui
    {
        ImDrawData* GetDrawData();
        ImGuiContext* CreateContext(ImFontAtlas* shared_font_atlas = NULL);
        void DestroyContext(ImGuiContext* ctx = NULL);
    }
#endif

using utils::SharedPtr;
using utils::String;
using utils::uint32;
using utils::uint64;
using utils::Array;

namespace gfx
{

SharedPtr<GraphicAPI> Platform::newOpenGLGraphicAPI(const SharedPtr<Window>& renderTarget)
{
    return SharedPtr<GraphicAPI>(new OpenGLGraphicAPI(renderTarget));
}

void OpenGLGraphicAPI::setRenderTarget(const utils::SharedPtr<Window>& renderTarget)
{
    if (SharedPtr<OpenGLWindow> glWindow = renderTarget.dynamicCast<OpenGLWindow>())
        m_renderTarget = glWindow;
    else
        logFatal << "Window is not OpenGLWindow" << std::endl;
    
    m_renderTarget->makeContextCurrent();
    GLenum err = glewInit();
    assert(err == GLEW_OK);

    logDebug << "OpenGLGraphicAPI render target set to window " << renderTarget << std::endl;
}

#ifdef IMGUI_ENABLED
void OpenGLGraphicAPI::useForImGui(const utils::Func<void()>& f)
{
    assert(s_imguiEnabledAPI == nullptr && "Im gui is already using a graphic api object");
    assert(m_renderTarget && "Render target need to be set before initializing imgui");
    
    ImGui::CreateContext();
    if (f) f();

    m_renderTarget->imGuiInit();
    #ifdef __APPLE__
        ImGui_ImplOpenGL3_Init("#version 150");
    #else
        ImGui_ImplOpenGL3_Init("#version 130");
    #endif
    s_imguiEnabledAPI = this;
}
#endif

void OpenGLGraphicAPI::setClearColor(float r, float g, float b, float a)
{
    m_clearColor[0] = r;
    m_clearColor[1] = g;
    m_clearColor[2] = b;
    m_clearColor[3] = a;
}

SharedPtr<VertexBuffer> OpenGLGraphicAPI::newVertexBuffer(void* data, uint64 size, const VertexBuffer::LayoutBase& layout)
{
    return SharedPtr<VertexBuffer>(new OpenGLVertexBuffer(data, size, layout));
}

SharedPtr<GraphicPipeline> OpenGLGraphicAPI::newGraphicsPipeline(const String& vertexShaderName, const String& fragmentShaderName)
{
    return SharedPtr<GraphicPipeline>(new OpenGLGraphicPipeline(vertexShaderName, fragmentShaderName));
}

SharedPtr<IndexBuffer> OpenGLGraphicAPI::newIndexBuffer(const Array<uint32>& indices)
{
    return SharedPtr<IndexBuffer>(new OpenGLIndexBuffer(indices));
}

void OpenGLGraphicAPI::beginFrame()
{
    m_renderTarget->makeContextCurrent();
    glClearColor(m_clearColor[0], m_clearColor[1],  m_clearColor[2], m_clearColor[3]);
    glClear(GL_COLOR_BUFFER_BIT);

    #ifdef IMGUI_ENABLED
    if (s_imguiEnabledAPI == this)
    {
        ImGui_ImplOpenGL3_NewFrame();
        m_renderTarget->imGuiNewFrame();
    }
    #endif
}

void OpenGLGraphicAPI::useGraphicsPipeline(const utils::SharedPtr<GraphicPipeline>& graphicsPipeline)
{
    if (utils::SharedPtr<OpenGLGraphicPipeline> glGraphicsPipeline = graphicsPipeline.dynamicCast<OpenGLGraphicPipeline>())
        glUseProgram(glGraphicsPipeline->shaderProgramID());
    else
        logFatal << "GraphicPipeline is not OpenGLGraphicPipeline" << std::endl;
}

void OpenGLGraphicAPI::useVertexBuffer(const utils::SharedPtr<VertexBuffer>& vertexBuffer)
{
    if (utils::SharedPtr<OpenGLVertexBuffer> glVertexBuffer = vertexBuffer.dynamicCast<OpenGLVertexBuffer>())
        glBindVertexArray(glVertexBuffer->vertexArrayID());
    else
        logFatal << "VertexBuffer is not OpenGLVertexBuffer" << std::endl;
}

void OpenGLGraphicAPI::setFragmentUniform(utils::uint32 index, float r, float g, float b, float a)
{
    glUniform4f(index, r, g, b, a);
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
    }
    else
        logFatal << "IndexBuffer is not OpenGLIndexBuffer" << std::endl;
}

void OpenGLGraphicAPI::endFrame()
{
    #ifdef IMGUI_ENABLED
    if (s_imguiEnabledAPI == this)
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    #endif

    m_renderTarget->swapBuffer();
}

OpenGLGraphicAPI::~OpenGLGraphicAPI()
{
    #ifdef IMGUI_ENABLED
    if (s_imguiEnabledAPI == this)
    {
        ImGui_ImplOpenGL3_Shutdown();
        m_renderTarget->imGuiShutdown();
        ImGui::DestroyContext();
        s_imguiEnabledAPI = nullptr;
    }
    #endif

    logDebug << "OpenGLGraphicAPI destructed" << std::endl;
}

OpenGLGraphicAPI::OpenGLGraphicAPI(const utils::SharedPtr<Window>& renderTarget)
{
    if (renderTarget)
        setRenderTarget(renderTarget);
    logDebug << "OpenGLGraphicAPI created" << std::endl;
}

}