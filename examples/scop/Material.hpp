/*
 * ---------------------------------------------------
 * Material.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/09/24 21:27:32
 * ---------------------------------------------------
 */

#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <Graphics/GraphicsPipeline.hpp>
#include <Graphics/ParameterBlock.hpp>
#include <Graphics/ParameterBlockPool.hpp>
#include <Graphics/Device.hpp>

#include <memory>

#include <glm/glm.hpp>

namespace scop
{

class Material
{
public:
    Material(const Material&) = delete;

    virtual const std::shared_ptr<gfx::GraphicsPipeline>& graphicsPipleine() const = 0;
    virtual std::unique_ptr<gfx::ParameterBlock> makeParameterBlock(gfx::ParameterBlockPool&) const = 0;

    virtual const char* name() const = 0;

    virtual ~Material() = default;

protected:
    Material() = default;
    Material(Material&&) = default;

    std::shared_ptr<gfx::GraphicsPipeline> m_graphicsPipeline;

public:
    Material& operator=(const Material&) = delete;

protected:
    Material& operator=(Material&&) = default;
};

} // namespace scop

#endif // MATERIAL_HPP
