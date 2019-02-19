// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include "buffer.h"

namespace ao::vulkan {
    /**
     * @brief Staging buffer
     *
     */
    class StagingBuffer : public Buffer {
       public:
        /**
         * @brief Construct a new StagingBuffer object
         *
         * @param device Device
         * @param usage_flags Usage flags
         * @param memory_barrier Memory barrier
         */
        StagingBuffer(std::weak_ptr<Device> device, vk::CommandBufferUsageFlags usage_flags = vk::CommandBufferUsageFlagBits::eSimultaneousUse,
                      bool memory_barrier = false);

        /**
         * @brief Destroy the StagingBuffer object
         *
         */
        virtual ~StagingBuffer();

        /**
         * @brief Free buffer on host side
         *
         */
        void freeHostBuffer();

        virtual vk::DeviceSize offset(size_t index) const override;
        virtual Buffer* map() override;
        vk::Buffer buffer() override;
        vk::DeviceSize size() const override;
        bool hasBuffer() const override;
        void free() override;

       protected:
        vk::CommandBufferUsageFlags cmd_usage;
        vk::CommandBuffer command_buffer;
        bool memory_barrier;
        vk::Fence fence;

        std::shared_ptr<Buffer> device_buffer;
        std::shared_ptr<Buffer> host_buffer;

        /**
         * @brief Synchronize host/device buffers
         *
         */
        void sync();
    };
}  // namespace ao::vulkan
