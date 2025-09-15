/*
 * ---------------------------------------------------
 * Sync.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/07/12 09:26:01
 * ---------------------------------------------------
 */

#include "Vulkan/Sync.hpp"

namespace gfx
{

ext::optional<vk::MemoryBarrier2> syncResource(const ResourceSyncState& state, const ResourceSyncRequest& request)
{
    constexpr vk::AccessFlags2 writeMask = vk::AccessFlagBits2::eShaderWrite
        | vk::AccessFlagBits2::eTransferWrite
        | vk::AccessFlagBits2::eColorAttachmentWrite
        | vk::AccessFlagBits2::eDepthStencilAttachmentWrite
        | vk::AccessFlagBits2::eHostWrite
        | vk::AccessFlagBits2::eMemoryWrite;

    ext::optional<vk::MemoryBarrier2> memoryBarrier;

    const bool hadPrevUse = state.accessMask != vk::AccessFlags2{};
    const bool prevWrote = (state.accessMask & writeMask) != vk::AccessFlags2{};
    const bool nextWrites = (request.accessMask & writeMask) != vk::AccessFlags2{};
    if (hadPrevUse && (prevWrote || nextWrites))
    {
        memoryBarrier = vk::MemoryBarrier2{}
            .setSrcStageMask(state.stageMask)
            .setSrcAccessMask(state.accessMask)
            .setDstStageMask(request.stageMask)
            .setDstAccessMask(request.accessMask);
    }
    return memoryBarrier;
}

ResourceSyncState resourceStateAfterSync(const ResourceSyncRequest& request)
{
    return ResourceSyncState{
        .stageMask = request.stageMask,
        .accessMask = request.accessMask
    };
}

ext::optional<vk::ImageMemoryBarrier2> syncImage(ImageSyncState& state, const ImageSyncRequest& request)
{
    ext::optional<vk::MemoryBarrier2> memoryBarrier = syncResource(state, request);
    ext::optional<vk::ImageMemoryBarrier2> imageMemoryBarrier;
    if (memoryBarrier.has_value())
    {
        const vk::ImageLayout oldLayout = (request.layout == state.layout) ? state.layout : (request.preserveContent ? state.layout : vk::ImageLayout::eUndefined);

        imageMemoryBarrier = vk::ImageMemoryBarrier2{}
            .setSrcStageMask(memoryBarrier->srcStageMask)
            .setSrcAccessMask(memoryBarrier->srcAccessMask)
            .setDstStageMask(memoryBarrier->dstStageMask)
            .setDstAccessMask(memoryBarrier->dstAccessMask)
            .setOldLayout(oldLayout)
            .setNewLayout(request.layout)
            .setSrcQueueFamilyIndex(vk::QueueFamilyIgnored)
            .setDstQueueFamilyIndex(vk::QueueFamilyIgnored);
    }
    else if (state.layout != request.layout)
    {
        const vk::ImageLayout oldLayout = request.preserveContent ? state.layout : vk::ImageLayout::eUndefined;

        imageMemoryBarrier = vk::ImageMemoryBarrier2{}
            .setSrcStageMask(state.stageMask)
            .setSrcAccessMask(state.accessMask)
            .setDstStageMask(request.stageMask)
            .setDstAccessMask(request.accessMask)
            .setOldLayout(oldLayout)
            .setNewLayout(request.layout)
            .setSrcQueueFamilyIndex(vk::QueueFamilyIgnored)
            .setDstQueueFamilyIndex(vk::QueueFamilyIgnored);
    }
    state = imageStateAfterSync(request);
    return imageMemoryBarrier;
}

ImageSyncState imageStateAfterSync(const ImageSyncRequest& request)
{
    ImageSyncState newState(resourceStateAfterSync(request));
    newState.layout = request.layout;
    return newState;
}

ext::optional<vk::BufferMemoryBarrier2> syncBuffer(BufferSyncState& state, const BufferSyncRequest& request)
{
    ext::optional<vk::MemoryBarrier2> memoryBarrier = syncResource(state, request);
    ext::optional<vk::BufferMemoryBarrier2> bufferMemoryBarrier;
    if (memoryBarrier.has_value())
    {
        bufferMemoryBarrier = vk::BufferMemoryBarrier2{}
            .setSrcStageMask(memoryBarrier->srcStageMask)
            .setSrcAccessMask(memoryBarrier->srcAccessMask)
            .setDstStageMask(memoryBarrier->dstStageMask)
            .setDstAccessMask(memoryBarrier->dstAccessMask)
            .setSrcQueueFamilyIndex(vk::QueueFamilyIgnored)
            .setDstQueueFamilyIndex(vk::QueueFamilyIgnored);
    }
    state = bufferStateAfterSync(request);
    return bufferMemoryBarrier;
}

BufferSyncState bufferStateAfterSync(const BufferSyncRequest& request)
{
    BufferSyncState newState(resourceStateAfterSync(request));
    return newState;
}

}
