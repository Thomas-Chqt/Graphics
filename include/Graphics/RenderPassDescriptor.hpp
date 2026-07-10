/*
 * ---------------------------------------------------
 * RenderPassDescriptor.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2026/07/10
 * ---------------------------------------------------
 */

#ifndef RENDERPASSDESCRIPTOR_HPP
#define RENDERPASSDESCRIPTOR_HPP

#include "Graphics/Enums.hpp"
#include "Graphics/Texture.hpp"

#include <array>
#include <cstdint>
#include <memory>
#include <optional>
#include <variant>
#include <vector>

namespace gfx
{

struct ClearFloatColor
{
    std::array<float, 4> value;
};

struct ClearUIntColor
{
    std::array<uint32_t, 4> value;
};

struct ClearDepth
{
    float value;
};

struct ClearValue
{
    using Value = std::variant<ClearFloatColor, ClearUIntColor, ClearDepth>;

    Value value = ClearFloatColor{{0.0f, 0.0f, 0.0f, 0.0f}};

    static ClearValue color(std::array<float, 4> value) { return ClearValue{ClearFloatColor{value}}; }
    static ClearValue uintColor(std::array<uint32_t, 4> value) { return ClearValue{ClearUIntColor{value}}; }
    static ClearValue uint64(uint64_t value)
    {
        return uintColor({
            static_cast<uint32_t>(value),
            static_cast<uint32_t>(value >> 32),
            0,
            0
        });
    }
    static ClearValue depth(float value) { return ClearValue{ClearDepth{value}}; }
};

class RenderPassDescriptor
{
public:
    struct Attachment
    {
        LoadAction loadAction = LoadAction::load;
        ClearValue clearValue = ClearValue::color({0.0f, 0.0f, 0.0f, 0.0f});
        std::shared_ptr<Texture> texture;
    };

public:
    RenderPassDescriptor(const RenderPassDescriptor&) = delete;
    RenderPassDescriptor(RenderPassDescriptor&&) = delete;

    virtual const std::vector<Attachment>& colorAttachments() const = 0;
    virtual void setColorAttachments(std::vector<Attachment>) = 0;

    virtual const std::optional<Attachment>& depthAttachment() const = 0;
    virtual void setDepthAttachment(std::optional<Attachment>) = 0;

    virtual ~RenderPassDescriptor() = default;

protected:
    RenderPassDescriptor() = default;

public:
    RenderPassDescriptor& operator=(const RenderPassDescriptor&) = delete;
    RenderPassDescriptor& operator=(RenderPassDescriptor&&) = delete;
};

} // namespace gfx

#endif // RENDERPASSDESCRIPTOR_HPP
