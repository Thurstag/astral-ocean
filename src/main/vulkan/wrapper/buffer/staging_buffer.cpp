// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#include "staging_buffer.h"

#include "../../exception/buffer_unitialized.h"

ao::vulkan::StagingBuffer::StagingBuffer(std::weak_ptr<Device> device, vk::CommandBufferUsageFlags cmd_usage, bool memory_barrier)
    : ao::vulkan::Buffer(device), cmd_usage(cmd_usage), memory_barrier(memory_barrier) {}

ao::vulkan::StagingBuffer::~StagingBuffer() {
    auto _device = ao::core::shared(this->device);
    this->free();

    if (this->command_buffer && _device->transfer_command_pool) {
        _device->transfer_command_pool->freeCommandBuffers(this->command_buffer);
    }
    if (this->fence) {
        _device->logical.destroyFence(this->fence);
    }
}

void ao::vulkan::StagingBuffer::free() {
    if (this->host_buffer.get() != nullptr) {
        this->host_buffer.reset();
    }
    if (this->device_buffer.get() != nullptr) {
        this->device_buffer.reset();
    }
}

vk::DeviceSize ao::vulkan::StagingBuffer::offset(size_t index) const {
    return this->host_buffer->offset(index);
}

ao::vulkan::Buffer* ao::vulkan::StagingBuffer::map() {
    return this->host_buffer->map();
}

vk::Buffer ao::vulkan::StagingBuffer::buffer() {
    if (this->device_buffer.get() == nullptr) {
        throw ao::vulkan::BufferUninitialized();
    }
    return this->device_buffer->buffer();
}

vk::DeviceSize ao::vulkan::StagingBuffer::size() const {
    if (this->device_buffer.get() == nullptr) {
        throw ao::vulkan::BufferUninitialized();
    }
    return this->device_buffer->size();
}

bool ao::vulkan::StagingBuffer::hasBuffer() const {
    if (this->host_buffer.get() == nullptr || this->device_buffer.get() == nullptr) {
        return false;
    }
    return this->host_buffer->hasBuffer() && this->device_buffer->hasBuffer();
}

void ao::vulkan::StagingBuffer::sync() {
    auto _device = ao::core::shared(this->device);

    // Create command to transfer data from host to device
    this->command_buffer.begin(vk::CommandBufferBeginInfo(this->cmd_usage));
    {
        // Memory barrier
        if (this->memory_barrier) {
            vk::BufferMemoryBarrier barrier(vk::AccessFlagBits::eTransferWrite, vk::AccessFlags(),
                                            _device->queues[vk::to_string(vk::QueueFlagBits::eTransfer)].family_index,
                                            _device->queues[vk::to_string(vk::QueueFlagBits::eGraphics)].family_index, this->device_buffer->buffer());
            this->command_buffer.pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eVertexInput,
                                                 vk::DependencyFlags(), {}, barrier, {});
        }

        // Copy buffer
        this->command_buffer.copyBuffer(this->host_buffer->buffer(), this->device_buffer->buffer(),
                                        vk::BufferCopy().setSize(this->device_buffer->size()));
    }
    this->command_buffer.end();

    // Reset fence
    _device->logical.resetFences(this->fence);

    // Submit command
    _device->queues.submit(vk::QueueFlagBits::eTransfer, vk::SubmitInfo().setCommandBufferCount(1).setPCommandBuffers(&this->command_buffer),
                           this->fence);

    // Wait fence
    _device->logical.waitForFences(this->fence, VK_TRUE, (std::numeric_limits<u64>::max)());
}
