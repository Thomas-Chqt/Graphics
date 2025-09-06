/*
 * ---------------------------------------------------
 * imgui_impl_metal.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/08/30 14:23:22
 * ---------------------------------------------------
 */

#ifndef IMGUI_IMPL_METAL_HPP
#define IMGUI_IMPL_METAL_HPP

#include "Graphics/Enums.hpp"

#include "Metal/MetalDevice.hpp"

namespace gfx
{

void ImGui_ImplMetal_Init(const MetalDevice*, const ext::vector<PixelFormat>&, const ext::optional<PixelFormat>&);
void ImGui_ImplMetal_Shutdown();
void ImGui_ImplMetal_NewFrame();
void ImGui_ImplMetal_RenderDrawData(ImDrawData*, id<MTLCommandBuffer>, id<MTLRenderCommandEncoder>);

}

#endif // IMGUI_IMPL_METAL_HPP
