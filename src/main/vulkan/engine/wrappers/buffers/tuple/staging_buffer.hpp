// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include "../staging_buffer.h"
#include "basic_buffer.hpp"

namespace ao::vulkan {
	template<class... T>
	class StagingTupleBuffer : public virtual TupleBuffer<T...>, public virtual StagingBuffer {
	public:
		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="device">Device</param>
		/// <param name="_usage">Usage</param>
		/// <param name="_memoryBarrier">Bind memory barrier on transfer</param>
		StagingTupleBuffer(std::weak_ptr<Device> device, vk::CommandBufferUsageFlags const _usage = vk::CommandBufferUsageFlagBits::eSimultaneousUse, bool const _memoryBarrier = false);

		/// <summary>
		/// Destructor
		/// </summary>
		virtual ~StagingTupleBuffer() = default;

		/// <summary>
		/// Method to init buffer
		/// 
		/// If object already stores a buffer, it will free the old one 
		/// </summary>
		/// <param name="sizes">Fragment sizes</param>
		/// <param name="usageFlags">Usage flags</param>
		/// <returns>This</returns>
		StagingTupleBuffer<T...>* init(std::initializer_list<vk::DeviceSize> const& sizes, std::optional<vk::BufferUsageFlags> const& usageFlags = std::nullopt);

		TupleBuffer<T...>* update(T const*... data) override;
		TupleBuffer<T...>* updateFragment(std::size_t const index, void* const data) override;
		bool hasBuffer() const override;
		vk::Buffer const& buffer() const override;
		vk::DeviceSize size() const override;
		vk::DeviceSize offset(size_t const index) const override;
		Buffer* map() override;
	};

	/* IMPLEMENTATION */

	template<class ...T>
	StagingTupleBuffer<T...>::StagingTupleBuffer(std::weak_ptr<Device> device, vk::CommandBufferUsageFlags const _usage, bool _memoryBarrier) : TupleBuffer<T...>(device), StagingBuffer(device, _usage, _memoryBarrier) {}

	template<class ...T>
	StagingTupleBuffer<T...>* StagingTupleBuffer<T...>::init(std::initializer_list<vk::DeviceSize> const& sizes, std::optional<vk::BufferUsageFlags> const& usageFlags) {
		if (this->hasBuffer()) {
			this->free();
		}

		// Init buffer in host's memory
		this->hostBuffer = std::shared_ptr<TupleBuffer<T...>>(
			(new BasicTupleBuffer<T...>(StagingBuffer::device))
			->init(vk::BufferUsageFlagBits::eTransferSrc, vk::SharingMode::eExclusive,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, sizes)
		);

				// Init buffer in device's memory
		this->deviceBuffer = std::shared_ptr<TupleBuffer<T...>>(
			(new BasicTupleBuffer<T...>(StagingBuffer::device))
			->init(usageFlags ? vk::BufferUsageFlagBits::eTransferDst | usageFlags.value() : vk::BufferUsageFlagBits::eTransferDst,
			vk::SharingMode::eExclusive, vk::MemoryPropertyFlagBits::eDeviceLocal, sizes)
		);

		if (auto _device = ao::core::shared(StagingBuffer::device)) {
			// Create command buffer
			this->commandBuffer = _device->logical.allocateCommandBuffers(vk::CommandBufferAllocateInfo(_device->transferCommandPool, vk::CommandBufferLevel::ePrimary, 1))[0];

			// Create fence
			this->fence = _device->logical.createFence(vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled));
		}
		return this;
	}

	template<class ...T>
	TupleBuffer<T...>* StagingTupleBuffer<T...>::update(T const*... data) {
		if (!this->hasBuffer()) {
			throw core::Exception("Buffer hasn't been initialized");
		}

		// Update host buffer & synchronize memories
		if (auto host = static_cast<TupleBuffer<T...>*>(this->hostBuffer.get())) {
			host->update(data...);
		}
		else {
			throw core::Exception("Fail to update host buffer");
		}
		this->sync();

		return this;
	}

	template<class ...T>
	TupleBuffer<T...>* StagingTupleBuffer<T...>::updateFragment(std::size_t const index, void* const data) {
		if (!this->hasBuffer()) {
			throw core::Exception("Buffer hasn't been initialized");
		}

		// Update host buffer & synchronize memories
		if (auto host = static_cast<TupleBuffer<T...>*>(this->hostBuffer.get())) {
			host->updateFragment(index, data);
		}
		else {
			throw core::Exception(fmt::format("Fail to update host buffer fragment: {0}", index));
		}
		this->sync();

		return this;
	}

	template<class ...T>
	bool StagingTupleBuffer<T...>::hasBuffer() const {
		return StagingBuffer::hasBuffer();
	}

	template<class ...T>
	vk::Buffer const& StagingTupleBuffer<T...>::buffer() const {
		return StagingBuffer::buffer();
	}

	template<class ...T>
	vk::DeviceSize StagingTupleBuffer<T...>::size() const {
		return StagingBuffer::size();
	}

	template<class ...T>
	vk::DeviceSize StagingTupleBuffer<T...>::offset(size_t index) const {
		return StagingBuffer::offset(index);
	}

	template<class ...T>
	Buffer * StagingTupleBuffer<T...>::map() {
		return StagingBuffer::map();
	}
}
