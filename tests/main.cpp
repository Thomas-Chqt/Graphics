/*
 * ---------------------------------------------------
 * main.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/05/19 12:58:32
 * ---------------------------------------------------
 */

#include <gtest/gtest.h>

#include "Graphics/ShaderLibrary.hpp"
#include "Graphics/Platform.hpp"

using namespace gfx;

class Environment : public ::testing::Environment
{
public:
    ~Environment() override {}

    void SetUp() override
    {
        Platform::init();
        ShaderLibrary::init();

        #ifdef USING_METAL
            ShaderLibrary::shared().setMetalShaderLibPath(MTL_SHADER_LIB);
        #endif

        #if defined (USING_METAL) && defined (USING_OPENGL)
            ShaderLibrary::shared().registerShader("graphicPipeline_vertex",   "graphicPipeline_vertex",   utils::String::contentOfFile(OPENGL_SHADER_DIR"/graphicPipeline/vertex.glsl"));
            ShaderLibrary::shared().registerShader("graphicPipeline_fragment", "graphicPipeline_fragment", utils::String::contentOfFile(OPENGL_SHADER_DIR"/graphicPipeline/fragment.glsl"));
            ShaderLibrary::shared().registerShader("triangle_vertex",          "triangle_vertex",          utils::String::contentOfFile(OPENGL_SHADER_DIR"/triangle/vertex.glsl"));
            ShaderLibrary::shared().registerShader("triangle_fragment",        "triangle_fragment",        utils::String::contentOfFile(OPENGL_SHADER_DIR"/triangle/fragment.glsl"));
            ShaderLibrary::shared().registerShader("indexedShape_vertex",      "indexedShape_vertex",      utils::String::contentOfFile(OPENGL_SHADER_DIR"/indexedShape/vertex.glsl"));
            ShaderLibrary::shared().registerShader("indexedShape_fragment",    "indexedShape_fragment",    utils::String::contentOfFile(OPENGL_SHADER_DIR"/indexedShape/fragment.glsl"));
            ShaderLibrary::shared().registerShader("fragmentUniform_vertex",   "fragmentUniform_vertex",   utils::String::contentOfFile(OPENGL_SHADER_DIR"/fragmentUniform/vertex.glsl"));
            ShaderLibrary::shared().registerShader("fragmentUniform_fragment", "fragmentUniform_fragment", utils::String::contentOfFile(OPENGL_SHADER_DIR"/fragmentUniform/fragment.glsl"));
            ShaderLibrary::shared().registerShader("flatColorCube_vertex",     "flatColorCube_vertex",     utils::String::contentOfFile(OPENGL_SHADER_DIR"/flatColorCube/vertex.glsl"));
            ShaderLibrary::shared().registerShader("flatColorCube_fragment",   "flatColorCube_fragment",   utils::String::contentOfFile(OPENGL_SHADER_DIR"/flatColorCube/fragment.glsl"));
            ShaderLibrary::shared().registerShader("texturedSquare_vertex",    "texturedSquare_vertex",    utils::String::contentOfFile(OPENGL_SHADER_DIR"/texturedSquare/vertex.glsl"));
            ShaderLibrary::shared().registerShader("texturedSquare_fragment",  "texturedSquare_fragment",  utils::String::contentOfFile(OPENGL_SHADER_DIR"/texturedSquare/fragment.glsl"));
            ShaderLibrary::shared().registerShader("noClearBuffer_vertex",     "noClearBuffer_vertex",     utils::String::contentOfFile(OPENGL_SHADER_DIR"/noClearBuffer/vertex.glsl"));
            ShaderLibrary::shared().registerShader("noClearBuffer_fragment",   "noClearBuffer_fragment",   utils::String::contentOfFile(OPENGL_SHADER_DIR"/noClearBuffer/fragment.glsl"));
        #elif defined (USING_METAL)
            ShaderLibrary::shared().registerShader("graphicPipeline_vertex",   "graphicPipeline_vertex");
            ShaderLibrary::shared().registerShader("graphicPipeline_fragment", "graphicPipeline_fragment");
            ShaderLibrary::shared().registerShader("triangle_vertex",          "triangle_vertex");
            ShaderLibrary::shared().registerShader("triangle_fragment",        "triangle_fragment");
            ShaderLibrary::shared().registerShader("indexedShape_vertex",      "indexedShape_vertex");
            ShaderLibrary::shared().registerShader("indexedShape_fragment",    "indexedShape_fragment");
            ShaderLibrary::shared().registerShader("fragmentUniform_vertex",   "fragmentUniform_vertex");
            ShaderLibrary::shared().registerShader("fragmentUniform_fragment", "fragmentUniform_fragment");
            ShaderLibrary::shared().registerShader("flatColorCube_vertex",     "flatColorCube_vertex");
            ShaderLibrary::shared().registerShader("flatColorCube_fragment",   "flatColorCube_fragment");
            ShaderLibrary::shared().registerShader("texturedSquare_vertex",    "texturedSquare_vertex");
            ShaderLibrary::shared().registerShader("texturedSquare_fragment",  "texturedSquare_fragment");
            ShaderLibrary::shared().registerShader("noClearBuffer_vertex",     "noClearBuffer_vertex");
            ShaderLibrary::shared().registerShader("noClearBuffer_fragment",   "noClearBuffer_fragment");
        #elif defined (USING_OPENGL)
            ShaderLibrary::shared().registerShader("graphicPipeline_vertex",   utils::String::contentOfFile(OPENGL_SHADER_DIR"/graphicPipeline/vertex.glsl"));
            ShaderLibrary::shared().registerShader("graphicPipeline_fragment", utils::String::contentOfFile(OPENGL_SHADER_DIR"/graphicPipeline/fragment.glsl"));
            ShaderLibrary::shared().registerShader("triangle_vertex",          utils::String::contentOfFile(OPENGL_SHADER_DIR"/triangle/vertex.glsl"));
            ShaderLibrary::shared().registerShader("triangle_fragment",        utils::String::contentOfFile(OPENGL_SHADER_DIR"/triangle/fragment.glsl"));
            ShaderLibrary::shared().registerShader("indexedShape_vertex",      utils::String::contentOfFile(OPENGL_SHADER_DIR"/indexedShape/vertex.glsl"));
            ShaderLibrary::shared().registerShader("indexedShape_fragment",    utils::String::contentOfFile(OPENGL_SHADER_DIR"/indexedShape/fragment.glsl"));
            ShaderLibrary::shared().registerShader("fragmentUniform_vertex",   utils::String::contentOfFile(OPENGL_SHADER_DIR"/fragmentUniform/vertex.glsl"));
            ShaderLibrary::shared().registerShader("fragmentUniform_fragment", utils::String::contentOfFile(OPENGL_SHADER_DIR"/fragmentUniform/fragment.glsl"));
            ShaderLibrary::shared().registerShader("flatColorCube_vertex",     utils::String::contentOfFile(OPENGL_SHADER_DIR"/flatColorCube/vertex.glsl"));
            ShaderLibrary::shared().registerShader("flatColorCube_fragment",   utils::String::contentOfFile(OPENGL_SHADER_DIR"/flatColorCube/fragment.glsl"));
            ShaderLibrary::shared().registerShader("texturedSquare_vertex",    utils::String::contentOfFile(OPENGL_SHADER_DIR"/texturedSquare/vertex.glsl"));
            ShaderLibrary::shared().registerShader("texturedSquare_fragment",  utils::String::contentOfFile(OPENGL_SHADER_DIR"/texturedSquare/fragment.glsl"));
            ShaderLibrary::shared().registerShader("noClearBuffer_vertex",     utils::String::contentOfFile(OPENGL_SHADER_DIR"/noClearBuffer/vertex.glsl"));
            ShaderLibrary::shared().registerShader("noClearBuffer_fragment",   utils::String::contentOfFile(OPENGL_SHADER_DIR"/noClearBuffer/fragment.glsl"));
        #else
        #endif
    }

    void TearDown() override
    {
        ShaderLibrary::terminated();
        Platform::terminate();
    }
};

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(new Environment);
    return RUN_ALL_TESTS();
}
