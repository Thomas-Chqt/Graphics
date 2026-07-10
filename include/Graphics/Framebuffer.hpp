/*
 * ---------------------------------------------------
 * Framebuffer.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/06 06:41:27
 * ---------------------------------------------------
 */

#ifndef FRAMEBUFFER_HPP
#define FRAMEBUFFER_HPP

#include "Graphics/Texture.hpp"
#include "Graphics/Enums.hpp"

#include <vector>
#include <memory>
#include <optional>
#include <array>
#include <cstdint>
#include <variant>

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

struct Framebuffer
{
    struct Attachment
    {
        LoadAction loadAction = LoadAction::load;
        ClearValue clearValue = ClearValue::color({0.0f, 0.0f, 0.0f, 0.0f});
        std::shared_ptr<Texture> texture;
    };

    std::vector<Attachment> colorAttachments;
    std::optional<Attachment> depthAttachment;
};

}

#endif // FRAMEBUFFER_HPP
