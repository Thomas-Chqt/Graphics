/*
 * ---------------------------------------------------
 * Light.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/09/27 14:34:47
 * ---------------------------------------------------
 */

#ifndef SCOP_LIGHT_HPP
#define SCOP_LIGHT_HPP

#include <glm/glm.hpp>

namespace scop
{

class Light
{
public:
    inline const glm::vec3& color() const { return m_color; }
    inline void setColor(const glm::vec3& c) { m_color = c; }

    virtual ~Light() = default;

protected:
    Light() = default;
    Light(const Light&) = default;
    Light(Light&&) = default;

    glm::vec3 m_color = { 1.0f, 1.0f, 1.0f };

protected:
    Light& operator=(const Light&) = default;
    Light& operator=(Light&&) = default;
};

class DirectionalLight : public Light
{
public:
    DirectionalLight() = default;
    DirectionalLight(const DirectionalLight&) = default;
    DirectionalLight(DirectionalLight&&) = default;

    ~DirectionalLight() override = default;

public:
    DirectionalLight& operator=(const DirectionalLight&) = default;
    DirectionalLight& operator=(DirectionalLight&&) = default;
};

class PointLight : public Light
{
public:
    PointLight() = default;
    PointLight(const PointLight&) = default;
    PointLight(PointLight&&) = default;

    inline float attenuation() const { return m_attenuation; }
    inline void setAttenuation(float a) { m_attenuation = a; }

private:
    float m_attenuation = 0.5;

public:
    ~PointLight() override = default;

public:
    PointLight& operator=(const PointLight&) = default;
    PointLight& operator=(PointLight&&) = default;
};

} // namespace scop

#endif // SCOP_LIGHT_HPP
