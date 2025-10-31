/*
 * ---------------------------------------------------
 * Entity.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/10/31 16:22:42
 * ---------------------------------------------------
 */

#include "Entity.hpp"
#include "Mesh.hpp"

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

namespace scop
{

RenderableEntity::RenderableEntity(const Mesh& mesh)
 : m_mesh(mesh)
{
}

glm::mat4x4 RenderableEntity::modelMatrix() const
{
    auto matrix = glm::mat4x4(1.0f);
    matrix = glm::translate(matrix, position());
    matrix = glm::rotate(matrix, rotation().x, glm::vec3(1, 0, 0));
    matrix = glm::rotate(matrix, rotation().y, glm::vec3(0, 1, 0));
    matrix = glm::rotate(matrix, rotation().z, glm::vec3(0, 0, 1));
    matrix = glm::scale(matrix, glm::vec3(scale()));
    return matrix;
}

}
