// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <map>
#include <mutex>

#include <ao/core/utilities/types.h>
#include <vulkan/vulkan.hpp>

namespace ao::vulkan {
    /**
     * @brief Access mode to command pool
     *
     */
    enum class CommandPoolAccessMode { eSequential, eConcurrent };

    /**
     * @brief vk::CommanPool wrapper
     *
     * TODO: Disable copy
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
        CommandPool(std::shared_ptr<vk::Device> device, vk::CommandPoolCreateFlags flags = vk::CommandPoolCreateFlags(), u32 queue_family_index = 0,
                    CommandPoolAccessMode access_mode = CommandPoolAccessMode::eSequential);

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
         * @brief Free command buffers
         *
         * @param buffers Buffers
         */
        void freeCommandBuffers(vk::ArrayProxy<vk::CommandBuffer const> buffers);

       protected:
        std::shared_ptr<vk::Device> device;
        std::mutex mutex;

        vk::CommandPoolCreateFlags create_flags;
        CommandPoolAccessMode access_mode;
        u32 queue_family_index;

        std::map<vk::CommandBuffer, vk::CommandPool> command_buffers;
        std::vector<vk::CommandPool> command_pools;

        /**
         * @brief Free a command buffer
         *
         * @param buffer Buffer
         */
        void freeCommandBuffer(vk::CommandBuffer buffer);
    };
}  // namespace ao::vulkan
