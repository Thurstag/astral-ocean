#pragma once

#include "basic_buffer.hpp"

namespace ao {
	namespace vulkan {
		template<class... T>
		class StagingTupleBuffer : public TupleBuffer<T...> {
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
			virtual ~StagingTupleBuffer();

			/// <summary>
			/// Method to free buffer
			/// </summary>
			void free();

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
			TupleBuffer<T...>* map() override;
			vk::Buffer& buffer() override;
			vk::DeviceSize size() override;
			bool hasBuffer() override;
			vk::DeviceSize offset(size_t index) override;

		protected:
			bool memoryBarrier;

			std::unique_ptr<TupleBuffer<T...>> deviceBuffer;
			std::unique_ptr<TupleBuffer<T...>> hostBuffer;
			vk::CommandBufferUsageFlags usage;

			vk::CommandBuffer commandBuffer;
			vk::Fence fence;

			/// <summary>
			/// Method to synchronize host & device memories
			/// </summary>
			void sync();
		};

		/* IMPLEMENTATION */

		template<class ...T>
		StagingTupleBuffer<T...>::StagingTupleBuffer(std::weak_ptr<Device> device, vk::CommandBufferUsageFlags _usage, bool _memoryBarrier) : TupleBuffer<T...>(device), usage(_usage), memoryBarrier(_memoryBarrier) {}

		template<class ...T>
		StagingTupleBuffer<T...>::~StagingTupleBuffer() {
			this->free();

			if (auto _device = ao::core::shared(this->device)) {
				_device->logical.freeCommandBuffers(_device->transferCommandPool, this->commandBuffer);
				_device->logical.destroyFence(this->fence);
			}
		}

		template<class ...T>
		void StagingTupleBuffer<T...>::free() {
			if (this->hostBuffer.get() != nullptr) {
				this->hostBuffer.reset(nullptr);
			}
			if (this->deviceBuffer.get() != nullptr) {
				this->deviceBuffer.reset(nullptr);
			}
		}

		template<class ...T>
		StagingTupleBuffer<T...>* StagingTupleBuffer<T...>::init(std::vector<vk::DeviceSize> sizes, boost::optional<vk::BufferUsageFlags> usageFlags) {
			if (this->hasBuffer()) {
				this->free();
			}

			// Init buffer in host's memory
			this->hostBuffer = std::unique_ptr<TupleBuffer<T...>>(
				(new BasicTupleBuffer<T...>(this->device))
				->init(vk::BufferUsageFlagBits::eTransferSrc, vk::SharingMode::eExclusive,
				vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, sizes)
			);

			// Init buffer in device's memory
			this->deviceBuffer = std::unique_ptr<TupleBuffer<T...>>(
				(new BasicTupleBuffer<T...>(this->device))
				->init(usageFlags ? vk::BufferUsageFlagBits::eTransferDst | usageFlags.get() : vk::BufferUsageFlagBits::eTransferDst,
				vk::SharingMode::eExclusive, vk::MemoryPropertyFlagBits::eDeviceLocal, sizes)
			);

			if (auto _device = ao::core::shared(this->device)) {
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
			this->hostBuffer->update(data...);
			this->sync();

			return this;
		}

		template<class ...T>
		TupleBuffer<T...>* StagingTupleBuffer<T...>::updateFragment(std::size_t index, void * data) {
			if (!this->hasBuffer()) {
				throw core::Exception("Buffer hasn't been initialized");
			}

			// Update host buffer & synchronize memories
			this->hostBuffer->updateFragment(index, data);
			this->sync();

			return this;
		}

		template<class ...T>
		TupleBuffer<T...>* StagingTupleBuffer<T...>::map() {
			return this->hostBuffer->map();;
		}

		template<class ...T>
		vk::Buffer & StagingTupleBuffer<T...>::buffer() {
			if (this->deviceBuffer.get() == nullptr) {
				throw ao::core::Exception("Device buffer hasn't been initialized");
			}
			return this->deviceBuffer->buffer();
		}

		template<class ...T>
		vk::DeviceSize StagingTupleBuffer<T...>::size() {
			if (this->deviceBuffer.get() == nullptr) {
				throw ao::core::Exception("Device buffer hasn't been initialized");
			}
			return this->deviceBuffer->size();
		}

		template<class ...T>
		bool StagingTupleBuffer<T...>::hasBuffer() {
			if (this->hostBuffer.get() == nullptr || this->deviceBuffer.get() == nullptr) {
				return false;
			}
			return this->hostBuffer->hasBuffer() && this->deviceBuffer->hasBuffer();
		}

		template<class ...T>
		vk::DeviceSize StagingTupleBuffer<T...>::offset(size_t index) {
			return this->hostBuffer->offset(index);
		}

		template<class ...T>
		void StagingTupleBuffer<T...>::sync() {
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
