#pragma once

#include <ao/core/exception/exception.h>
#include <boost/optional.hpp>
#include <vulkan/vulkan.hpp>

#include "../device.h"
#include "buffer.hpp"

namespace ao {
	namespace vulkan {
		template<class T>
		class BasicBuffer : public Buffer<T> {
		public:
			/// <summary>
			/// Constructor
			/// </summary>
			/// <param name="_device">Device</param>
			BasicBuffer(std::weak_ptr<Device> _device);

			/// <summary>
			/// Destructor
			/// </summary>
			virtual ~BasicBuffer();

			/// <summary>
			/// Method to free buffer (buffer + memory)
			/// </summary>
			void free();

			/// <summary>
			/// Method to init buffer, memory & data.
			/// 
			/// If object already stores a buffer, it will free the old one
			/// </summary>
			/// <param name="usageFlags">Usage flags</param>
			/// <param name="sharingMode">Sharing mode</param>
			/// <param name="memoryFlags">Memory flags</param>
			/// <param name="size">Data size</param>
			/// <param name="data">Data</param>
			/// <returns>This</returns>
			virtual BasicBuffer<T>& init(vk::BufferUsageFlags usageFlags, vk::SharingMode sharingMode, vk::MemoryPropertyFlags memoryFlags, vk::DeviceSize size, boost::optional<T> data = boost::none);

			/// <summary>
			/// Method to map memory
			/// </summary>
			/// <returns>This</returns>
			virtual BasicBuffer<T>& map();

			BasicBuffer<T>& update(T data) override;
			vk::DeviceSize size() override;
			vk::Buffer& buffer() override;
			bool hasBuffer() override;

		protected:
			bool mHasBuffer = false;

			vk::DeviceMemory memory;
			vk::DeviceSize mSize;
			vk::Buffer mBuffer;
			void* mapper = nullptr;
		};

		/* IMPLEMENTATION */

		template<class T>
		BasicBuffer<T>::BasicBuffer(std::weak_ptr<Device> _device) : Buffer<T>(_device) {}

		template<class T>
		BasicBuffer<T>::~BasicBuffer() {
			this->free();
		}

		template<class T>
		void BasicBuffer<T>::free() {
			auto _device = ao::core::get(this->device);

			if (!this->memory) {
				_device->logical.unmapMemory(this->memory);
			}

			_device->logical.destroyBuffer(this->mBuffer);
			_device->logical.freeMemory(this->memory);

			this->mHasBuffer = false;
			this->mapper = nullptr;
		}

		template<class T>
		BasicBuffer<T>& BasicBuffer<T>::init(vk::BufferUsageFlags usageFlags, vk::SharingMode sharingMode, vk::MemoryPropertyFlags memoryFlags, vk::DeviceSize size, boost::optional<T> data) {
			if (this->hasBuffer()) {
				this->free();
			}
			auto _device = ao::core::get(this->device);

			// Create buffer
			this->mBuffer = _device->logical.createBuffer(vk::BufferCreateInfo(vk::BufferCreateFlags(), size, usageFlags, sharingMode));

			// Get memory requirements
			vk::MemoryRequirements memRequirements = _device->logical.getBufferMemoryRequirements(this->mBuffer);

			// Allocate memory
			this->memory = _device->logical.allocateMemory(
				vk::MemoryAllocateInfo(memRequirements.size, _device->memoryType(memRequirements.memoryTypeBits, memoryFlags))
			);

			// Bind memory and buffer
			_device->logical.bindBufferMemory(this->mBuffer, this->memory, 0);

			// Update fields
			this->mHasBuffer = true;
			this->mSize = size;

			// Copy data into buffer
			if (data) {
				this->map();
				std::memcpy(this->mapper, data.get(), size);
			}
			return *this;
		}

		template<class T>
		BasicBuffer<T>& BasicBuffer<T>::map() {
			if (this->mapper) {
				throw ao::core::Exception("Buffer is already mapped");
			}
			this->mapper = ao::core::get(this->device)->logical.mapMemory(this->memory, 0, mSize);
			return *this;
		}

		template<class T>
		BasicBuffer<T>& BasicBuffer<T>::update(T data) {
			if (!this->hasBuffer()) {
				throw ao::core::Exception("Buffer hasn't been initialized");
			}
			if (!this->mapper) {
				this->map();
			}

			// Copy data into buffer
			std::memcpy(this->mapper, data, this->mSize);
			return *this;
		}

		template<class T>
		vk::Buffer & BasicBuffer<T>::buffer() {
			return this->mBuffer;
		}

		template<class T>
		bool BasicBuffer<T>::hasBuffer() {
			return this->mHasBuffer;
		}
		template<class T>
		vk::DeviceSize BasicBuffer<T>::size() {
			return this->mSize;
		}
	}
}
