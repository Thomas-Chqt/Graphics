/*
 * ---------------------------------------------------
 * EntityComponent.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/07/18 12:53:03
 * ---------------------------------------------------
 */

#ifndef ENTITYCOMPONENT_HPP
# define ENTITYCOMPONENT_HPP

#include "Math/Vector.hpp"
#include "AssetManager.hpp"
#include "UtilsCPP/String.hpp"

template<typename T>
class Optional
{
public:
    Optional()                = default;
    Optional(const Optional&) = default;
    Optional(Optional&&)      = default;
    
    Optional(const T& data) : m_null(false), m_data(data)
    {
    }

    ~Optional() = default;

private:
    bool m_null = true;
    T m_data;

public:
    Optional& operator = (const Optional&) = default;
    Optional& operator = (Optional&&)      = default;
    
    Optional& operator = (const T& data)
    {
        m_null = false;
        m_data = data;
        return *this;
    }
    
    inline       T& operator  * ()       { return  m_data; }
    inline const T& operator  * () const { return  m_data; }
    inline       T* operator -> ()       { return &m_data; }
    inline const T* operator -> () const { return &m_data; }

    inline operator bool () const { return m_null == false; }
};

struct TransformComponent
{
    math::vec3f pos = { 0, 0, 0 };
    math::vec3f rot = { 0, 0, 0 };
    math::vec3f sca = { 1, 1, 1 };
};

struct ViewPointComponent
{
    bool isActive;
};

struct LightSourceComponent
{
    enum class Type { directional, point, spot } type;
    math::rgb color         = WHITE3;
    float ambiantIntensity  = 0.25;
    float diffuseIntensity  = 0.5;
    float specularIntensity = 0.5;
};

struct RenderableComponent
{
    Mesh mesh;
};

struct Entity
{
    utils::String name;

    Optional<TransformComponent> transformComponent;
    Optional<ViewPointComponent> viewPointComponent;
    Optional<LightSourceComponent> lightSourceComponent;
    Optional<RenderableComponent> renderableComponent;
};

#endif // ENTITYCOMPONENT_HPP