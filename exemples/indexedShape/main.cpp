/*
 * ---------------------------------------------------
 * main.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/07 15:52:51
 * ---------------------------------------------------
 */

#include "Graphics/Event.hpp"
#include "Graphics/GraphicAPI.hpp"
#include "Graphics/GraphicPipeline.hpp"
#include "Graphics/KeyCodes.hpp"
#include "Graphics/Platform.hpp"

#include "Graphics/VertexBuffer.hpp"
#include "Graphics/Window.hpp"
#include "UtilsCPP/Array.hpp"
#include "UtilsCPP/SharedPtr.hpp"

#include "UtilsCPP/String.hpp"
#include "Vertex.hpp"

template<>
utils::Array<gfx::VertexBuffer::LayoutElement> gfx::VertexBuffer::getLayout<Vertex>()
{
    return {
        { 2, Type::FLOAT, false, sizeof(Vertex), (void*)0 }
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

    const utils::Array<Vertex> vertices = {
        {-0.5, -0.5},
        {-0.5,  0.5},
        { 0.5,  0.5},
        { 0.5, -0.5}
    };

    const utils::Array<utils::uint32> indices = utils::Array<utils::uint32>({ 0, 1, 2, 0, 2, 3 });

    utils::SharedPtr<gfx::Window> window = gfx::Platform::shared().newDefaultWindow(800, 600);
    utils::SharedPtr<gfx::GraphicAPI> graphicAPI = gfx::Platform::shared().newDefaultGraphicAPI(window);

    #ifdef GFX_METAL_ENABLED
        graphicAPI->initMetalShaderLib(MTL_SHADER_LIB);
    #endif    

    utils::SharedPtr<gfx::GraphicPipeline> graphicPipeline = graphicAPI->newGraphicsPipeline(makeGfxPipelineDescriptor("indexedShape"));
    utils::SharedPtr<gfx::VertexBuffer> vertexBuffer = graphicAPI->newVertexBuffer(vertices);
    utils::SharedPtr<gfx::IndexBuffer> indexBuffer = graphicAPI->newIndexBuffer(indices);

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
        
        graphicAPI->beginFrame();
        
        graphicAPI->useGraphicsPipeline(graphicPipeline);
        graphicAPI->useVertexBuffer(vertexBuffer);
        graphicAPI->drawIndexedVertices(indexBuffer);

        graphicAPI->endFrame();
    }

    gfx::Platform::terminate();
}