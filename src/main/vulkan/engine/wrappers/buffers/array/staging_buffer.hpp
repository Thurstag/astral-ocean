#pragma once

#include "basic_buffer.hpp"

namespace ao {
	namespace vulkan {
		template<class T>
		class StagingDynamicArrayBuffer : public DynamicArrayBuffer<T> {
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
			virtual ~StagingDynamicArrayBuffer();

			/// <summary>
			/// Method to free buffer
			/// </summary>
			void free();

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
			vk::DeviceSize offset(size_t index) override;
			vk::Buffer& buffer() override;
			vk::DeviceSize size() override;
			DynamicArrayBuffer<T>* map() override;

		protected:
			bool memoryBarrier;

			std::unique_ptr<DynamicArrayBuffer<T>> deviceBuffer;
			std::unique_ptr<DynamicArrayBuffer<T>> hostBuffer;
			vk::CommandBufferUsageFlags usage;

			vk::CommandBuffer commandBuffer;
			vk::Fence fence;

			/// <summary>
			/// Method to synchronize host & device memories
			/// </summary>
			void sync();
		};

		/* IMPLEMENTATION */

		template<class T>
		StagingDynamicArrayBuffer<T>::StagingDynamicArrayBuffer(std::weak_ptr<Device> device, vk::CommandBufferUsageFlags _usage, bool _memoryBarrier) : DynamicArrayBuffer<T>(device), usage(_usage), memoryBarrier(_memoryBarrier) {}

		template<class T>
		StagingDynamicArrayBuffer<T>::~StagingDynamicArrayBuffer() {
			this->free();

			if (auto _device = ao::core::shared(this->device)) {
				_device->logical.freeCommandBuffers(_device->transferCommandPool, this->commandBuffer);
				_device->logical.destroyFence(this->fence);
			}
		}

		template<class T>
		void StagingDynamicArrayBuffer<T>::free() {
			if (this->hostBuffer.get() != nullptr) {
				this->hostBuffer.reset(nullptr);
			}
			if (this->deviceBuffer.get() != nullptr) {
				this->deviceBuffer.reset(nullptr);
			}
		}

