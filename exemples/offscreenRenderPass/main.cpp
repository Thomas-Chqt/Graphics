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
#include "Graphics/Texture.hpp"
#include "Math/Vector.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "Vertex.hpp"
#ifdef GFX_IMGUI_ENABLED
#include "imgui/imgui.h"
#endif

template<>
gfx::StructLayout gfx::getLayout<Vertex>()
{
    return {
        { 2, Type::FLOAT, (void*)0 },
        { 2, Type::FLOAT, (void*)offsetof(Vertex, uv) },
    };
}

gfx::GraphicPipeline::Descriptor makeGfxPipelineDescriptor(const utils::String& shaderName)
{
    gfx::GraphicPipeline::Descriptor graphicPipelineDescriptor;
    #ifdef GFX_METAL_ENABLED
        graphicPipelineDescriptor.metalVSFunction = shaderName + "_vs";
        graphicPipelineDescriptor.metalFSFunction = shaderName + "_fs";
    #endif 
    #if GFX_OPENGL_ENABLED
        graphicPipelineDescriptor.openglVSCode = utils::String::contentOfFile(utils::String(OPENGL_SHADER_DIR) + utils::String("/") + shaderName + utils::String(".vs"));
        graphicPipelineDescriptor.openglFSCode = utils::String::contentOfFile(utils::String(OPENGL_SHADER_DIR) + utils::String("/") + shaderName + utils::String(".fs"));
    #endif

    return graphicPipelineDescriptor;
}

int main()
{
    gfx::Platform::init();

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

    #ifdef GFX_IMGUI_ENABLED
        graphicAPI->useForImGui(
            ImGuiConfigFlags_NavEnableKeyboard |
            ImGuiConfigFlags_DockingEnable     |
            ImGuiConfigFlags_ViewportsEnable
        );
    #endif

    #ifdef GFX_METAL_ENABLED
        graphicAPI->initMetalShaderLib(MTL_SHADER_LIB);
    #endif    

    utils::SharedPtr<gfx::GraphicPipeline> triangleGfxPipeline = graphicAPI->newGraphicsPipeline(makeGfxPipelineDescriptor("triangle"));
    utils::SharedPtr<gfx::VertexBuffer> triangleVtxBuffer = graphicAPI->newVertexBuffer(triangle_vertices);

    utils::SharedPtr<gfx::GraphicPipeline> squareGfxPipeline = graphicAPI->newGraphicsPipeline(makeGfxPipelineDescriptor("texturedSquare"));
    utils::SharedPtr<gfx::VertexBuffer> squareVtxBuffer = graphicAPI->newVertexBuffer(square_vertices);
    utils::SharedPtr<gfx::IndexBuffer> squateIdxBuffer = graphicAPI->newIndexBuffer(square_indices);

    utils::SharedPtr<gfx::FrameBuffer> offscreenFrameBuf = graphicAPI->newFrameBuffer(
        graphicAPI->newTexture(gfx::Texture::Descriptor(1000, 1000))
    );

    bool running = true;

    window->addEventCallBack([&](gfx::Event& event)
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
        
        graphicAPI->beginFrame();

        graphicAPI->setClearColor(RED);
        graphicAPI->beginOffScreenRenderPass(offscreenFrameBuf);

        graphicAPI->useGraphicsPipeline(triangleGfxPipeline);
        graphicAPI->useVertexBuffer(triangleVtxBuffer);
        graphicAPI->drawVertices(0, 3);
        
        graphicAPI->endOffScreeRenderPass();

        graphicAPI->setClearColor(BLACK);
        graphicAPI->beginOnScreenRenderPass();

        graphicAPI->useGraphicsPipeline(squareGfxPipeline);
        graphicAPI->useVertexBuffer(squareVtxBuffer);
        graphicAPI->setFragmentTexture(squareGfxPipeline->findFragmentUniformIndex("u_texture"), offscreenFrameBuf->colorTexture());
        graphicAPI->drawIndexedVertices(squateIdxBuffer);

        #ifdef GFX_IMGUI_ENABLED
        ImGui::Text("Hello World");
        #endif

        graphicAPI->endOnScreenRenderPass();
        graphicAPI->endFrame();
    }

    gfx::Platform::terminate();
};