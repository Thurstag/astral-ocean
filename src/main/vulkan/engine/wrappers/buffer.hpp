#pragma once

#include <ao/core/exception.h>
#include <boost/optional.hpp>
#include <vulkan/vulkan.hpp>

#include "device.h"

namespace ao {
	namespace vulkan {
		template<class T>
		class Buffer {
		public:
			/// <summary>
			/// Constructor
			/// </summary>
			/// <param name="device">Device</param>
			Buffer(Device* device);

			/// <summary>
			/// Destructor
			/// </summary>
			virtual ~Buffer();

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
			Buffer<T>& init(vk::BufferUsageFlags usageFlags, vk::SharingMode sharingMode, vk::MemoryPropertyFlags memoryFlags, vk::DeviceSize size, boost::optional<T> data = boost::optional<T>());

			/// <summary>
			/// Method to update buffer
			/// </summary>
			/// <param name="data">Data</param>
			/// <returns>This</returns>
			Buffer<T>& update(T data);

			/// <summary>
			/// Method to get buffer
			/// </summary>
			/// <returns>vk::Buffer</returns>
			vk::Buffer& buffer();
		private:
			bool hasBuffer = false;
			Device* device;

			vk::DeviceMemory memory;
			vk::DeviceSize size;
			vk::Buffer mBuffer;
			void* mapper;
		};

		/* IMPLEMENTATION */

		template<class T>
		Buffer<T>::Buffer(Device * device) {
			this->device = device;
		}

		template<class T>
		Buffer<T>::~Buffer() {
			this->free();
		}

		template<class T>
		void Buffer<T>::free() {
			this->device->logical.unmapMemory(this->memory);
			this->device->logical.destroyBuffer(this->mBuffer);
			this->device->logical.freeMemory(this->memory);

			this->hasBuffer = false;
		}

		template<class T>
		Buffer<T>& Buffer<T>::init(vk::BufferUsageFlags usageFlags, vk::SharingMode sharingMode, vk::MemoryPropertyFlags memoryFlags, vk::DeviceSize size, boost::optional<T> data) {
			if (this->hasBuffer) {
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
			this->hasBuffer = true;
			this->size = size;

			// Map memory
			this->mapper = this->device->logical.mapMemory(this->memory, 0, size);

			// Copy data into buffer
			if (data) {
				std::memcpy(this->mapper, data.get(), size);
			}
			return *this;
		}

		template<class T>
		Buffer<T>& Buffer<T>::update(T data) {
			if (!this->hasBuffer) {
				throw ao::core::Exception("Buffer hasn't been initialized");
			}

			// Copy data into buffer
			std::memcpy(this->mapper, data, this->size);
			return *this;
		}

		template<class T>
		vk::Buffer & Buffer<T>::buffer() {
			return this->mBuffer;
		}
	}
}
