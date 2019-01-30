// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include "buffer.h"

namespace ao::vulkan {
    class StagingBuffer : public Buffer {
       public:
        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="device">Device</param>
        /// <param name="usage_flags">Usage</param>
        /// <param name="memory_barrier">Bind memory barrier on transfer</param>
        StagingBuffer(std::weak_ptr<Device> device, vk::CommandBufferUsageFlags usage_flags = vk::CommandBufferUsageFlagBits::eSimultaneousUse,
                      bool memory_barrier = false);

        /// <summary>
        /// Destructor
        /// </summary>
        virtual ~StagingBuffer();

        /// <summary>
        /// Method to free buffer
        /// </summary>
        void free();

        /// <summary>
        /// Method to get offset of fragment at index
        /// </summary>
        /// <param name="index">Fragment index</param>
        /// <returns>Offset</returns>
        virtual vk::DeviceSize offset(size_t index) const;

        virtual Buffer* map() override;

        vk::Buffer buffer() override;
        vk::DeviceSize size() const override;
        bool hasBuffer() const override;

       protected:
        vk::CommandBufferUsageFlags cmd_usage;
        vk::CommandBuffer command_buffer;
        bool memory_barrier;
        vk::Fence fence;

        std::shared_ptr<Buffer> device_buffer;
        std::shared_ptr<Buffer> host_buffer;

        /// <summary>
        /// Method to synchronize host & device memories
        /// </summary>
        void sync();
    };
}  // namespace ao::vulkan
