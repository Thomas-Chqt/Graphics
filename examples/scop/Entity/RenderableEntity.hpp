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

#include <future>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <optional>

namespace scop
{

class RenderableEntity : public Entity
{
public:
    RenderableEntity() = default;
    RenderableEntity(const RenderableEntity&) = delete;
    RenderableEntity(RenderableEntity&&) = default;

    RenderableEntity(std::future<Mesh>&& meshFuture) : m_meshFuture(std::move(meshFuture)) {};

    inline std::optional<Mesh> mesh()
    {
        if (m_mesh.has_value())
            return m_mesh;
        if (m_meshFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
            m_mesh = m_meshFuture.get();
        return m_mesh;
    }

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
    std::future<Mesh> m_meshFuture;
    std::optional<Mesh> m_mesh;

public:
    RenderableEntity& operator=(const RenderableEntity&) = delete;
    RenderableEntity& operator=(RenderableEntity&&) = default;
};

} // namespace scop

#endif // RENDERABLEENTITY_HPP
