/*
 * ---------------------------------------------------
 * Sync.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/07/11 10:50:33
 * ---------------------------------------------------
 */

#ifndef SYNC_HPP
#define SYNC_HPP

namespace gfx
{

struct ResourceSyncRequest
{
    vk::PipelineStageFlags2 stageMask;
    vk::AccessFlags2 accessMask;
};

struct ResourceSyncState
{
    vk::PipelineStageFlags2 stageMask;
    vk::AccessFlags2 accessMask;
};

struct ImageSyncRequest : public ResourceSyncRequest
{
    vk::ImageLayout layout = vk::ImageLayout::eUndefined;
    bool preserveContent = true;
};

struct ImageSyncState : public ResourceSyncState
{
    vk::ImageLayout layout = vk::ImageLayout::eUndefined;
};

struct BufferSyncRequest : public ResourceSyncRequest { };

struct BufferSyncState : public ResourceSyncState { };

std::optional<vk::MemoryBarrier2> syncResource(const ResourceSyncState&, const ResourceSyncRequest&);
ResourceSyncState resourceStateAfterSync(const ResourceSyncRequest&);

std::optional<vk::ImageMemoryBarrier2> syncImage(ImageSyncState&, const ImageSyncRequest&);
ImageSyncState imageStateAfterSync(const ImageSyncRequest&);

std::optional<vk::BufferMemoryBarrier2> syncBuffer(BufferSyncState&, const BufferSyncRequest&);
BufferSyncState bufferStateAfterSync(const BufferSyncRequest&);

}

#endif // SYNC_HPP
