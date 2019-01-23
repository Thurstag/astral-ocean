// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#include "buffer.h"

ao::vulkan::Buffer::Buffer(std::weak_ptr<Device> device) : device(device) {}

size_t ao::vulkan::Buffer::calculateUBOAligmentSize(size_t objectSize) const {
    return ao::vulkan::Buffer::CalculateUBOAligmentSize(ao::core::shared(this->device)->physical, objectSize);
}

size_t ao::vulkan::Buffer::CalculateUBOAligmentSize(vk::PhysicalDevice const& device, size_t objectSize) {
    size_t minUboAlignment = device.getProperties().limits.minUniformBufferOffsetAlignment;

    if (minUboAlignment > 0) {
        return (objectSize + minUboAlignment - 1) & ~(minUboAlignment - 1);
    }
    return objectSize;
}
