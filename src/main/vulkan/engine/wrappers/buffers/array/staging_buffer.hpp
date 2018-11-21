#pragma once

#include "../staging_buffer.h"
#include "basic_buffer.hpp"

namespace ao {
	namespace vulkan {
		template<class T>
		class StagingDynamicArrayBuffer : public virtual DynamicArrayBuffer<T>, public virtual StagingBuffer {
		public:
			/// <summary>
			/// Constructor
			/// </summary>
			/// <param name="device">Device</param>
			/// <param name="_usage">Usage</param>
			/// <param name="_memoryBarrier">Bind memory barrier on transfer</param>
			StagingDynamicArrayBuffer(std::weak_ptr<Device> device, vk::CommandBufferUsageFlags _usage = vk::CommandBufferUsageFlagBits::eSimultaneousUse, bool _memoryBarrier = false);

			/// <summary>
			/// Destructor
			/// </summary>
			virtual ~StagingDynamicArrayBuffer() = default;

			/// <summary>
			/// Method to init buffer
			/// 
			/// If object already stores a buffer, it will free the old one 
			/// </summary>
			/// <param name="size">Fragment size</param>
			/// <param name="usageFlags">Usage flags</param>
			/// <returns>This</returns>
			StagingDynamicArrayBuffer<T>* init(vk::DeviceSize size, boost::optional<vk::BufferUsageFlags> usageFlags = boost::none);

			DynamicArrayBuffer<T>* update(std::vector<T>& data) override;
			DynamicArrayBuffer<T>* updateFragment(std::size_t index, T* data) override;
			bool hasBuffer() override;
			vk::Buffer& buffer() override;
			vk::DeviceSize size() override;
			vk::DeviceSize offset(size_t index) override;
			Buffer* map() override;
		};

		/* IMPLEMENTATION */

		template<class T>
		StagingDynamicArrayBuffer<T>::StagingDynamicArrayBuffer(std::weak_ptr<Device> device, vk::CommandBufferUsageFlags _usage, bool _memoryBarrier) : DynamicArrayBuffer<T>(device), StagingBuffer(device, _usage, _memoryBarrier) {}

		template<class T>
		StagingDynamicArrayBuffer<T>* StagingDynamicArrayBuffer<T>::init(vk::DeviceSize size, boost::optional<vk::BufferUsageFlags> usageFlags) {
			if (this->hasBuffer()) {
				this->free();
			}

			// Init buffer in host's memory
			this->hostBuffer = std::unique_ptr<DynamicArrayBuffer<T>>(
				(new BasicDynamicArrayBuffer<T>(this->count, StagingBuffer::device))
				->init(vk::BufferUsageFlagBits::eTransferSrc, vk::SharingMode::eExclusive,
				vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, size)
			);

			// Init buffer in device's memory
			this->deviceBuffer = std::unique_ptr<DynamicArrayBuffer<T>>(
				(new BasicDynamicArrayBuffer<T>(this->count, StagingBuffer::device))
				->init(usageFlags ? vk::BufferUsageFlagBits::eTransferDst | usageFlags.get() : vk::BufferUsageFlagBits::eTransferDst,
				vk::SharingMode::eExclusive, vk::MemoryPropertyFlagBits::eDeviceLocal, size)
			);

			if (auto _device = ao::core::shared(StagingBuffer::device)) {
				// Create command buffer
				this->commandBuffer = _device->logical.allocateCommandBuffers(vk::CommandBufferAllocateInfo(_device->transferCommandPool, vk::CommandBufferLevel::ePrimary, 1))[0];

				// Create fence
				this->fence = _device->logical.createFence(vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled));
			}
			return this;
		}

		template<class T>
		DynamicArrayBuffer<T>* StagingDynamicArrayBuffer<T>::update(std::vector<T>& data) {
			if (!this->hasBuffer()) {
				throw core::Exception("Buffer hasn't been initialized");
			}

			// Update host buffer & synchronize memories
			this->hostBuffer->update(data);
			this->sync();

			return this;
		}

		template<class T>
		DynamicArrayBuffer<T>* StagingDynamicArrayBuffer<T>::updateFragment(std::size_t index, T * data) {
			if (!this->hasBuffer()) {
				throw core::Exception("Buffer hasn't been initialized");
			}

			// Update host buffer & synchronize memories
			this->hostBuffer->updateFragment(index, data);
			this->sync();

			return this;
		}

		template<class T>
		bool StagingDynamicArrayBuffer<T>::hasBuffer() {
			return StagingBuffer::hasBuffer();
		}

		template<class T>
		vk::Buffer & StagingDynamicArrayBuffer<T>::buffer() {
			return StagingBuffer::buffer();
		}

		template<class T>
		vk::DeviceSize StagingDynamicArrayBuffer<T>::size() {
			return StagingBuffer::size();
		}

		template<class T>
		vk::DeviceSize StagingDynamicArrayBuffer<T>::offset(size_t index) {
			return StagingBuffer::offset(index);
		}

		template<class T>
		Buffer * StagingDynamicArrayBuffer<T>::map() {
			return StagingBuffer::map();
		}

