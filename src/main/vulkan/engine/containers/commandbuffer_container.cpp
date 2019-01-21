// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#include "commandbuffer_container.h"

#include <ao/core/utilities/pointers.h>

ao::vulkan::CommandBufferContainer::CommandBufferContainer(std::weak_ptr<Device> _device) : device(_device) {}

ao::vulkan::CommandBufferContainer::~CommandBufferContainer() {
	if (!this->map.empty()) {
		this->clear();
	}
}

void ao::vulkan::CommandBufferContainer::clear() {
	if (auto _device = ao::core::shared(this->device)) {
		for (auto& pair : this->map) {
			_device->logical.freeCommandBuffers(pair.second.pool, pair.second.buffers);
		}
		this->map.clear();
	}
}
