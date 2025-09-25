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

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace scop
{

class Camera
{
public:
    virtual glm::mat4x4 viewMatrix() const = 0;

    virtual ~Camera() = default;

protected:
    Camera() = default;
    Camera(const Camera&) = default;
    Camera(Camera&&) = default;

protected:
    Camera& operator=(const Camera&) = default;
    Camera& operator=(Camera&&) = default;
};

class FixCamera : public Camera
{
public:
    FixCamera() = default;
    FixCamera(const FixCamera&) = default;
    FixCamera(FixCamera&&) = default;

    inline glm::mat4x4 viewMatrix() const override
    {
        constexpr glm::vec3 camPos = glm::vec3(0.0f, 0.0f,  3.0f);
        constexpr glm::vec3 camDir = glm::vec3(0.0f, 0.0f, -1.0f);
        constexpr glm::vec3 camUp  = glm::vec3(0.0f, 1.0f,  0.0f);
        return glm::lookAt(camPos, camPos + camDir, camUp);
    }

    ~FixCamera() override = default;

public:
    FixCamera& operator=(const FixCamera&) = default;
    FixCamera& operator=(FixCamera&&) = default;
};



} // namespace scop

#endif // CAMERA_HPP