		template<class T, size_t N>
		class StagingArrayBuffer : public virtual ArrayBuffer<T, N>, public StagingBuffer {
		public:
			/// <summary>
			/// Constructor
			/// </summary>
			/// <param name="device">Device</param>
			/// <param name="_usage">Usage</param>
			/// <param name="_memoryBarrier">Bind memory barrier on transfer</param>
			StagingArrayBuffer(std::weak_ptr<Device> device, vk::CommandBufferUsageFlags _usage = vk::CommandBufferUsageFlagBits::eSimultaneousUse, bool _memoryBarrier = false);

			/// <summary>
			/// Destructor
			/// </summary>
			virtual ~StagingArrayBuffer() = default;

			/// <summary>
			/// Method to init buffer
			/// 
			/// If object already stores a buffer, it will free the old one 
			/// </summary>
			/// <param name="size">Fragment size</param>
			/// <param name="usageFlags">Usage flags</param>
			/// <returns>This</returns>
			StagingArrayBuffer<T, N>* init(vk::DeviceSize size, boost::optional<vk::BufferUsageFlags> usageFlags = boost::none);

			ArrayBuffer<T, N>* update(std::array<T, N> data) override;
			ArrayBuffer<T, N>* updateFragment(std::size_t index, T* data) override;
			bool hasBuffer() override;
			vk::Buffer& buffer() override;
			vk::DeviceSize size() override;
			vk::DeviceSize offset(size_t index) override;
			Buffer* map() override;
		};

		/* IMPLEMENTATION */

		template<class T, size_t N>
		StagingArrayBuffer<T, N>::StagingArrayBuffer(std::weak_ptr<Device> device, vk::CommandBufferUsageFlags _usage, bool _memoryBarrier) : ArrayBuffer<T, N>(device), StagingBuffer(device, _usage, _memoryBarrier) {}

		template<class T, size_t N>
		StagingArrayBuffer<T, N>* StagingArrayBuffer<T, N>::init(vk::DeviceSize size, boost::optional<vk::BufferUsageFlags> usageFlags) {
			if (this->hasBuffer()) {
				this->free();
			}

			// Init buffer in host's memory
			this->hostBuffer = std::unique_ptr<ArrayBuffer<T, N>>(
				(new BasicArrayBuffer<T, N>(StagingBuffer::device))
				->init(vk::BufferUsageFlagBits::eTransferSrc, vk::SharingMode::eExclusive,
				vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, size)
			);

			// Init buffer in device's memory
			this->deviceBuffer = std::unique_ptr<ArrayBuffer<T, N>>(
				(new BasicArrayBuffer<T, N>(StagingBuffer::device))
				->init(usageFlags ? vk::BufferUsageFlagBits::eTransferDst | usageFlags.get() : vk::BufferUsageFlagBits::eTransferDst,
				vk::SharingMode::eExclusive, vk::MemoryPropertyFlagBits::eDeviceLocal, size)
			);

			if (auto _device = ao::core::shared(StagingBuffer::device)) {
				// Create command buffer
				this->commandBuffer = _device->logical.allocateCommandBuffers(vk::CommandBufferAllocateInfo(_device->transferCommandPool, vk::CommandBufferLevel::ePrimary, 1))[0];

				// Create fence
				this->fence = _device->logical.createFence(vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled));
			}
			return this;
		}

		template<class T, size_t N>
		ArrayBuffer<T, N>* StagingArrayBuffer<T, N>::update(std::array<T, N> data) {
			if (!this->hasBuffer()) {
				throw core::Exception("Buffer hasn't been initialized");
			}

			// Update host buffer & synchronize memories
			if (auto host = static_cast<ArrayBuffer<T, N>*>(this->hostBuffer.get())) {
				host->update(data...);
			}
			else {
				throw core::Exception("Fail to update host buffer");
			}
			this->sync();

			return this;
		}

		template<class T, size_t N>
		ArrayBuffer<T, N>* StagingArrayBuffer<T, N>::updateFragment(std::size_t index, T * data) {
			if (!this->hasBuffer()) {
				throw core::Exception("Buffer hasn't been initialized");
			}

			// Update host buffer & synchronize memories
			if (auto host = static_cast<ArrayBuffer<T, N>*>(this->hostBuffer.get())) {
				host->updateFragment(index, data);
			}
			else {
				throw core::Exception(fmt::format("Fail to update host buffer fragment: {0}", index));
			}
			this->sync();

			return this;
		}

		template<class T, size_t N>
		bool StagingArrayBuffer<T, N>::hasBuffer() {
			return StagingBuffer::hasBuffer();
		}

		template<class T, size_t N>
		vk::Buffer & StagingArrayBuffer<T, N>::buffer() {
			return StagingBuffer::buffer();
		}

		template<class T, size_t N>
		vk::DeviceSize StagingArrayBuffer<T, N>::size() {
			return StagingBuffer::size();
		}

		template<class T, size_t N>
		vk::DeviceSize StagingArrayBuffer<T, N>::offset(size_t index) {
			return StagingBuffer::offset(index);
		}

		template<class T, size_t N>
		Buffer * StagingArrayBuffer<T, N>::map() {
			return StagingBuffer::map();
		}
	}
}
