/*
 * ---------------------------------------------------
 * main.cpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/14 23:37:10
 * ---------------------------------------------------
 */

#include "Graphics/GraphicAPI.hpp"
#include "Graphics/GraphicPipeline.hpp"
#include "UtilsCPP/Array.hpp"
#include "UtilsCPP/RuntimeError.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "UtilsCPP/String.hpp"
#include "Vertex.hpp"
#include "helpers.hpp"

struct Entity
{
    math::vec3f position;
    math::vec3f rotation;

    math::mat4x4 modelMatrix() const
    {
        return math::mat4x4(
            1, 0, 0, position.x,
            0, 1, 0, position.y,
            0, 0, 1, position.z,
            0, 0, 0,          1
        ) 
        * math::mat4x4(
            1,                    0,                     0, 0,
            0, std::cos(rotation.x), -std::sin(rotation.x), 0,
            0, std::sin(rotation.x),  std::cos(rotation.x), 0,
            0,                    0,                     0, 1
        ) 
        * math::mat4x4(
             std::cos(rotation.y), 0, std::sin(rotation.y), 0,
                                0, 1,                    0, 0,
            -std::sin(rotation.y), 0, std::cos(rotation.y), 0,
                                0, 0,                    0, 1
        ) 
        * math::mat4x4(
            std::cos(rotation.z), -std::sin(rotation.z), 0, 0,
            std::sin(rotation.z),  std::cos(rotation.z), 0, 0,
                               0,                     0, 1, 0,
                               0,                     0, 0, 1
        );
    }
};

struct RenderableEntity : public Entity
{
    utils::SharedPtr<gfx::VertexBuffer> vertexBuffer;
    utils::SharedPtr<gfx::IndexBuffer> indexBuffer;
    utils::SharedPtr<gfx::Texture> texture;

    virtual void setUniform(const utils::SharedPtr<gfx::GraphicAPI>& api, const utils::SharedPtr<gfx::GraphicPipeline>& gfxPipeline, const utils::String& uniform) = 0;
};

struct Camera : public Entity
{
};

struct GrassCube : RenderableEntity
{
    GrassCube(const utils::SharedPtr<gfx::GraphicAPI>& api)
    {
        vertexBuffer = api->newVertexBuffer<Vertex>(
        {
            {{ -0.5, -0.5, -0.5 }, { 0.0, 1.0 }},
            {{ -0.5,  0.5, -0.5 }, { 0.0, 0.0 }},
            {{  0.5,  0.5, -0.5 }, { 1.0, 0.0 }},
            {{  0.5, -0.5, -0.5 }, { 1.0, 1.0 }},
            {{ -0.5, -0.5,  0.5 }, { 0.0, 1.0 }},
            {{ -0.5,  0.5,  0.5 }, { 0.0, 0.0 }},
            {{  0.5,  0.5,  0.5 }, { 1.0, 0.0 }},
            {{  0.5, -0.5,  0.5 }, { 1.0, 1.0 }},
            {{ -0.5,  0.5, -0.5 }, { 0.0, 1.0 }},
            {{ -0.5,  0.5,  0.5 }, { 0.0, 0.0 }},
            {{  0.5,  0.5,  0.5 }, { 1.0, 0.0 }},
            {{  0.5,  0.5, -0.5 }, { 1.0, 1.0 }},
            {{ -0.5, -0.5, -0.5 }, { 0.0, 1.0 }},
            {{ -0.5, -0.5,  0.5 }, { 0.0, 0.0 }},
            {{  0.5, -0.5,  0.5 }, { 1.0, 0.0 }},
            {{  0.5, -0.5, -0.5 }, { 1.0, 1.0 }},
            {{ -0.5, -0.5,  0.5 }, { 0.0, 1.0 }},
            {{ -0.5,  0.5,  0.5 }, { 0.0, 0.0 }},
            {{ -0.5,  0.5, -0.5 }, { 1.0, 0.0 }},
            {{ -0.5, -0.5, -0.5 }, { 1.0, 1.0 }},
            {{  0.5, -0.5, -0.5 }, { 0.0, 1.0 }},
            {{  0.5,  0.5, -0.5 }, { 0.0, 0.0 }},
            {{  0.5,  0.5,  0.5 }, { 1.0, 0.0 }},
            {{  0.5, -0.5,  0.5 }, { 1.0, 1.0 }}
        });

        indexBuffer = api->newIndexBuffer(
        {
            0,  1,  2,  0,  2,  3,
            4,  5,  6,  4,  6,  7,
            8,  9, 10,  8, 10, 11,
            12, 13, 14, 12, 14, 15,
            16, 17, 18, 16, 18, 19,
            20, 21, 22, 20, 22, 23
        });

        texture = textureFromFile(api, RESSOURCES_DIR"/mc_grass.jpg");
    }

    void setUniform(const utils::SharedPtr<gfx::GraphicAPI>& api, const utils::SharedPtr<gfx::GraphicPipeline>& gfxPipeline, const utils::String& uniform) override
    {
        if(uniform == utils::String("u_ModelMatrix"))
            api->setVertexUniform(gfxPipeline->findVertexUniformIndex(uniform), modelMatrix());
        else if (uniform == utils::String("u_texture"))
            api->setFragmentTexture(gfxPipeline->findVertexUniformIndex(uniform), texture);
        else
            throw utils::RuntimeError("No such uniform");
    }
};

class Renderer
{
public:
    Renderer(utils::SharedPtr<gfx::GraphicAPI> api) : m_graphicApi(api)
    {
    }

private:
    utils::SharedPtr<gfx::GraphicAPI> m_graphicApi;
    gfx::GraphicPipeline* m_pipeline;
    utils::Array<RenderableEntity*> m_sceneRenderables;
};

int main()
{
    

}