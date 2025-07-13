/*
 * ---------------------------------------------------
 * Sync.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/07/12 09:26:01
 * ---------------------------------------------------
 */

#include "Vulkan/Sync.hpp"

#include <vulkan/vulkan.hpp>

#if defined(GFX_USE_UTILSCPP)
#else
    #include <optional>
    namespace ext = std;
#endif

namespace gfx
{

ext::optional<vk::ImageMemoryBarrier2> syncImage(ImageSyncState& state, const ImageSyncRequest& request)
{
    ext::optional<vk::ImageMemoryBarrier2> barrier;
    if (state.layout != request.layout)
    {
        barrier = vk::ImageMemoryBarrier2{}
            .setSrcStageMask(vk::PipelineStageFlagBits2::eAllCommands)
            .setSrcAccessMask(vk::AccessFlagBits2::eMemoryRead | vk::AccessFlagBits2::eMemoryWrite)
            .setDstStageMask(vk::PipelineStageFlagBits2::eAllCommands)
            .setDstAccessMask(vk::AccessFlagBits2::eMemoryRead | vk::AccessFlagBits2::eMemoryWrite)
            .setOldLayout(request.preserveContent ? state.layout : vk::ImageLayout::eUndefined)
            .setNewLayout(request.layout)
            .setSrcQueueFamilyIndex(vk::QueueFamilyIgnored)
            .setDstQueueFamilyIndex(vk::QueueFamilyIgnored);
    }
    state = imageStateAfterSync(request);
    return barrier;
}

ImageSyncState imageStateAfterSync(const ImageSyncRequest& request)
{
    return ImageSyncState{
        .layout = request.layout
    };
}

}
