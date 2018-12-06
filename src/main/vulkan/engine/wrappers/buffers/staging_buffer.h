// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include "buffer.h"

namespace ao::vulkan {
	class StagingBuffer : public Buffer {
	public:
		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="device">Device</param>
		/// <param name="_usage">Usage</param>
		/// <param name="_memoryBarrier">Bind memory barrier on transfer</param>
		StagingBuffer(std::weak_ptr<Device> device, vk::CommandBufferUsageFlags const _usage = vk::CommandBufferUsageFlagBits::eSimultaneousUse, bool const _memoryBarrier = false);

		/// <summary>
		/// Destructor
		/// </summary>
		virtual ~StagingBuffer();

		/// <summary>
		/// Method to free buffer
		/// </summary>
		void free();

		/// <summary>
		/// Method to get offset of fragment at index
		/// </summary>
		/// <param name="index">Fragment index</param>
		/// <returns>Offset</returns>
		virtual vk::DeviceSize offset(size_t const index) const;

		virtual Buffer* map() override;

		vk::Buffer const& buffer() const override;
		vk::DeviceSize size() const override;
		bool hasBuffer() const override;

	protected:
		vk::CommandBufferUsageFlags usage;
		bool memoryBarrier;

		std::shared_ptr<Buffer> deviceBuffer;
		std::shared_ptr<Buffer> hostBuffer;

		vk::CommandBuffer commandBuffer;
		vk::Fence fence;

		/// <summary>
		/// Method to synchronize host & device memories
		/// </summary>
		void sync();
	};
}
