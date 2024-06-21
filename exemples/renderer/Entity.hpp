/*
 * ---------------------------------------------------
 * Entity.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/21 10:35:56
 * ---------------------------------------------------
 */

#ifndef ENTITY_HPP
# define ENTITY_HPP

#include "Graphics/GraphicPipeline.hpp"
#include "Graphics/IndexBuffer.hpp"
#include "Graphics/Texture.hpp"
#include "Graphics/VertexBuffer.hpp"
#include "Math/Vector.hpp"
#include "Math/Matrix.hpp"
#include "ShaderDatas.hpp"
#include "UtilsCPP/Array.hpp"
#include "UtilsCPP/SharedPtr.hpp"

struct Entity
{
    math::vec3f position;
    math::vec3f rotation;

    inline math::mat4x4 modelMatrix() const { return math::mat4x4::translation(position) * math::mat4x4::rotation(rotation); }

    inline math::vec3f right()   const { return (math::mat4x4::rotation({0, rotation.y, 0}) * math::mat4x4::rotation({rotation.x, 0, 0}) * math::vec4f({1, 0, 0, 1})).xyz().normalized(); }
    inline math::vec3f up()      const { return (math::mat4x4::rotation({0, rotation.y, 0}) * math::mat4x4::rotation({rotation.x, 0, 0}) * math::vec4f({0, 1, 0, 1})).xyz().normalized(); }
    inline math::vec3f forward() const { return (math::mat4x4::rotation({0, rotation.y, 0}) * math::mat4x4::rotation({rotation.x, 0, 0}) * math::vec4f({0, 0, 1, 1})).xyz().normalized(); }
};

struct Camera : public Entity
{
    inline math::mat4x4 viewMatrix() const
    {
        return math::mat4x4(
            right().x,   right().y,   right().z,   -dot(right(), position),
               up().x,      up().y,      up().z,      -dot(up(), position),
          forward().x, forward().y, forward().z, -dot(forward(), position),
                    0,           0,           0,                        1);
    }
};

struct RenderableEntity : public Entity
{
    struct SubMesh
    {
        utils::SharedPtr<gfx::VertexBuffer> vertexBuffer;
        utils::SharedPtr<gfx::IndexBuffer> indexBuffer;
        utils::SharedPtr<gfx::GraphicPipeline> pipeline;
        Material material;
        utils::SharedPtr<gfx::Texture> diffuseTexture;
    };

    utils::Array<SubMesh> subMeshes;
};

#endif // ENTITY_HPP