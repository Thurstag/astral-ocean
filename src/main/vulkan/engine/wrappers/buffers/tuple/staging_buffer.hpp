#pragma once

#include "../staging_buffer.h"
#include "basic_buffer.hpp"

namespace ao {
	namespace vulkan {
		template<class... T>
		class StagingTupleBuffer : public virtual TupleBuffer<T...>, public virtual StagingBuffer {
		public:
			/// <summary>
			/// Constructor
			/// </summary>
			/// <param name="device">Device</param>
			/// <param name="_usage">Usage</param>
			/// <param name="_memoryBarrier">Bind memory barrier on transfer</param>
			StagingTupleBuffer(std::weak_ptr<Device> device, vk::CommandBufferUsageFlags _usage = vk::CommandBufferUsageFlagBits::eSimultaneousUse, bool _memoryBarrier = false);

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
			StagingTupleBuffer<T...>* init(std::vector<vk::DeviceSize> sizes, boost::optional<vk::BufferUsageFlags> usageFlags = boost::none);

			TupleBuffer<T...>* update(T*... data) override;
			TupleBuffer<T...>* updateFragment(std::size_t index, void* data) override;
			bool hasBuffer() override;
			vk::Buffer& buffer() override;
			vk::DeviceSize size() override;
			vk::DeviceSize offset(size_t index) override;
			Buffer* map() override;
		};

		/* IMPLEMENTATION */

		template<class ...T>
		StagingTupleBuffer<T...>::StagingTupleBuffer(std::weak_ptr<Device> device, vk::CommandBufferUsageFlags _usage, bool _memoryBarrier) : TupleBuffer<T...>(device), StagingBuffer(device, _usage, _memoryBarrier) {}

		template<class ...T>
		StagingTupleBuffer<T...>* StagingTupleBuffer<T...>::init(std::vector<vk::DeviceSize> sizes, boost::optional<vk::BufferUsageFlags> usageFlags) {
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
				->init(usageFlags ? vk::BufferUsageFlagBits::eTransferDst | usageFlags.get() : vk::BufferUsageFlagBits::eTransferDst,
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
		TupleBuffer<T...>* StagingTupleBuffer<T...>::update(T*... data) {
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
		TupleBuffer<T...>* StagingTupleBuffer<T...>::updateFragment(std::size_t index, void * data) {
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
		bool StagingTupleBuffer<T...>::hasBuffer() {
			return StagingBuffer::hasBuffer();
		}

		template<class ...T>
		vk::Buffer & StagingTupleBuffer<T...>::buffer() {
			return StagingBuffer::buffer();
		}

		template<class ...T>
		vk::DeviceSize StagingTupleBuffer<T...>::size() {
			return StagingBuffer::size();
		}

		template<class ...T>
		vk::DeviceSize StagingTupleBuffer<T...>::offset(size_t index) {
			return StagingBuffer::offset(index);
		}

		template<class ...T>
		Buffer * StagingTupleBuffer<T...>::map() {
			return StagingBuffer::map();
		}
	}
}
