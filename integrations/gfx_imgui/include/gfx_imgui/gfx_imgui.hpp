/*
 * ---------------------------------------------------
 * ImGui.hpp
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2026/05/09
 * ---------------------------------------------------
 */

#ifndef GRAPHICS_IMGUI_HPP
#define GRAPHICS_IMGUI_HPP

#include "Graphics/Enums.hpp"

#include <cstdint>
#include <memory>
#include <optional>
#include <vector>

struct ImDrawData;

namespace gfx
{

class CommandBuffer;
class Device;
class Texture;

namespace imgui
{

struct InitInfo
{
    std::vector<PixelFormat> colorAttachmentPixelFormats;
    std::optional<PixelFormat> depthAttachmentPixelFormat = std::nullopt;
    uint32_t minImageCount = 3;
    uint32_t imageCount = 3;
};

void init(const Device&, const InitInfo&);
void newFrame(const Device&);
void renderDrawData(CommandBuffer&, ImDrawData*);
void shutdown(Device&);

uint64_t initTextureId(Texture&);
std::optional<uint64_t> textureId(const Texture&);
void removeTextureId(Texture&);

} // namespace imgui

} // namespace gfx

#endif // GRAPHICS_IMGUI_HPP
