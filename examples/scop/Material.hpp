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

#include "shaders/scop_shader.h"
#undef float3

#include <Graphics/Sampler.hpp>
#include <Graphics/Texture.hpp>
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


class FlatColorMaterial : public Material
{
public:
    FlatColorMaterial() = delete;
    FlatColorMaterial(const FlatColorMaterial&) = delete;
    FlatColorMaterial(FlatColorMaterial&&) = delete;

    FlatColorMaterial(const gfx::Device& device);

    static void createPipeline(gfx::Device& device);
    inline static std::shared_ptr<gfx::GraphicsPipeline> getPipeline() { return s_graphicsPipeline; }
    inline static void destroyPipeline() { s_graphicsPipeline.reset(); }

    inline const std::shared_ptr<gfx::GraphicsPipeline>& graphicsPipleine() const override { return s_graphicsPipeline; }

    std::unique_ptr<gfx::ParameterBlock> makeParameterBlock(gfx::ParameterBlockPool& pool) const override;

    inline glm::vec3 color() const { return m_material->content<shader::FlatColorMaterial>()->diffuseColor; }
    inline void setColor(const glm::vec3& c) { m_material->content<shader::FlatColorMaterial>()->diffuseColor = c; }

    inline float shininess() const { return m_material->content<shader::FlatColorMaterial>()->shininess; }
    inline void setShininess(float s) { m_material->content<shader::FlatColorMaterial>()->shininess = s; }

    inline float specular() const { return m_material->content<shader::FlatColorMaterial>()->specular; }
    inline void setSpecular(float s) { m_material->content<shader::FlatColorMaterial>()->specular = s; }

    ~FlatColorMaterial() override = default;

private:
    inline static std::shared_ptr<gfx::GraphicsPipeline> s_graphicsPipeline;

    ext::shared_ptr<gfx::Buffer> m_material;

public:
    FlatColorMaterial& operator=(const FlatColorMaterial&) = delete;
    FlatColorMaterial& operator=(FlatColorMaterial&&) = delete;
};

class TexturedCubeMaterial : public Material
{
public:
    TexturedCubeMaterial() = delete;
    TexturedCubeMaterial(const TexturedCubeMaterial&) = delete;
    TexturedCubeMaterial(TexturedCubeMaterial&&) = delete;

    TexturedCubeMaterial(const gfx::Device& device);

    static void createPipeline(gfx::Device& device);
    inline static std::shared_ptr<gfx::GraphicsPipeline> getPipeline() { return s_graphicsPipeline; }
    inline static void destroyPipeline() { s_graphicsPipeline.reset(); }

    inline const std::shared_ptr<gfx::GraphicsPipeline>& graphicsPipleine() const override { return s_graphicsPipeline; }

    std::unique_ptr<gfx::ParameterBlock> makeParameterBlock(gfx::ParameterBlockPool& pool) const override;

    inline const ext::shared_ptr<gfx::Texture>& texture() const { return m_texture; }
    inline void setTexture(const ext::shared_ptr<gfx::Texture>& t) { m_texture = t; }

    inline const ext::shared_ptr<gfx::Sampler>& sampler() const { return m_sampler; }
    inline void setSampler(const ext::shared_ptr<gfx::Sampler>& s) { m_sampler = s; }

    ~TexturedCubeMaterial() override = default;

private:
    inline static std::shared_ptr<gfx::GraphicsPipeline> s_graphicsPipeline;

    ext::shared_ptr<gfx::Texture> m_texture;
    ext::shared_ptr<gfx::Sampler> m_sampler;

public:
    TexturedCubeMaterial& operator=(const TexturedCubeMaterial&) = delete;
    TexturedCubeMaterial& operator=(TexturedCubeMaterial&&) = delete;
};

} // namespace scop

#endif // MATERIAL_HPP
