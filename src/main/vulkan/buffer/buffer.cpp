// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#include "buffer.h"

ao::vulkan::Buffer::Buffer(std::shared_ptr<Device> device) : device(device) {}

size_t ao::vulkan::Buffer::CalculateUBOAligmentSize(vk::PhysicalDevice device, size_t size) {
    size_t alignment = device.getProperties().limits.minUniformBufferOffsetAlignment;

    if (alignment > 0) {
        return (size + alignment - 1) & ~(alignment - 1);
    }
    return size;
}
