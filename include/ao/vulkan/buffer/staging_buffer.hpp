// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include "../wrapper/fence.h"
#include "buffer.h"

namespace ao::vulkan {
    /**
     * @brief Staging buffer
     *
     * @tparam T Buffer type
     */
    template<class T>
    class StagingBuffer : public virtual Buffer {
       public:
        /**
         * @brief Construct a new StagingBuffer object
         *
         * @param device Device
         * @param usage_flags Usage flags
         * @param memory_barrier Memory barrier
         */
        StagingBuffer(std::shared_ptr<Device> device, vk::CommandBufferUsageFlags usage_flags = vk::CommandBufferUsageFlagBits::eSimultaneousUse,
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
        void freeHostBuffer() {
            this->host_buffer->free();
        }

        virtual Buffer* map() override {
            return this->host_buffer->map();
        }

        virtual vk::DeviceSize offset(size_t index) const override {
            return this->host_buffer->offset(index);
        }

        vk::Buffer buffer() override;
        vk::DeviceSize size() const override;
        bool hasBuffer() const override;
        void free() override;

       protected:
        vk::CommandBufferUsageFlags cmd_usage;
        vk::CommandBuffer command_buffer;
        bool memory_barrier;
        Fence fence;

        std::unique_ptr<T> device_buffer;
        std::unique_ptr<T> host_buffer;

        /**
         * @brief Synchronize host/device buffers
         *
         */
        void sync();
    };

    template<class T>
    StagingBuffer<T>::StagingBuffer(std::shared_ptr<Device> device, vk::CommandBufferUsageFlags cmd_usage, bool memory_barrier)
        : Buffer(device), cmd_usage(cmd_usage), memory_barrier(memory_barrier), fence(device->logical()), command_buffer(nullptr) {}

    template<class T>
    StagingBuffer<T>::~StagingBuffer() {
        this->free();

        if (this->command_buffer) {
            this->device->transferPool().freeCommandBuffers(this->command_buffer);
        }
    }

    template<class T>
    void StagingBuffer<T>::free() {
        if (this->host_buffer) {
            this->host_buffer.reset();
        }
        if (this->device_buffer) {
            this->device_buffer.reset();
        }
    }

    template<class T>
    vk::Buffer StagingBuffer<T>::buffer() {
        if (!this->device_buffer) {
            throw ao::vulkan::BufferUninitialized();
        }
        return this->device_buffer->buffer();
    }

    template<class T>
    vk::DeviceSize StagingBuffer<T>::size() const {
        if (!this->device_buffer) {
            throw ao::vulkan::BufferUninitialized();
        }
        return this->device_buffer->size();
    }

    template<class T>
    bool StagingBuffer<T>::hasBuffer() const {
        if (!this->device_buffer) {
            return false;
        }
        return this->device_buffer->hasBuffer();
    }

    template<class T>
    void StagingBuffer<T>::sync() {
        // Create command to transfer data from host to device
        this->command_buffer.begin(vk::CommandBufferBeginInfo(this->cmd_usage));
        {
            // Memory barrier
            if (this->memory_barrier) {
                vk::BufferMemoryBarrier barrier(vk::AccessFlagBits::eTransferWrite, vk::AccessFlags(),
                                                this->device->queues()->at(vk::to_string(vk::QueueFlagBits::eTransfer)).family_index,
                                                VK_QUEUE_FAMILY_IGNORED, this->device_buffer->buffer());
                this->command_buffer.pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eVertexInput,
                                                     vk::DependencyFlags(), {}, barrier, {});
            }

            // Copy buffer
            this->command_buffer.copyBuffer(this->host_buffer->buffer(), this->device_buffer->buffer(),
                                            vk::BufferCopy().setSize(this->device_buffer->size()));
        }
        this->command_buffer.end();

        // Reset fence
        this->fence.reset();

        // Submit command
        this->device->queues()->submit(vk::QueueFlagBits::eTransfer,
                                       vk::SubmitInfo().setCommandBufferCount(1).setPCommandBuffers(&this->command_buffer), this->fence);

        // Wait fence
        this->fence.wait();
    }
}  // namespace ao::vulkan
