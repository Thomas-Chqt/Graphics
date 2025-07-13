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

#include <vulkan/vulkan.hpp>

#if defined(GFX_USE_UTILSCPP)
#else
    #include <optional>
    namespace ext = std;
#endif

namespace gfx
{

struct ImageSyncRequest
{
    vk::ImageLayout layout;
    bool preserveContent;
};

struct ImageSyncState
{
    vk::ImageLayout layout;
};

ext::optional<vk::ImageMemoryBarrier2> syncImage(ImageSyncState&, const ImageSyncRequest&);
ImageSyncState imageStateAfterSync(const ImageSyncRequest&);

}

#endif // SYNC_HPP
