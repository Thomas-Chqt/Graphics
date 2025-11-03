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

#include <glm/glm.hpp>
#include <string>

namespace scop
{

class Entity
{
public:
    Entity() = default;
    Entity(const Entity&) = default;
    Entity(Entity&&) = default;

    inline const std::string& name() const { return m_name; }
    inline void setName(const std::string& n) { m_name = n; }

    inline const glm::vec3& position() const { return m_position; }
    inline void setPosition(const glm::vec3& p) { m_position = p; }

    inline const glm::vec3& rotation() const { return m_rotation; }
    inline void setRotation(const glm::vec3& r) { m_rotation = r; }

    inline const float& scale() const { return m_scale; }
    inline void setScale(const float& s) { m_scale = s; }

    virtual ~Entity() = default;

private:
    std::string m_name;
    glm::vec3 m_position = glm::vec3(0.0f);
    glm::vec3 m_rotation = glm::vec3(0.0f);
    float m_scale = 1.0f;

public:
    Entity& operator=(const Entity&) = default;
    Entity& operator=(Entity&&) = default;
};

} // namespace scop

#endif // ENTITY_HPP
