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
			/// <param name="_usage">Buffer usage</param>
			DeviceBuffer(std::weak_ptr<Device> device, vk::CommandBufferUsageFlags _usage = vk::CommandBufferUsageFlagBits::eSimultaneousUse);

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
			virtual DeviceBuffer<T>& init(vk::DeviceSize size, boost::optional<vk::BufferUsageFlags> usageFlags = boost::none, boost::optional<T> data = boost::none);

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
		DeviceBuffer<T>::DeviceBuffer(std::weak_ptr<Device> device, vk::CommandBufferUsageFlags _usage) : Buffer<T>(device), usage(_usage) {}

		template<class T>
		DeviceBuffer<T>::~DeviceBuffer() {
			this->free();

			auto _device = ao::core::get(this->device);
			if (!(this->usage & vk::CommandBufferUsageFlagBits::eOneTimeSubmit)) {
				_device->logical.freeCommandBuffers(_device->transferCommandPool, this->commandBuffer);
				_device->logical.destroyFence(this->fence);
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


			if (auto _device = ao::core::get(this->device)) {
				// Create command buffer
				this->commandBuffer = _device->logical.allocateCommandBuffers(vk::CommandBufferAllocateInfo(_device->transferCommandPool, vk::CommandBufferLevel::ePrimary, 1))[0];

				// Create fence
				this->fence = _device->logical.createFence(vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled));
			}

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
			auto _device = ao::core::get(this->device);

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
			_device->queues[vk::QueueFlagBits::eTransfer].queue.submit(submitInfo, this->fence);

			// Wait fence
			_device->logical.waitForFences(this->fence, VK_TRUE, (std::numeric_limits<u64>::max)());

			// Reset fence
			_device->logical.resetFences(this->fence);

			// Free useless resources
			if (this->usage & vk::CommandBufferUsageFlagBits::eOneTimeSubmit) {
				_device->logical.freeCommandBuffers(_device->transferCommandPool, this->commandBuffer);
				_device->logical.destroyFence(this->fence);

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
