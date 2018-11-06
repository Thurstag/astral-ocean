#pragma once

#include <ao/core/exception.h>
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
			BasicBuffer(Device* _device);

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
			virtual BasicBuffer<T>& init(vk::BufferUsageFlags usageFlags, vk::SharingMode sharingMode, vk::MemoryPropertyFlags memoryFlags, vk::DeviceSize size, boost::optional<T> data = boost::optional<T>());

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
		BasicBuffer<T>::BasicBuffer(Device * _device) : Buffer<T>(_device) {}

		template<class T>
		BasicBuffer<T>::~BasicBuffer() {
			this->free();
		}

		template<class T>
		void BasicBuffer<T>::free() {
			if (!this->memory) {
				this->device->logical.unmapMemory(this->memory);
			}

			this->device->logical.destroyBuffer(this->mBuffer);
			this->device->logical.freeMemory(this->memory);

			this->mHasBuffer = false;
			this->mapper = nullptr;
		}

		template<class T>
		BasicBuffer<T>& BasicBuffer<T>::init(vk::BufferUsageFlags usageFlags, vk::SharingMode sharingMode, vk::MemoryPropertyFlags memoryFlags, vk::DeviceSize size, boost::optional<T> data) {
			if (this->hasBuffer()) {
				this->free();
			}

			// Create buffer
			this->mBuffer = this->device->logical.createBuffer(vk::BufferCreateInfo(vk::BufferCreateFlags(), size, usageFlags, sharingMode));

			// Get memory requirements
			vk::MemoryRequirements memRequirements = this->device->logical.getBufferMemoryRequirements(this->mBuffer);

			// Allocate memory
			this->memory = this->device->logical.allocateMemory(
				vk::MemoryAllocateInfo(memRequirements.size, this->device->memoryType(memRequirements.memoryTypeBits, memoryFlags))
			);

			// Bind memory and buffer
			this->device->logical.bindBufferMemory(this->mBuffer, this->memory, 0);

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
			this->mapper = this->device->logical.mapMemory(this->memory, 0, mSize);
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
		inline vk::DeviceSize BasicBuffer<T>::size() {
			return this->mSize;
		}
	}
}
