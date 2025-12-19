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

#include <GLFW/glfw3.h>
#if !defined (SCOP_MANDATORY)
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#else
#include "math/math.hpp"
#ifndef SCOP_MATH_GLM_ALIAS_DEFINED
#define SCOP_MATH_GLM_ALIAS_DEFINED
namespace glm = scop::math;
#endif
#endif

#include <set>

namespace scop
{

class Camera : public Entity
{
public:
    virtual void update(const std::set<int>& pressedKeys, double deltaTime) = 0;
    virtual glm::mat4x4 viewMatrix() const = 0;

    inline float fov() const { return m_fovDegrees; }
    inline float nearPlane() const { return m_nearPlane; }
    inline float farPlane() const { return m_farPlane; }

    inline void setFov(float fovDegrees) { m_fovDegrees = fovDegrees; }
    inline void setNearPlane(float nearPlane) { m_nearPlane = nearPlane; }
    inline void setFarPlane(float farPlane) { m_farPlane = farPlane; }

    ~Camera() override = default;

protected:
    Camera() = default;
    Camera(const Camera&) = default;
    Camera(Camera&&) = default;

protected:
    Camera& operator=(const Camera&) = default;
    Camera& operator=(Camera&&) = default;

private:
    float m_fovDegrees = 60.0f;
    float m_nearPlane = 0.1f;
    float m_farPlane = 1000.0f;
};

class FixedCamera : public Camera
{
public:
    FixedCamera() = default;
    FixedCamera(const FixedCamera&) = default;
    FixedCamera(FixedCamera&&) = default;

    inline void update(const std::set<int>&, double) override {};

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

    ~FixedCamera() override = default;

public:
    FixedCamera& operator=(const FixedCamera&) = default;
    FixedCamera& operator=(FixedCamera&&) = default;
};

class FlightCamera : public Camera
{
public:
    FlightCamera() = default;
    FlightCamera(const FlightCamera&) = default;
    FlightCamera(FlightCamera&&) = default;

    inline void update(const std::set<int>& pressedKeys, double deltaTime) override
    {
        glm::vec3 rotationInput = glm::vec3{
            (pressedKeys.contains(GLFW_KEY_UP)   ? 1.0f : 0.0f) - (pressedKeys.contains(GLFW_KEY_DOWN)  ? 1.0f : 0.0f),
            (pressedKeys.contains(GLFW_KEY_LEFT) ? 1.0f : 0.0f) - (pressedKeys.contains(GLFW_KEY_RIGHT) ? 1.0f : 0.0f),
            0
        };
        if (glm::length(rotationInput) > 0.0f)
            setRotation(rotation() + glm::normalize(rotationInput) * (float)deltaTime * 2.0f);

        auto rotationMat = glm::mat4x4(1.0f);
        rotationMat = glm::rotate(rotationMat, rotation().y, glm::vec3(0, 1, 0));
        rotationMat = glm::rotate(rotationMat, rotation().x, glm::vec3(1, 0, 0));

        glm::vec3 movementInput = glm::vec3{
            (pressedKeys.contains(GLFW_KEY_D) ? 1.0f : 0.0f) - (pressedKeys.contains(GLFW_KEY_A) ? 1.0f : 0.0f),
            0,
            (pressedKeys.contains(GLFW_KEY_S) ? 1.0f : 0.0f) - (pressedKeys.contains(GLFW_KEY_W) ? 1.0f : 0.0f)
        };
        if (glm::length(movementInput) > 0.0f)
            setPosition(position() + glm::vec3(rotationMat * glm::vec4(glm::normalize(movementInput), 0)) * (float)deltaTime * 4.0f);
    };

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
