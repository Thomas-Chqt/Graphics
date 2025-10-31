/*
 * ---------------------------------------------------
 * Entity.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/10/31 10:35:24
 * ---------------------------------------------------
 */

#ifndef ENTITY_HPP
#define ENTITY_HPP

#include "Mesh.hpp"

#include <glm/glm.hpp>

namespace scop
{

class Entity
{
public:
    Entity() = default;
    Entity(const Entity&) = default;
    Entity(Entity&&) = default;

    inline const glm::vec3& position() const { return m_position; }
    inline void setPosition(const glm::vec3& p) { m_position = p; }

    inline const glm::vec3& rotation() const { return m_rotation; }
    inline void setRotation(const glm::vec3& r) { m_rotation = r; }

    inline const float& scale() const { return m_scale; }
    inline void setScale(const float& s) { m_scale = s; }
    

    ~Entity() = default;

private:
    glm::vec3 m_position = glm::vec3(0.0f);
    glm::vec3 m_rotation = glm::vec3(0.0f);
    float m_scale = 1.0f;

public:
    Entity& operator=(const Entity&) = default;
    Entity& operator=(Entity&&) = default;
};

class RenderableEntity : public Entity
{
public:
    RenderableEntity() = default;
    RenderableEntity(const RenderableEntity&) = default;
    RenderableEntity(RenderableEntity&&) = default;

    RenderableEntity(const Mesh&);

    inline const Mesh& mesh() const { return m_mesh; }
    inline void setMesh(const Mesh& m) { m_mesh = m; }

    glm::mat4x4 modelMatrix() const;

    ~RenderableEntity() = default;

private:
    Mesh m_mesh;

public:
    RenderableEntity& operator=(const RenderableEntity&) = default;
    RenderableEntity& operator=(RenderableEntity&&) = default;
};

} // namespace scop

#endif // ENTITY_HPP