		template<class T>
		StagingDynamicArrayBuffer<T>* StagingDynamicArrayBuffer<T>::init(vk::DeviceSize size, boost::optional<vk::BufferUsageFlags> usageFlags) {
			if (this->hasBuffer()) {
				this->free();
			}

			// Init buffer in host's memory
			this->hostBuffer = std::unique_ptr<DynamicArrayBuffer<T>>(
				(new BasicDynamicArrayBuffer<T>(this->count, this->device))
				->init(vk::BufferUsageFlagBits::eTransferSrc, vk::SharingMode::eExclusive,
				vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, size)
			);

			// Init buffer in device's memory
			this->deviceBuffer = std::unique_ptr<DynamicArrayBuffer<T>>(
				(new BasicDynamicArrayBuffer<T>(this->count, this->device))
				->init(usageFlags ? vk::BufferUsageFlagBits::eTransferDst | usageFlags.get() : vk::BufferUsageFlagBits::eTransferDst,
				vk::SharingMode::eExclusive, vk::MemoryPropertyFlagBits::eDeviceLocal, size)
			);

			if (auto _device = ao::core::shared(this->device)) {
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
		vk::DeviceSize StagingDynamicArrayBuffer<T>::offset(size_t index) {
			return this->hostBuffer->offset(index);
		}

		template<class T>
		vk::Buffer & StagingDynamicArrayBuffer<T>::buffer() {
			if (this->deviceBuffer.get() == nullptr) {
				throw ao::core::Exception("Device buffer hasn't been initialized");
			}
			return this->deviceBuffer->buffer();
		}

		template<class T>
		vk::DeviceSize StagingDynamicArrayBuffer<T>::size() {
			if (this->deviceBuffer.get() == nullptr) {
				throw ao::core::Exception("Device buffer hasn't been initialized");
			}
			return this->deviceBuffer->size();
		}

		template<class T>
		DynamicArrayBuffer<T>* StagingDynamicArrayBuffer<T>::map() {
			return this->hostBuffer->map();
		}

		template<class T>
		void StagingDynamicArrayBuffer<T>::sync() {
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

		template<class T, size_t N>
		class StagingArrayBuffer : public ArrayBuffer<T, N> {
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
			virtual ~StagingArrayBuffer();

			/// <summary>
			/// Method to free buffer
			/// </summary>
			void free();

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
			vk::DeviceSize offset(size_t index) override;
			vk::Buffer& buffer() override;
			vk::DeviceSize size() override;
			ArrayBuffer<T, N>* map() override;

		protected:
			bool memoryBarrier;

			std::unique_ptr<ArrayBuffer<T, N>> deviceBuffer;
			std::unique_ptr<ArrayBuffer<T, N>> hostBuffer;
			vk::CommandBufferUsageFlags usage;

			vk::CommandBuffer commandBuffer;
			vk::Fence fence;

			/// <summary>
			/// Method to synchronize host & device memories
			/// </summary>
			void sync();
		};

		/* IMPLEMENTATION */

		template<class T, size_t N>
		StagingArrayBuffer<T, N>::StagingArrayBuffer(std::weak_ptr<Device> device, vk::CommandBufferUsageFlags _usage, bool _memoryBarrier) : ArrayBuffer<T, N>(device), usage(_usage), memoryBarrier(_memoryBarrier) {}

		template<class T, size_t N>
		StagingArrayBuffer<T, N>::~StagingArrayBuffer() {
			this->free();

			if (auto _device = ao::core::shared(this->device)) {
				_device->logical.freeCommandBuffers(_device->transferCommandPool, this->commandBuffer);
				_device->logical.destroyFence(this->fence);
			}
		}

		template<class T, size_t N>
		void StagingArrayBuffer<T, N>::free() {
			if (this->hostBuffer.get() != nullptr) {
				this->hostBuffer.reset(nullptr);
			}
			if (this->deviceBuffer.get() != nullptr) {
				this->deviceBuffer.reset(nullptr);
			}
		}

		template<class T, size_t N>
		StagingArrayBuffer<T, N>* StagingArrayBuffer<T, N>::init(vk::DeviceSize size, boost::optional<vk::BufferUsageFlags> usageFlags) {
			if (this->hasBuffer()) {
				this->free();
			}

			// Init buffer in host's memory
			this->hostBuffer = std::unique_ptr<ArrayBuffer<T, N>>(
				(new BasicArrayBuffer<T, N>(this->device))
				->init(vk::BufferUsageFlagBits::eTransferSrc, vk::SharingMode::eExclusive,
				vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, size)
			);

			// Init buffer in device's memory
			this->deviceBuffer = std::unique_ptr<ArrayBuffer<T, N>>(
				(new BasicArrayBuffer<T, N>(this->device))
				->init(usageFlags ? vk::BufferUsageFlagBits::eTransferDst | usageFlags.get() : vk::BufferUsageFlagBits::eTransferDst,
				vk::SharingMode::eExclusive, vk::MemoryPropertyFlagBits::eDeviceLocal, size)
			);

			if (auto _device = ao::core::shared(this->device)) {
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
			this->hostBuffer->update(data);
			this->sync();

			return this;
		}

		template<class T, size_t N>
		ArrayBuffer<T, N>* StagingArrayBuffer<T, N>::updateFragment(std::size_t index, T * data) {
			if (!this->hasBuffer()) {
				throw core::Exception("Buffer hasn't been initialized");
			}

			// Update host buffer & synchronize memories
			this->hostBuffer->updateFragment(index, data);
			this->sync();

			return this;
		}

		template<class T, size_t N>
		vk::DeviceSize StagingArrayBuffer<T, N>::offset(size_t index) {
			return this->hostBuffer->offset(index);
		}

		template<class T, size_t N>
		vk::Buffer & StagingArrayBuffer<T, N>::buffer() {
			if (this->deviceBuffer.get() == nullptr) {
				throw ao::core::Exception("Device buffer hasn't been initialized");
			}
			return this->deviceBuffer->buffer();
		}

		template<class T, size_t N>
		vk::DeviceSize StagingArrayBuffer<T, N>::size() {
			if (this->deviceBuffer.get() == nullptr) {
				throw ao::core::Exception("Device buffer hasn't been initialized");
			}
			return this->deviceBuffer->size();
		}

		template<class T, size_t N>
		ArrayBuffer<T, N>* StagingArrayBuffer<T, N>::map() {
			return this->hostBuffer->map();
		}

		template<class T, size_t N>
		void StagingArrayBuffer<T, N>::sync() {
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
	}
}
