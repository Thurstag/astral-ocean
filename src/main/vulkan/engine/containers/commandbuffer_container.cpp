// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#include "commandbuffer_container.h"

#include <ao/core/utilities/pointers.h>

ao::vulkan::CommandBufferContainer::CommandBufferContainer(std::weak_ptr<Device> device) : device(device) {}

ao::vulkan::CommandBufferContainer::~CommandBufferContainer() {
    if (!this->map.empty()) {
        this->clear();
    }
}

void ao::vulkan::CommandBufferContainer::clear() {
    auto _device = ao::core::shared(this->device);

    // Free command buffers
    for (auto& [key, value] : this->map) {
        _device->logical.freeCommandBuffers(value.pool, value.buffers);
    }
    this->map.clear();
}
