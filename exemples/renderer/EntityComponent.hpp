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
#include "UtilsCPP/Optional.hpp"
#include "UtilsCPP/String.hpp"

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

    utils::Optional<TransformComponent> transformComponent;
    utils::Optional<ViewPointComponent> viewPointComponent;
    utils::Optional<LightSourceComponent> lightSourceComponent;
    utils::Optional<RenderableComponent> renderableComponent;
};

#endif // ENTITYCOMPONENT_HPP