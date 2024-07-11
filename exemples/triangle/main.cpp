/*
 * ---------------------------------------------------
 * main.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/06 16:59:24
 * ---------------------------------------------------
 */

#include "Graphics/Buffer.hpp"
#include "Graphics/Enums.hpp"
#include "Graphics/Event.hpp"
#include "Graphics/GraphicAPI.hpp"
#include "Graphics/GraphicPipeline.hpp"
#include "Graphics/KeyCodes.hpp"
#include "Graphics/Platform.hpp"
#include "Graphics/Shader.hpp"
#include "Graphics/VertexLayout.hpp"
#include "Graphics/Window.hpp"
#include "Math/Vector.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "UtilsCPP/String.hpp"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

struct Vertex
{
    math::vec2f position;
};

utils::SharedPtr<gfx::Shader> createVertexShader(gfx::GraphicAPI& api)
{
    gfx::Shader::MetalShaderDescriptor metalShaderDescriptor;
    #ifdef GFX_BUILD_METAL
        metalShaderDescriptor.type = gfx::ShaderType::vertex;
        metalShaderDescriptor.mtlShaderLibPath = MTL_SHADER_LIB;
        metalShaderDescriptor.mtlFunction = "triangle_vs";
    #endif

    gfx::Shader::OpenGLShaderDescriptor openglShaderDescriptor;
    #ifdef GFX_BUILD_OPENGL
        openglShaderDescriptor.type = gfx::ShaderType::vertex;
        openglShaderDescriptor.openglCode = utils::String::contentOfFile(OPENGL_SHADER_DIR"/triangle.vs");
    #endif

    return api.newShader(metalShaderDescriptor, openglShaderDescriptor);
}

utils::SharedPtr<gfx::Shader> createFragmentShader(gfx::GraphicAPI& api)
{
    gfx::Shader::MetalShaderDescriptor metalShaderDescriptor;
    #ifdef GFX_BUILD_METAL
        metalShaderDescriptor.type = gfx::ShaderType::fragment;
        metalShaderDescriptor.mtlShaderLibPath = MTL_SHADER_LIB;
        metalShaderDescriptor.mtlFunction = "triangle_fs";
    #endif

    gfx::Shader::OpenGLShaderDescriptor openglShaderDescriptor;
    #ifdef GFX_BUILD_OPENGL
        openglShaderDescriptor.type = gfx::ShaderType::fragment;
        openglShaderDescriptor.openglCode = utils::String::contentOfFile(OPENGL_SHADER_DIR"/triangle.fs");
    #endif

    return api.newShader(metalShaderDescriptor, openglShaderDescriptor);
}

int main()
{
    gfx::Platform::init();
    {
        const utils::Array<Vertex> vertices = { {{-1, -1}}, {{0,  1}}, {{1, -1}} };

        utils::SharedPtr<gfx::Window> window = gfx::Platform::shared().newWindow(WINDOW_WIDTH, WINDOW_HEIGHT); 
        utils::SharedPtr<gfx::GraphicAPI> graphicAPI = gfx::Platform::shared().newGraphicAPI(window);

        gfx::Buffer::Descriptor bufferDescriptor;
        bufferDescriptor.debugName = "vertexBuffer";
        bufferDescriptor.initialData = (const void*)vertices;
        bufferDescriptor.size = sizeof(Vertex) * vertices.length();
        utils::SharedPtr<gfx::Buffer> vertexBuffer = graphicAPI->newBuffer(bufferDescriptor);

        utils::SharedPtr<gfx::Shader> vertexShader = createVertexShader(*graphicAPI);
        utils::SharedPtr<gfx::Shader> fragmentShader = createFragmentShader(*graphicAPI);

        gfx::VertexLayout vertexLayout;
        vertexLayout.attributes.append({gfx::VertexAttributeFormat::vec2f, 0});
        vertexLayout.stride = sizeof(Vertex);

        gfx::GraphicPipeline::Descriptor graphicPipelineDescriptor;
        graphicPipelineDescriptor.vertexLayout = vertexLayout;
        graphicPipelineDescriptor.vertexShader = vertexShader;
        graphicPipelineDescriptor.fragmentShader = fragmentShader;

        utils::SharedPtr<gfx::GraphicPipeline> graphicPipeline = graphicAPI->newGraphicsPipeline(graphicPipelineDescriptor);

        bool running = true;

        window->addEventCallBack([&](gfx::Event& event) {
            event.dispatch<gfx::KeyDownEvent>([&](gfx::KeyDownEvent& event) {
                if (event.keyCode() == ESC_KEY)
                    running = false;
            });
        });

        while (running)
        {
            gfx::Platform::shared().pollEvents();
            
            graphicAPI->beginFrame();
            graphicAPI->beginRenderPass();
            
            graphicAPI->useGraphicsPipeline(graphicPipeline);
            graphicAPI->useVertexBuffer(vertexBuffer);
            graphicAPI->drawVertices(0, 3);

            graphicAPI->endRenderPass();
            graphicAPI->endFrame();
        }
    }
    gfx::Platform::terminate();
}