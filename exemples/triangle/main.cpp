/*
 * ---------------------------------------------------
 * main.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/06 16:59:24
 * ---------------------------------------------------
 */

#include "Graphics/GraphicAPI.hpp"
#include "Graphics/GraphicPipeline.hpp"
#include "Graphics/KeyCodes.hpp"
#include "Graphics/Platform.hpp"
#include "Graphics/ShaderLibrary.hpp"

#include "Graphics/VertexBuffer.hpp"
#include "UtilsCPP/Array.hpp"
#include "UtilsCPP/SharedPtr.hpp"

#include "Vertex.hpp"

int main()
{
    gfx::Platform::init();
    gfx::ShaderLibrary::init();

    bool running = true;

    gfx::Platform::shared().setEventCallBack([&](gfx::Event& ev)
    {
        ev.dispatch<gfx::KeyDownEvent>([&](gfx::KeyDownEvent& e)
        {
            if (e.keyCode() == ESC_KEY)
                running = false;
        });
    });

    #ifdef GFX_METAL_ENABLED
        gfx::ShaderLibrary::shared().setMetalShaderLibPath(MTL_SHADER_LIB);
    #endif

    gfx::ShaderLibrary::shared().registerShader("triangle_vs"
        #if GFX_METAL_ENABLED
            , "triangle_vs"
        #endif
        #if GFX_OPENGL_ENABLED
            , utils::String::contentOfFile(OPENGL_SHADER_DIR"/triangle.vs.glsl")
        #endif
    );

    utils::SharedPtr<gfx::GraphicAPI> graphicAPI = gfx::Platform::shared().newDefaultGraphicAPI(gfx::Platform::shared().newDefaultWindow(800, 600));

    utils::Array<Vertex> vertices = {
        {-1, -1},
        { 0,  1},
        { 1, -1}
    };

    utils::SharedPtr<gfx::VertexBuffer> vertexBuffer = graphicAPI->newVertexBuffer(vertices);
    utils::SharedPtr<gfx::GraphicPipeline> graphicPipeline = graphicAPI->newGraphicsPipeline("triangle_vs", "triangle_vs");

    while (running)
    {
        gfx::Platform::shared().pollEvents();
        
        graphicAPI->beginFrame();
        
        graphicAPI->useVertexBuffer(vertexBuffer);
        graphicAPI->useGraphicsPipeline(graphicPipeline);
        graphicAPI->drawVertices(0, 3);

        graphicAPI->endFrame();
    }}