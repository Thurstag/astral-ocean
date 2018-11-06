#pragma once

#include <limits>

#include "basic_buffer.hpp"

namespace ao {
	namespace vulkan {
		template<class T>
		class DeviceBuffer : public Buffer<T> {
		public:
			/// <summary>
			/// Constructor
			/// </summary>
			/// <param name="device">Device</param>
			DeviceBuffer(Device* device);
			
			/// <summary>
			/// Destructor
			/// </summary>
			virtual ~DeviceBuffer();

			/// <summary>
			/// Method to free buffer (buffer + memory)
			/// </summary>
			void free();

			/// <summary>
			/// Method to init buffer, memory & data.
			/// 
			/// If object already stores a buffer, it will free the old one
			/// </summary>
			/// <param name="size">Data size</param>
			/// <param name="data">Data</param>
			/// <returns>This</returns>
			virtual DeviceBuffer<T>& init(vk::DeviceSize size, boost::optional<T> data = boost::optional<T>());

			DeviceBuffer<T>& update(T data) override;
			vk::DeviceSize size() override;
			vk::Buffer& buffer() override;
			bool hasBuffer() override;

		protected:
			BasicBuffer<T>* deviceBuffer = nullptr;
			BasicBuffer<T>* hostBuffer = nullptr;

			vk::CommandBuffer commandBuffer;
			vk::Fence fence;

			/// <summary>
			/// Method to synchronize host & device memories
			/// </summary>
			void sync();
		};

		/* IMPLEMENTATION */

		template<class T>
		DeviceBuffer<T>::DeviceBuffer(Device * device) : Buffer<T>(device) {}

		template<class T>
		DeviceBuffer<T>::~DeviceBuffer() {
			this->free();

			this->device->logical.freeCommandBuffers(this->device->commandPool, this->commandBuffer);
			this->device->logical.destroyFence(this->fence);
		}

		template<class T>
		void DeviceBuffer<T>::free() {
			if (this->hostBuffer) {
				delete this->hostBuffer;
			}
			if (this->deviceBuffer) {
				delete this->deviceBuffer;
			}

			this->deviceBuffer = this->hostBuffer = nullptr;
		}

		template<class T>
		DeviceBuffer<T>& DeviceBuffer<T>::init(vk::DeviceSize size, boost::optional<T> data) {
			if (this->hasBuffer()) {
				this->free();
			}

			// Init buffer in host's memory
			this->hostBuffer = &(new BasicBuffer<T>(this->device))
				->init(vk::BufferUsageFlagBits::eTransferSrc, vk::SharingMode::eExclusive,
					  vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
					  size, data);

			// Init buffer in device's memory
			this->deviceBuffer = &(new BasicBuffer<T>(this->device))
				->init(vk::BufferUsageFlagBits::eTransferDst, vk::SharingMode::eExclusive,
					  vk::MemoryPropertyFlagBits::eDeviceLocal, size);

			// Create command buffer
			this->commandBuffer = this->device->logical.allocateCommandBuffers(vk::CommandBufferAllocateInfo(this->device->commandPool, vk::CommandBufferLevel::ePrimary, 1))[0];

			// Create fence
			this->fence = this->device->logical.createFence(vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled));

			// Synchronize memories
			if (data) {
				this->sync();
			}

			return *this;
		}

		template<class T>
		DeviceBuffer<T>& DeviceBuffer<T>::update(T data) {
			if (!this->hasBuffer()) {
				throw ao::core::Exception("Buffer hasn't been initialized");
			}

			// Update host buffer & synchronize memories
			this->hostBuffer->update(data);
			this->sync();

			return *this;
		}

		template<class T>
		inline vk::DeviceSize DeviceBuffer<T>::size() {
			if (this->deviceBuffer) {
				return this->deviceBuffer->size();
			}
			return 0;
		}

		template<class T>
		vk::Buffer & DeviceBuffer<T>::buffer() {
			if (this->deviceBuffer) {
				return this->deviceBuffer->buffer();
			}

			LOGGER << LogLevel::WARN << "Try to get buffer but it hasn't been initialized";
			return vk::Buffer();
		}

		template<class T>
		void DeviceBuffer<T>::sync() {
			// Create command to transfer data from host to device
			this->commandBuffer.begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eSimultaneousUse));
			{
				this->commandBuffer.copyBuffer(this->hostBuffer->buffer(), this->deviceBuffer->buffer(), vk::BufferCopy().setSize(this->deviceBuffer->size()));
			}
			this->commandBuffer.end();

			vk::SubmitInfo submitInfo = vk::SubmitInfo()
				.setCommandBufferCount(1)
				.setPCommandBuffers(&this->commandBuffer);

			// Submit command
			this->device->transferQueue.submit(submitInfo, this->fence);

			// Wait fence
			auto MAX_64 = std::numeric_limits<uint64_t>::max;
			this->device->logical.waitForFences(this->fence, VK_TRUE, MAX_64());

			// Reset fence
			this->device->logical.resetFences(this->fence);
		}

		template<class T>
		bool DeviceBuffer<T>::hasBuffer() {
			if (!this->hostBuffer || !this->deviceBuffer) {
				return false;
			}
			return this->hostBuffer->hasBuffer() && this->hostBuffer->hasBuffer();
		}
	}
}
