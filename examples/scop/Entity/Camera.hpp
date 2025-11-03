/*
 * ---------------------------------------------------
 * Camera.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/09/16 08:05:20
 * ---------------------------------------------------
 */

#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "Entity/Entity.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace scop
{

class Camera : public Entity
{
public:
    virtual glm::mat4x4 viewMatrix() const = 0;

    ~Camera() override = default;

protected:
    Camera() = default;
    Camera(const Camera&) = default;
    Camera(Camera&&) = default;

protected:
    Camera& operator=(const Camera&) = default;
    Camera& operator=(Camera&&) = default;
};

class FlightCamera : public Camera
{
public:
    FlightCamera() = default;
    FlightCamera(const FlightCamera&) = default;
    FlightCamera(FlightCamera&&) = default;

    inline glm::mat4x4 viewMatrix() const override
    {
        auto rotationMat = glm::mat4x4(1.0f);
        rotationMat = glm::rotate(rotationMat, rotation().y, glm::vec3(0, 1, 0));
        rotationMat = glm::rotate(rotationMat, rotation().x, glm::vec3(1, 0, 0));
        rotationMat = glm::rotate(rotationMat, rotation().z, glm::vec3(0, 0, 1));

        glm::vec3 pos = position();
        glm::vec3 dir = rotationMat * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
        glm::vec3 up = rotationMat * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);

        return glm::lookAt(pos, pos + dir, up);
    }

    ~FlightCamera() override = default;

public:
    FlightCamera& operator=(const FlightCamera&) = default;
    FlightCamera& operator=(FlightCamera&&) = default;
};

} // namespace scop

#endif // CAMERA_HPP
