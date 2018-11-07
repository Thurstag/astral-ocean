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
			/// <param name="usage">Buffer usage</param>
			DeviceBuffer(Device* device, vk::CommandBufferUsageFlags usage = vk::CommandBufferUsageFlagBits::eSimultaneousUse);
			
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
			/// <param name="usageFlags">Additional usage flags</param>
			/// <param name="data">Data</param>
			/// <returns>This</returns>
			virtual DeviceBuffer<T>& init(vk::DeviceSize size, boost::optional<vk::BufferUsageFlags> usageFlags = boost::none,  boost::optional<T> data = boost::none);

			DeviceBuffer<T>& update(T data) override;
			vk::DeviceSize size() override;
			vk::Buffer& buffer() override;
			bool hasBuffer() override;

		protected:
			BasicBuffer<T>* deviceBuffer = nullptr;
			BasicBuffer<T>* hostBuffer = nullptr;
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
		DeviceBuffer<T>::DeviceBuffer(Device * device, vk::CommandBufferUsageFlags usage) : Buffer<T>(device) {
			this->usage = usage;
		}

		template<class T>
		DeviceBuffer<T>::~DeviceBuffer() {
			this->free();

			if (!(this->usage & vk::CommandBufferUsageFlagBits::eOneTimeSubmit)) {
				this->device->logical.freeCommandBuffers(this->device->commandPool, this->commandBuffer);
				this->device->logical.destroyFence(this->fence);
			}
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
		DeviceBuffer<T>& DeviceBuffer<T>::init(vk::DeviceSize size, boost::optional<vk::BufferUsageFlags> usageFlags, boost::optional<T> data) {
			if (this->hasBuffer()) {
				this->free();
			}

			// Init buffer in host's memory
			this->hostBuffer = &(new BasicBuffer<T>(this->device))
				->init(vk::BufferUsageFlagBits::eTransferSrc, vk::SharingMode::eExclusive,
					  vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
					  size, data);

			// Init buffer in device's memory
			if (usageFlags) {
				this->deviceBuffer = &(new BasicBuffer<T>(this->device))
					->init(vk::BufferUsageFlagBits::eTransferDst | usageFlags.get(), vk::SharingMode::eExclusive,
						   vk::MemoryPropertyFlagBits::eDeviceLocal, size);
			}
			else {
				this->deviceBuffer = &(new BasicBuffer<T>(this->device))
					->init(vk::BufferUsageFlagBits::eTransferDst, vk::SharingMode::eExclusive,
						   vk::MemoryPropertyFlagBits::eDeviceLocal, size);
			}


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
			if (this->usage & vk::CommandBufferUsageFlagBits::eOneTimeSubmit) {
				throw ao::core::Exception("Buffer usage is eOneTimeSubmit, can't update it");
			}

			// Update host buffer & synchronize memories
			this->hostBuffer->update(data);
			this->sync();

			return *this;
		}

		template<class T>
		vk::DeviceSize DeviceBuffer<T>::size() {
			if (this->deviceBuffer) {
				return this->deviceBuffer->size();
			}
			return 0;
		}

		template<class T>
		vk::Buffer & DeviceBuffer<T>::buffer() {
			if (!this->deviceBuffer) {
				throw ao::core::Exception("Device buffer hasn't been initialized");
			}
			return this->deviceBuffer->buffer();
		}

		template<class T>
		void DeviceBuffer<T>::sync() {
			if ((this->usage & vk::CommandBufferUsageFlagBits::eOneTimeSubmit) && !this->hostBuffer) {
				throw ao::core::Exception("Buffer usage is eOneTimeSubmit, can't update it");
			}

			// Create command to transfer data from host to device
			this->commandBuffer.begin(vk::CommandBufferBeginInfo(this->usage));
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

			// Free useless resources
			if (this->usage & vk::CommandBufferUsageFlagBits::eOneTimeSubmit) {
				this->device->logical.freeCommandBuffers(this->device->commandPool, this->commandBuffer);
				this->device->logical.destroyFence(this->fence);

				delete this->hostBuffer;
				this->hostBuffer = nullptr;

				LOGGER << LogLevel::DEBUG << "Free resources after buffer submission";
			}
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
