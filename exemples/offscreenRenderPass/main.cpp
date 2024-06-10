/*
 * ---------------------------------------------------
 * main.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/10 17:25:59
 * ---------------------------------------------------
 */

#include "Graphics/FrameBuffer.hpp"
#include "Graphics/GraphicAPI.hpp"
#include "Graphics/GraphicPipeline.hpp"
#include "Graphics/KeyCodes.hpp"
#include "Graphics/Platform.hpp"
#include "Graphics/ShaderLibrary.hpp"

#include "UtilsCPP/SharedPtr.hpp"
#include "Vertex.hpp"

int main()
{
    gfx::Platform::init();
    gfx::ShaderLibrary::init();

    #ifdef GFX_METAL_ENABLED
        gfx::ShaderLibrary::shared().setMetalShaderLibPath(MTL_SHADER_LIB);
    #endif

    #if defined (GFX_METAL_ENABLED) && defined (GFX_OPENGL_ENABLED)
        gfx::ShaderLibrary::shared().registerShader("triangle_vs",       "triangle_vs",       utils::String::contentOfFile(OPENGL_SHADER_DIR"/triangle.vs"));
        gfx::ShaderLibrary::shared().registerShader("triangle_fs",       "triangle_fs",       utils::String::contentOfFile(OPENGL_SHADER_DIR"/triangle.fs"));
        gfx::ShaderLibrary::shared().registerShader("texturedSquare_vs", "texturedSquare_vs", utils::String::contentOfFile(OPENGL_SHADER_DIR"/texturedSquare.vs"));
        gfx::ShaderLibrary::shared().registerShader("texturedSquare_fs", "texturedSquare_fs", utils::String::contentOfFile(OPENGL_SHADER_DIR"/texturedSquare.fs"));
    #elif defined (GFX_METAL_ENABLED)
        gfx::ShaderLibrary::shared().registerShader("triangle_vs",       "triangle_vs");
        gfx::ShaderLibrary::shared().registerShader("triangle_fs",       "triangle_fs");
        gfx::ShaderLibrary::shared().registerShader("texturedSquare_vs", "texturedSquare_vs");
        gfx::ShaderLibrary::shared().registerShader("texturedSquare_fs", "texturedSquare_fs");
    #elif defined (GFX_OPENGL_ENABLED)
        gfx::ShaderLibrary::shared().registerShader("triangle_vs",       utils::String::contentOfFile(OPENGL_SHADER_DIR"/triangle.vs"));
        gfx::ShaderLibrary::shared().registerShader("triangle_fs",       utils::String::contentOfFile(OPENGL_SHADER_DIR"/triangle.fs"));
        gfx::ShaderLibrary::shared().registerShader("texturedSquare_vs", utils::String::contentOfFile(OPENGL_SHADER_DIR"/texturedSquare.vs"));
        gfx::ShaderLibrary::shared().registerShader("texturedSquare_fs", utils::String::contentOfFile(OPENGL_SHADER_DIR"/texturedSquare.fs"));
    #endif

    const utils::Array<Vertex> triangle_vertices = {
        { math::vec2f{ -1, -1 }, math::vec2f{ 0, 0 } },
        { math::vec2f{  0,  1 }, math::vec2f{ 0, 0 } },
        { math::vec2f{  1, -1 }, math::vec2f{ 0, 0 } }
    };

    const utils::Array<Vertex> square_vertices = {
        { math::vec2f{ -0.5, -0.5 }, math::vec2f{ 0.0, 1.0 } },
        { math::vec2f{ -0.5,  0.5 }, math::vec2f{ 0.0, 0.0 } },
        { math::vec2f{  0.5,  0.5 }, math::vec2f{ 1.0, 0.0 } },
        { math::vec2f{  0.5, -0.5 }, math::vec2f{ 1.0, 1.0 } }
    };
    const utils::Array<utils::uint32> square_indices = utils::Array<utils::uint32>({ 0, 1, 2, 0, 2, 3 });

    utils::SharedPtr<gfx::Window> window = gfx::Platform::shared().newDefaultWindow(800, 600);
    utils::SharedPtr<gfx::GraphicAPI> graphicAPI = gfx::Platform::shared().newDefaultGraphicAPI(window);

    utils::SharedPtr<gfx::GraphicPipeline> triangleGfxPipeline = graphicAPI->newGraphicsPipeline("triangle_vs", "triangle_fs");
    utils::SharedPtr<gfx::VertexBuffer>    triangleVtxBuffer   = graphicAPI->newVertexBuffer(triangle_vertices);

    utils::SharedPtr<gfx::GraphicPipeline> squareGfxPipeline   = graphicAPI->newGraphicsPipeline("texturedSquare_vs", "texturedSquare_fs");
    utils::SharedPtr<gfx::VertexBuffer>    squareVtxBuffer     = graphicAPI->newVertexBuffer(square_vertices);
    utils::SharedPtr<gfx::IndexBuffer>     squateIdxBuffer     = graphicAPI->newIndexBuffer(square_indices);

    utils::SharedPtr<gfx::FrameBuffer>     offscreenFrameBuf   = graphicAPI->newFrameBuffer(1000, 1000);

    bool running = true;

    window->setEventCallBack([&](gfx::Event& event)
    {
        event.dispatch<gfx::KeyDownEvent>([&](gfx::KeyDownEvent& event)
        {
            switch (event.keyCode())
            {
            case ESC_KEY:
                running = false;
            }
        });
    });

    while (running)
    {
        gfx::Platform::shared().pollEvents();
        
        gfx::RenderPassDescriptor offscreenRenderPassDescriptor;
        offscreenRenderPassDescriptor.clearColor = RED;
        offscreenRenderPassDescriptor.frameBuffer = offscreenFrameBuf;

        graphicAPI->beginFrame(offscreenRenderPassDescriptor);
        
        graphicAPI->useGraphicsPipeline(triangleGfxPipeline);
        graphicAPI->useVertexBuffer(triangleVtxBuffer);
        graphicAPI->drawVertices(0, 3);

        graphicAPI->nextRenderPass();

        graphicAPI->useGraphicsPipeline(squareGfxPipeline);
        graphicAPI->useVertexBuffer(squareVtxBuffer);
        graphicAPI->setFragmentTexture(squareGfxPipeline->findFragmentUniformIndex("u_texture"), offscreenFrameBuf);
        graphicAPI->drawIndexedVertices(squateIdxBuffer);

        graphicAPI->endFrame();
    }

    gfx::ShaderLibrary::terminated();
    gfx::Platform::terminate();
};