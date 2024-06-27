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

#include "Math/Vector.hpp"
#include "Math/Matrix.hpp"
#include "MeshLibrary.hpp"
#include "UtilsCPP/String.hpp"

struct Entity
{
    utils::String name;
    
    math::vec3f position = { 0.0, 0.0, 0.0 };
    math::vec3f rotation = { 0.0, 0.0, 0.0 };
    math::vec3f scale    = { 1.0, 1.0, 1.0 };

    inline math::mat4x4 modelMatrix() const { return math::mat4x4::translation(position) * math::mat4x4::rotation(rotation) * math::mat4x4::scale(scale); }

    inline math::vec3f right()   const { return (math::mat4x4::rotation({0, rotation.y, 0}) * math::mat4x4::rotation({rotation.x, 0, 0}) * math::vec4f({1, 0, 0, 1})).xyz().normalized(); }
    inline math::vec3f up()      const { return (math::mat4x4::rotation({0, rotation.y, 0}) * math::mat4x4::rotation({rotation.x, 0, 0}) * math::vec4f({0, 1, 0, 1})).xyz().normalized(); }
    inline math::vec3f forward() const { return (math::mat4x4::rotation({0, rotation.y, 0}) * math::mat4x4::rotation({rotation.x, 0, 0}) * math::vec4f({0, 0, 1, 1})).xyz().normalized(); }

    virtual ~Entity() = default;
};

struct Camera : public Entity
{
    inline math::mat4x4 viewMatrix() const
    {
        math::vec3f ri = Entity::right();
        math::vec3f up = Entity::up();
        math::vec3f fo = Entity::forward();
        return math::mat4x4(
            ri.x, ri.y, ri.z, -dot(ri, position),
            up.x, up.y, up.z, -dot(up, position),
            fo.x, fo.y, fo.z, -dot(fo, position),
               0,    0,    0,                  1);
    }

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
    inline RenderableEntity(const Model& m) : model(m) {}
    Model model;
    
    ~RenderableEntity() override = default;
};


#endif // ENTITY_HPP