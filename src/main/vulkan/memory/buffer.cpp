// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#include "buffer.h"

ao::vulkan::Buffer::Buffer(std::shared_ptr<Allocator> allocator, vk::DeviceSize size, vk::BufferUsageFlags usage) : allocator_(allocator) {
    this->buffer_info = std::make_unique<ao::vulkan::Allocator::BufferInfo>(allocator->allocate(size, usage));
}