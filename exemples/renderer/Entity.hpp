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

#include "Graphics/IndexBuffer.hpp"
#include "Graphics/VertexBuffer.hpp"
#include "MaterialLibrary.hpp"
#include "Math/Vector.hpp"
#include "Math/Matrix.hpp"
#include "UtilsCPP/Array.hpp"
#include "UtilsCPP/SharedPtr.hpp"
#include "UtilsCPP/String.hpp"

struct Entity
{
    Entity() = default;

    utils::String name;

    math::mat4x4 baseMat = math::mat4x4(1.0f);
    math::vec3f position = { 0.0, 0.0, 0.0 };
    math::vec3f rotation = { 0.0, 0.0, 0.0 };
    math::vec3f scale    = { 1.0, 1.0, 1.0 };

    inline math::mat4x4 modelMatrix() const { return math::mat4x4::translation(position) * math::mat4x4::rotation(rotation) * math::mat4x4::scale(scale); }

    inline math::vec3f right()   const { return (math::mat4x4::rotation(rotation) * math::vec4f({1, 0, 0, 1})).xyz().normalized(); }
    inline math::vec3f up()      const { return (math::mat4x4::rotation(rotation) * math::vec4f({0, 1, 0, 1})).xyz().normalized(); }
    inline math::vec3f forward() const { return (math::mat4x4::rotation(rotation) * math::vec4f({0, 0, 1, 1})).xyz().normalized(); }

    virtual ~Entity() = default;
};

struct Camera : public Entity
{
    inline math::mat4x4 viewMatrix() const { return modelMatrix().inversed(); }

    ~Camera() override = default;
};

struct PointLight : public Entity
{
    math::rgb color;
    float     ambiantIntensity;
    float     diffuseIntensity;
    float     specularIntensity;

    ~PointLight() override = default;
};

struct RenderableEntity : public Entity
{
    struct Mesh
    {
        utils::SharedPtr<gfx::VertexBuffer> vertexBuffer;
        utils::SharedPtr<gfx::IndexBuffer> indexBuffer;
        utils::SharedPtr<Material> material;
        math::mat4x4 modelMatrix;
    };

    utils::Array<Mesh> meshes;
    
    ~RenderableEntity() override = default;
};


#endif // ENTITY_HPP