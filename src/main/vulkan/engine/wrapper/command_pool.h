// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <map>

#include <ao/core/utilities/types.h>
#include <vulkan/vulkan.hpp>

namespace ao::vulkan {
    /// <summary>
    /// Access mode to command pool
    /// </summary>
    enum class CommandPoolAccessModeFlagBits { eSequential, eConcurrent };

    /// <summary>
    /// vk::CommanPool wrapper
    /// </summary>
    class CommandPool {
       public:
        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="device">Device</param>
        /// <param name="flags">Create flags</param>
        /// <param name="queue_family_index">Queue family index</param>
        /// <param name="access_mode">Access mode</param>
        CommandPool(vk::Device device, vk::CommandPoolCreateFlags flags = vk::CommandPoolCreateFlags(), u32 queue_family_index = 0,
                    CommandPoolAccessModeFlagBits access_mode = CommandPoolAccessModeFlagBits::eSequential);

        /// <summary>
        /// Destructor
        /// </summary>
        ~CommandPool();

        /// <summary>
        /// Method to allocate command buffers
        /// </summary>
        /// <param name="level">Level</param>
        /// <param name="count">Count</param>
        /// <returns>Command buffers</returns>
        std::vector<vk::CommandBuffer> allocateCommandBuffers(vk::CommandBufferLevel level, u32 count);

        /// <summary>
        /// Method to free a command buffer
        /// </summary>
        /// <param name="buffer">Buffer</param>
        void freeCommandBuffers(vk::CommandBuffer buffer);

       protected:
        CommandPoolAccessModeFlagBits access_mode;
        vk::CommandPoolCreateFlags create_flags;
        u32 queue_family_index;
        vk::Device device;  // TODO: Change type when refactor Device struct

        std::map<vk::CommandBuffer, vk::CommandPool> command_buffers;
        std::vector<vk::CommandPool> command_pools;
    };
}  // namespace ao::vulkan
