// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#include "host_allocator.h"

#include <ao/core/utilities/memory.h>

#include "../../exception/unknown_allocation.h"
#include "../../utilities/device.h"

ao::vulkan::HostAllocator::HostAllocator(std::shared_ptr<ao::vulkan::Device> device, size_t alignment)
    : ao::vulkan::Allocator::Allocator(device), alignment(alignment) {}

ao::vulkan::HostAllocator::~HostAllocator() {
    for (auto& [key, allocation] : this->allocations) {
        this->device->logical()->unmapMemory(allocation.second);
        this->device->logical()->destroyBuffer(key);
        this->device->logical()->freeMemory(allocation.second);
    }
}

ao::vulkan::Allocator::BufferInfo ao::vulkan::HostAllocator::allocate(vk::DeviceSize size, vk::BufferUsageFlags usage) {
    // Create buffer
    auto buffer = this->device->logical()->createBuffer(vk::BufferCreateInfo(vk::BufferCreateFlags(), size, usage, vk::SharingMode::eExclusive));

    // Get memory requirements
    auto mem_requirements = this->device->logical()->getBufferMemoryRequirements(buffer);

    // Allocate memory
    auto memory = this->device->logical()->allocateMemory(vk::MemoryAllocateInfo(
        mem_requirements.size,
        ao::vulkan::utilities::memoryType(this->device->physical(), mem_requirements.memoryTypeBits,
                                          vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCached)));

    // Bind memory and buffer
    this->device->logical()->bindBufferMemory(buffer, memory, 0);

    // Map memory
    auto ptr = this->device->logical()->mapMemory(memory, 0, mem_requirements.size);

    // Add to allocations
    auto buffer_info = ao::vulkan::Allocator::BufferInfo(mem_requirements.size, buffer, ptr);
    this->allocations_mutex.lock();
    this->allocations[buffer] = std::make_pair(buffer_info, memory);
    this->allocations_mutex.unlock();

    return buffer_info;
}

void ao::vulkan::HostAllocator::invalidate(ao::vulkan::Allocator::BufferInfo const& info, vk::DeviceSize const& offset, vk::DeviceSize const& size) {
    // Check info
    if (!this->own(info)) {
        throw ao::vulkan::UnknownAllocation();
    }

    // Flush memory
    this->device->logical()->flushMappedMemoryRanges(vk::MappedMemoryRange(this->allocations[info.buffer].second, offset, size));
}

void ao::vulkan::HostAllocator::free(Allocator::BufferInfo const& info) {
    // Check info
    if (!this->own(info)) {
        throw ao::vulkan::UnknownAllocation();
    }

    // Free
    auto& allocation = this->allocations[info.buffer];
    this->device->logical()->unmapMemory(allocation.second);
    this->device->logical()->destroyBuffer(allocation.first.buffer);
    this->device->logical()->freeMemory(allocation.second);

    // Remove from allocations
    this->allocations_mutex.lock();
    this->allocations.erase(info.buffer);
    this->allocations_mutex.unlock();
}

size_t ao::vulkan::HostAllocator::alignSize(size_t size) const {
    return ao::core::utilities::calculateAligmentSize(size, this->alignment);
}

vk::DeviceSize ao::vulkan::HostAllocator::size() const {
    return std::accumulate(this->allocations.begin(), this->allocations.end(), vk::DeviceSize(0),
                           [](vk::DeviceSize result, auto pair) { return result + pair.second.first.size; });
}

bool ao::vulkan::HostAllocator::own(ao::vulkan::Allocator::BufferInfo const& info) const {
    return this->allocations.count(info.buffer) != 0;
}