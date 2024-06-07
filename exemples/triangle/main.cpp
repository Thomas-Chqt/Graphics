/*
 * ---------------------------------------------------
 * main.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/06 16:59:24
 * ---------------------------------------------------
 */

#include "Graphics/Event.hpp"
#include "Graphics/GraphicAPI.hpp"
#include "Graphics/GraphicPipeline.hpp"
#include "Graphics/KeyCodes.hpp"
#include "Graphics/Platform.hpp"
#include "Graphics/ShaderLibrary.hpp"

#include "Graphics/VertexBuffer.hpp"
#include "Graphics/Window.hpp"
#include "UtilsCPP/Array.hpp"
#include "UtilsCPP/SharedPtr.hpp"

#include "Vertex.hpp"

int main()
{
    gfx::Platform::init();
    gfx::ShaderLibrary::init();

    #ifdef GFX_METAL_ENABLED
        gfx::ShaderLibrary::shared().setMetalShaderLibPath(MTL_SHADER_LIB);
    #endif

    gfx::ShaderLibrary::shared().registerShader("triangle_vs"
        #if GFX_METAL_ENABLED
            , "triangle_vs"
        #endif
        #if GFX_OPENGL_ENABLED
            , utils::String::contentOfFile(OPENGL_SHADER_DIR"/triangle.vs")
        #endif
    );
    gfx::ShaderLibrary::shared().registerShader("triangle_fs"
        #if GFX_METAL_ENABLED
            , "triangle_fs"
        #endif
        #if GFX_OPENGL_ENABLED
            , utils::String::contentOfFile(OPENGL_SHADER_DIR"/triangle.fs")
        #endif
    );

    const utils::Array<Vertex> vertices = {
        {-1, -1},
        { 0,  1},
        { 1, -1}
    };

    utils::SharedPtr<gfx::Window> window = gfx::Platform::shared().newDefaultWindow(800, 600);
    utils::SharedPtr<gfx::GraphicAPI> graphicAPI = gfx::Platform::shared().newDefaultGraphicAPI(window);

    utils::SharedPtr<gfx::GraphicPipeline> graphicPipeline = graphicAPI->newGraphicsPipeline("triangle_vs", "triangle_fs");
    utils::SharedPtr<gfx::VertexBuffer> vertexBuffer = graphicAPI->newVertexBuffer(vertices);

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
        graphicAPI->drawVertices(0, 3);

        graphicAPI->endFrame();
    }

    gfx::ShaderLibrary::terminated();
    gfx::Platform::terminate();
}