// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#include "device_allocator.h"

#include <ao/core/utilities/memory.h>

#include "../../exception/unknown_allocation.h"
#include "../../utilities/device.h"

ao::vulkan::DeviceAllocator::DeviceAllocator(std::shared_ptr<Device> device, vk::CommandBufferUsageFlags cmd_usage, size_t alignment)
    : ao::vulkan::Allocator(device), cmd_usage(cmd_usage), alignment(alignment) {}

ao::vulkan::DeviceAllocator::~DeviceAllocator() {
    for (auto& [key, allocation] : this->allocations) {
        // Host
        if (allocation.host.first.buffer != vk::Buffer()) {
            this->device->logical()->unmapMemory(allocation.host.second);
            this->device->logical()->destroyBuffer(allocation.host.first.buffer);
            this->device->logical()->freeMemory(allocation.host.second);
        }

        // Device
        this->device->logical()->destroyBuffer(allocation.device.first.buffer);
        this->device->logical()->freeMemory(allocation.device.second);

        // Others
        if (allocation.host.first.buffer != vk::Buffer()) {
            this->device->transferPool().freeCommandBuffers(allocation.command);
            allocation.fence.destroy();
        }
    }
}

void ao::vulkan::DeviceAllocator::invalidate(BufferInfo const& info, vk::DeviceSize const& offset, vk::DeviceSize const& size) {
    // Check info
    if (!this->own(info)) {
        throw ao::vulkan::UnknownAllocation();
    }

    auto& allocation = this->allocations[info.buffer];

    // Create command to transfer data from host to device
    allocation.command.begin(vk::CommandBufferBeginInfo(this->cmd_usage));
    allocation.command.copyBuffer(allocation.host.first.buffer, allocation.device.first.buffer, vk::BufferCopy(offset, offset, size));
    allocation.command.end();

    // Reset fence
    allocation.fence.reset();

    // Submit command
    this->device->queues()->submit(vk::QueueFlagBits::eTransfer, vk::SubmitInfo().setCommandBufferCount(1).setPCommandBuffers(&allocation.command),
                                   allocation.fence);

    // Wait fence
    allocation.fence.wait();
}

ao::vulkan::Allocator::BufferInfo ao::vulkan::DeviceAllocator::allocate(vk::DeviceSize size, vk::BufferUsageFlags usage) {
    ao::vulkan::DeviceAllocator::Allocation allocation;
    ao::vulkan::Allocator::BufferInfo info;

    // Host buffer
    {
        // Create buffer
        auto buffer = this->device->logical()->createBuffer(
            vk::BufferCreateInfo(vk::BufferCreateFlags(), size, vk::BufferUsageFlagBits::eTransferSrc | usage, vk::SharingMode::eExclusive));

        // Get memory requirements
        auto mem_requirements = this->device->logical()->getBufferMemoryRequirements(buffer);

        // Allocate memory
        auto memory = this->device->logical()->allocateMemory(vk::MemoryAllocateInfo(
            mem_requirements.size,
            ao::vulkan::utilities::memoryType(this->device->physical(), mem_requirements.memoryTypeBits,
                                              vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent)));

        // Bind memory and buffer
        this->device->logical()->bindBufferMemory(buffer, memory, 0);

        // Map memory
        info.ptr = this->device->logical()->mapMemory(memory, 0, mem_requirements.size);

        // Set size
        info.size = mem_requirements.size;

        // Fill allocation
        allocation.host = std::make_pair(ao::vulkan::Allocator::BufferInfo(info.size, buffer, info.ptr), memory);
    }

    // Device buffer
    {
        // Create buffer
        info.buffer = this->device->logical()->createBuffer(
            vk::BufferCreateInfo(vk::BufferCreateFlags(), size, vk::BufferUsageFlagBits::eTransferDst | usage, vk::SharingMode::eExclusive));

        // Get memory requirements
        auto mem_requirements = this->device->logical()->getBufferMemoryRequirements(info.buffer);

        // Allocate memory
        auto memory = this->device->logical()->allocateMemory(vk::MemoryAllocateInfo(
            mem_requirements.size,
            ao::vulkan::utilities::memoryType(this->device->physical(), mem_requirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal)));

        // Bind memory and buffer
        this->device->logical()->bindBufferMemory(info.buffer, memory, 0);

        // Fill allocation
        allocation.device = std::make_pair(ao::vulkan::Allocator::BufferInfo(mem_requirements.size, info.buffer), memory);
    }

    // Create command buffer
    allocation.command = this->device->transferPool().allocateCommandBuffers(vk::CommandBufferLevel::ePrimary, 1).front();

    // Create fence
    allocation.fence = ao::vulkan::Fence(this->device->logical());

    // Add to allocations
    this->allocations_mutex.lock();
    this->allocations[info.buffer] = allocation;
    this->allocations_mutex.unlock();

    return info;
}

