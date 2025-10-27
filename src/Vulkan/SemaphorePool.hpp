/*
 * ---------------------------------------------------
 * SemaphorePool.hpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/08/16 20:35:44
 * ---------------------------------------------------
 */

#ifndef SEMAPHOREPOOL_HPP
#define SEMAPHOREPOOL_HPP

namespace gfx
{

class VulkanDevice;

class SemaphorePool
{
public:
    SemaphorePool() = default;
    SemaphorePool(const SemaphorePool&) = delete;
    SemaphorePool(SemaphorePool&&) noexcept;

    SemaphorePool(const VulkanDevice*);

    const vk::Semaphore* get();
    void swapPools(); // swap back and front pools
    void reset(); // reset the back pool
    void clear();

    ~SemaphorePool();

private:
    struct PoolData
    {
        std::deque<vk::Semaphore> free;
        std::deque<vk::Semaphore> inUse;
    };

    const VulkanDevice* m_device = nullptr;

    std::array<PoolData, 2> m_pools;

    PoolData* m_frontPool = &m_pools[0];
    PoolData* m_backPool = &m_pools[1];

public:
    SemaphorePool& operator=(const SemaphorePool&) = delete;
    SemaphorePool& operator=(SemaphorePool&&) noexcept;
};

} // namespace gfx

#endif // SEMAPHOREPOOL_HPP
