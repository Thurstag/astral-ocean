// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#include "staging_buffer.h"

ao::vulkan::StagingBuffer::StagingBuffer(std::weak_ptr<Device> device, vk::CommandBufferUsageFlags _usage, bool _memoryBarrier) : ao::vulkan::Buffer(device), usage(_usage), memoryBarrier(_memoryBarrier) {}

ao::vulkan::StagingBuffer::~StagingBuffer() {
	this->free();

	if (auto _device = ao::core::shared(this->device)) {
		_device->logical.freeCommandBuffers(_device->transferCommandPool, this->commandBuffer);
		_device->logical.destroyFence(this->fence);
	}
}

void ao::vulkan::StagingBuffer::free() {
	if (this->hostBuffer.get() != nullptr) {
		this->hostBuffer.reset();
	}
	if (this->deviceBuffer.get() != nullptr) {
		this->deviceBuffer.reset();
	}
}

vk::DeviceSize ao::vulkan::StagingBuffer::offset(size_t index) {
	return this->hostBuffer->offset(index);
}

ao::vulkan::Buffer* ao::vulkan::StagingBuffer::map() {
	return this->hostBuffer->map();
}

vk::Buffer & ao::vulkan::StagingBuffer::buffer() {
	if (this->deviceBuffer.get() == nullptr) {
		throw ao::core::Exception("Device buffer hasn't been initialized");
	}
	return this->deviceBuffer->buffer();
}

vk::DeviceSize ao::vulkan::StagingBuffer::size() {
	if (this->deviceBuffer.get() == nullptr) {
		throw ao::core::Exception("Device buffer hasn't been initialized");
	}
	return this->deviceBuffer->size();
}

bool ao::vulkan::StagingBuffer::hasBuffer() {
	if (this->hostBuffer.get() == nullptr || this->deviceBuffer.get() == nullptr) {
		return false;
	}
	return this->hostBuffer->hasBuffer() && this->deviceBuffer->hasBuffer();
}

void ao::vulkan::StagingBuffer::sync() {
	auto _device = ao::core::shared(this->device);

	// Create command to transfer data from host to device
	this->commandBuffer.begin(vk::CommandBufferBeginInfo(this->usage));
	{
		// Memory barrier
		if (this->memoryBarrier) {
			vk::BufferMemoryBarrier barrier(
				vk::AccessFlagBits::eTransferWrite,
				vk::AccessFlags(), _device->queues[vk::QueueFlagBits::eTransfer].index,
				_device->queues[vk::QueueFlagBits::eGraphics].index, this->deviceBuffer->buffer()
			);
			this->commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eVertexInput, vk::DependencyFlags(), {}, barrier, {});
		}

		// Copy buffer
		this->commandBuffer.copyBuffer(this->hostBuffer->buffer(), this->deviceBuffer->buffer(), vk::BufferCopy().setSize(this->deviceBuffer->size()));
	}
	this->commandBuffer.end();

	// Submit command
	_device->queues[vk::QueueFlagBits::eTransfer].queue.submit(vk::SubmitInfo().setCommandBufferCount(1)
															   .setPCommandBuffers(&this->commandBuffer),
															   this->fence);

	// Wait fence
	_device->logical.waitForFences(this->fence, VK_TRUE, (std::numeric_limits<u64>::max)());

	// Reset fence
	_device->logical.resetFences(this->fence);
}