void ao::vulkan::DeviceAllocator::free(Allocator::BufferInfo const& info) {
    // Check info
    if (!this->own(info)) {
        throw ao::vulkan::UnknownAllocation();
    }

    // Free
    auto& allocation = this->allocations[info.buffer];
    if (allocation.host.first.buffer != vk::Buffer()) {  // Host
        this->device->logical()->unmapMemory(allocation.host.second);
        this->device->logical()->destroyBuffer(allocation.host.first.buffer);
        this->device->logical()->freeMemory(allocation.host.second);
    }
    this->device->logical()->destroyBuffer(allocation.device.first.buffer);  // Device
    this->device->logical()->freeMemory(allocation.device.second);
    if (allocation.host.first.buffer != vk::Buffer()) {  // Others
        this->device->transferPool().freeCommandBuffers(allocation.command);
        allocation.fence.destroy();
    }

    // Remove from allocations
    this->allocations_mutex.lock();
    this->allocations.erase(info.buffer);
    this->allocations_mutex.unlock();
}

size_t ao::vulkan::DeviceAllocator::alignSize(size_t size) const {
    return ao::core::utilities::calculateAligmentSize(size, this->alignment);
}

vk::DeviceSize ao::vulkan::DeviceAllocator::size() const {
    return std::accumulate(this->allocations.begin(), this->allocations.end(), vk::DeviceSize(0),
                           [](vk::DeviceSize result, auto pair) { return result + pair.second.host.first.size + pair.second.device.first.size; });
}

vk::DeviceSize ao::vulkan::DeviceAllocator::sizeOnDevice() const {
    return std::accumulate(this->allocations.begin(), this->allocations.end(), vk::DeviceSize(0),
                           [](vk::DeviceSize result, auto pair) { return result + pair.second.device.first.size; });
}

vk::DeviceSize ao::vulkan::DeviceAllocator::sizeOnHost() const {
    return std::accumulate(this->allocations.begin(), this->allocations.end(), vk::DeviceSize(0),
                           [](vk::DeviceSize result, auto pair) { return result + pair.second.host.first.size; });
}

bool ao::vulkan::DeviceAllocator::own(ao::vulkan::Allocator::BufferInfo const& info) const {
    return this->allocations.count(info.buffer) != 0;
}

void ao::vulkan::DeviceAllocator::freeHost(ao::vulkan::Allocator::BufferInfo const& info) {
    // Check info
    if (!this->own(info)) {
        throw ao::vulkan::UnknownAllocation();
    }

    // Free
    auto& allocation = this->allocations[info.buffer];
    this->device->logical()->unmapMemory(allocation.host.second);  // Host
    this->device->logical()->destroyBuffer(allocation.host.first.buffer);
    this->device->logical()->freeMemory(allocation.host.second);
    this->device->transferPool().freeCommandBuffers(allocation.command);  // Others
    allocation.fence.destroy();

    // Indicate that host buffer is destroyed
    allocation.host.first.buffer = vk::Buffer();
    allocation.host.first.ptr = nullptr;
    allocation.host.first.size = 0;
}