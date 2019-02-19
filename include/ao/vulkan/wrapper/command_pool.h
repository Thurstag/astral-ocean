// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <map>

#include <ao/core/utilities/types.h>
#include <vulkan/vulkan.hpp>

namespace ao::vulkan {
    /**
     * @brief Access mode to command pool
     *
     */
    enum class CommandPoolAccessModeFlagBits { eSequential, eConcurrent };

    /**
     * @brief vk::CommanPool wrapper
     *
     */
    class CommandPool {
       public:
        /**
         * @brief Construct a new CommandPool object
         *
         * @param device Device
         * @param flags Flags
         * @param queue_family_index Queue family index
         * @param access_mode Access mode
         */
        CommandPool(vk::Device device, vk::CommandPoolCreateFlags flags = vk::CommandPoolCreateFlags(), u32 queue_family_index = 0,
                    CommandPoolAccessModeFlagBits access_mode = CommandPoolAccessModeFlagBits::eSequential);

        /**
         * @brief Destroy the CommandPool object
         *
         */
        ~CommandPool();

        /**
         * @brief Allocate {count} command buffers
         *
         * @param level Level
         * @param count Count
         * @return std::vector<vk::CommandBuffer> Command buffers
         */
        std::vector<vk::CommandBuffer> allocateCommandBuffers(vk::CommandBufferLevel level, u32 count);

        /**
         * @brief Free a command buffer
         *
         * @param buffer Buffer
         */
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
