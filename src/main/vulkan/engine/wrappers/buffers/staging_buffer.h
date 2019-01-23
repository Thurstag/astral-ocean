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
        /// <param name="usage">Usage</param>
        /// <param name="memoryBarrier">Bind memory barrier on transfer</param>
        StagingBuffer(std::weak_ptr<Device> device, vk::CommandBufferUsageFlags usage = vk::CommandBufferUsageFlagBits::eSimultaneousUse,
                      bool memoryBarrier = false);

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

        vk::Buffer const& buffer() const override;
        vk::DeviceSize size() const override;
        bool hasBuffer() const override;

       protected:
        vk::CommandBufferUsageFlags usage;
        bool memory_barrier;

        std::shared_ptr<Buffer> device_buffer;
        std::shared_ptr<Buffer> host_buffer;

        vk::CommandBuffer command_buffer;
        vk::Fence fence;

        /// <summary>
        /// Method to synchronize host & device memories
        /// </summary>
        void sync();
    };
}  // namespace ao::vulkan
