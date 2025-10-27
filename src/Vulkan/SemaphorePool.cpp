/*
 * ---------------------------------------------------
 * SemaphorePool.cpp
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/08/16 20:46:52
 * ---------------------------------------------------
 */

#include "Vulkan/SemaphorePool.hpp"
#include "Vulkan/VulkanDevice.hpp"

namespace gfx
{

SemaphorePool::SemaphorePool(SemaphorePool&& other) noexcept
    : m_device(std::exchange(other.m_device, nullptr)),
      m_pools(std::move(other.m_pools))
{
    other.m_frontPool = &other.m_pools[0];
    other.m_backPool = &other.m_pools[1];
}

SemaphorePool::SemaphorePool(const VulkanDevice* device)
    : m_device(device)
{
}

const vk::Semaphore* SemaphorePool::get()
{
    assert(m_device);
    vk::Semaphore semaphore;
    if (m_frontPool->free.empty() == false) {
        semaphore = std::move(m_frontPool->free.front());
        m_frontPool->free.pop_front();
    } else {
        semaphore = m_device->vkDevice().createSemaphore(vk::SemaphoreCreateInfo{});
    }
    return &m_frontPool->inUse.emplace_back(std::move(semaphore));
}

void SemaphorePool::swapPools()
{
    std::swap(m_frontPool, m_backPool);
}

void SemaphorePool::reset()
{
    m_backPool->free.append_range(m_backPool->inUse);
    m_backPool->inUse.clear();
}

void SemaphorePool::clear()
{
    for (auto& pool : m_pools)
    {
        for (auto& s : pool.inUse) {
            assert(m_device);
            m_device->vkDevice().destroySemaphore(s);
        }
        pool.inUse.clear();

        for (auto& s : pool.free) {
            assert(m_device);
            m_device->vkDevice().destroySemaphore(s);
        }
        pool.free.clear();
    }
    m_device = nullptr;
}

SemaphorePool::~SemaphorePool()
{
    clear();
}

SemaphorePool& SemaphorePool::operator=(SemaphorePool&& other) noexcept
{
    if (this != &other)
    {
        clear();
        m_device = std::exchange(other.m_device, nullptr);
        m_pools = std::move(other.m_pools);

        other.m_frontPool = &other.m_pools[0];
        other.m_backPool = &other.m_pools[1];
    }
    return *this;
}


}
