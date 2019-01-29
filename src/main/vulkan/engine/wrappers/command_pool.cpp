// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#include "command_pool.h"

#include <ao/core/utilities/pointers.h>

ao::vulkan::CommandPool::CommandPool(vk::Device device, vk::CommandPoolCreateFlags flags, u32 queue_family_index,
                                     CommandPoolAccessModeFlagBits access_mode)
    : device(device), create_flags(flags), queue_family_index(queue_family_index), access_mode(access_mode) {
    // Create unique command pool
    if (access_mode == ao::vulkan::CommandPoolAccessModeFlagBits::eSequential) {
        this->command_pools.push_back(device.createCommandPool(vk::CommandPoolCreateInfo(flags, queue_family_index)));
    }
}

ao::vulkan::CommandPool::~CommandPool() {
    // Destroy command buffers
    for (auto& [buffer, pool] : this->command_buffers) {
        this->device.freeCommandBuffers(pool, buffer);
    }

    // Destroy pools
    for (auto& pool : this->command_pools) {
        this->device.destroyCommandPool(pool);
    }
}

std::vector<vk::CommandBuffer> ao::vulkan::CommandPool::allocateCommandBuffers(vk::CommandBufferLevel level, u32 count) {
    // Choose a command pool
    vk::CommandPool command_pool;
    if (this->access_mode == ao::vulkan::CommandPoolAccessModeFlagBits::eConcurrent) {
        command_pool = this->device.createCommandPool(vk::CommandPoolCreateInfo(this->create_flags, queue_family_index));

        this->command_pools.push_back(command_pool);
    } else {
        command_pool = this->command_pools.front();
    }

    // Allocate
    auto buffers = this->device.allocateCommandBuffers(vk::CommandBufferAllocateInfo(command_pool, level, count));

    // Add
    for (auto& buffer : buffers) {
        this->command_buffers[buffer] = command_pool;
    }

    return buffers;
}

void ao::vulkan::CommandPool::freeCommandBuffers(vk::CommandBuffer buffer) {
    // Free command buffer
    this->device.freeCommandBuffers(this->command_buffers[buffer], buffer);

    // Destroy command pool
    if (this->access_mode == ao::vulkan::CommandPoolAccessModeFlagBits::eConcurrent) {
        this->device.destroyCommandPool(this->command_buffers[buffer]);

        auto it = std::find(this->command_pools.begin(), this->command_pools.end(), this->command_buffers[buffer]);
        if (it == this->command_pools.end()) {
            throw ao::core::Exception("Fail to find command pool");
        }
        this->command_pools.erase(it);
    }

    // Remove buffer
    this->command_buffers.erase(buffer);
}
