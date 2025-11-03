/*
 * ---------------------------------------------------
 * RenderableEntity.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/11/01 17:58:56
 * ---------------------------------------------------
 */

#ifndef RENDERABLEENTITY_HPP
#define RENDERABLEENTITY_HPP

#include "Entity/Entity.hpp"
#include "Mesh.hpp"

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

namespace scop
{

class RenderableEntity : public Entity
{
public:
    RenderableEntity() = default;
    RenderableEntity(const RenderableEntity&) = default;
    RenderableEntity(RenderableEntity&&) = default;

    RenderableEntity(const Mesh& mesh) : m_mesh(mesh) {};

    inline const Mesh& mesh() const { return m_mesh; }
    inline void setMesh(const Mesh& m) { m_mesh = m; }

    inline glm::mat4x4 modelMatrix() const
    {
        auto matrix = glm::mat4x4(1.0f);
        matrix = glm::translate(matrix, position());
        matrix = glm::rotate(matrix, rotation().x, glm::vec3(1, 0, 0));
        matrix = glm::rotate(matrix, rotation().y, glm::vec3(0, 1, 0));
        matrix = glm::rotate(matrix, rotation().z, glm::vec3(0, 0, 1));
        matrix = glm::scale(matrix, glm::vec3(scale()));
        return matrix;
    }

    ~RenderableEntity() override = default;

private:
    Mesh m_mesh;

public:
    RenderableEntity& operator=(const RenderableEntity&) = default;
    RenderableEntity& operator=(RenderableEntity&&) = default;
};

} // namespace scop

#endif // RENDERABLEENTITY_HPP
