// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <ao/core/exception/index_out_of_range.h>

#include "buffer.hpp"

namespace ao {
	namespace vulkan {

		template<class T>
		class BasicDynamicArrayBuffer : public DynamicArrayBuffer<T> {
		public:
			/// <summary>
			/// Constructor
			/// </summary>
			/// <param name="count">Count</param>
			/// <param name="device">Device</param>
			BasicDynamicArrayBuffer(size_t count, std::weak_ptr<Device> device);

			/// <summary>
			/// Destructor
			/// </summary>
			virtual ~BasicDynamicArrayBuffer();

			/// <summary>
			/// Method to free buffer
			/// </summary>
			void free();

			/// <summary>
			/// Method to init buffer
			/// 
			/// If object already stores a buffer, it will free the old one
			/// </summary>
			/// <param name="usageFlags">Usage flags</param>
			/// <param name="sharingMode">Sharing mode</param>
			/// <param name="memoryFlags">Memory flags</param>
			/// <param name="size">Fragment size</param>
			/// <returns>This</returns>
			BasicDynamicArrayBuffer<T>* init(vk::BufferUsageFlags usageFlags, vk::SharingMode sharingMode, vk::MemoryPropertyFlags memoryFlags, vk::DeviceSize size);

			DynamicArrayBuffer<T>* update(std::vector<T>& data) override;
			DynamicArrayBuffer<T>* updateFragment(std::size_t index, T* data) override;
			vk::DeviceSize offset(size_t index) override;
			vk::Buffer& buffer() override;
			vk::DeviceSize size() override;
			DynamicArrayBuffer<T>* map() override;

		protected:
			vk::MemoryPropertyFlags memoryFlags;

			vk::DeviceMemory memory;
			vk::DeviceSize mSize;
			vk::Buffer mBuffer;

			bool hasMapper;
			void* mapper;
		};

		/* IMPLEMENTATION */

		template<class T>
		BasicDynamicArrayBuffer<T>::BasicDynamicArrayBuffer(size_t count, std::weak_ptr<Device> device) : DynamicArrayBuffer<T>(count, device), hasMapper(false) {}

		template<class T>
		BasicDynamicArrayBuffer<T>::~BasicDynamicArrayBuffer() {
			this->free();
		}

		template<class T>
		void BasicDynamicArrayBuffer<T>::free() {
			auto _device = ao::core::shared(this->device);

			if (this->hasMapper) {
				_device->logical.unmapMemory(this->memory);
				this->hasMapper = false;
			}
			if (this->mHasBuffer) {
				_device->logical.destroyBuffer(this->mBuffer);
				_device->logical.freeMemory(this->memory);
				this->mHasBuffer = false;
			}
		}

		template<class T>
		BasicDynamicArrayBuffer<T>* BasicDynamicArrayBuffer<T>::init(vk::BufferUsageFlags usageFlags, vk::SharingMode sharingMode, vk::MemoryPropertyFlags memoryFlags, vk::DeviceSize size) {
			if (this->hasBuffer()) {
				this->free();
			}
			auto _device = ao::core::shared(this->device);

			// Get total size
			this->mSize = size * this->count;

			// Create buffer
			this->mBuffer = _device->logical.createBuffer(vk::BufferCreateInfo(vk::BufferCreateFlags(), this->mSize, usageFlags, sharingMode));

			// Get memory requirements
			vk::MemoryRequirements memRequirements = _device->logical.getBufferMemoryRequirements(this->mBuffer);

			// Allocate memory
			this->memory = _device->logical.allocateMemory(
				vk::MemoryAllocateInfo(memRequirements.size, _device->memoryType(memRequirements.memoryTypeBits, memoryFlags))
			);

			// Bind memory and buffer
			_device->logical.bindBufferMemory(this->mBuffer, this->memory, 0);
			this->memoryFlags = memoryFlags;
			this->mHasBuffer = true;

			return this;
		}

		template<class T>
		DynamicArrayBuffer<T>* BasicDynamicArrayBuffer<T>::update(std::vector<T>& data) {
			if (!this->hasBuffer()) {
				throw core::Exception("Buffer hasn't been initialized");
			}

			// Map memory
			if (!this->hasMapper) {
				this->map();
			}

			// Check sizes
			if (data.size() != this->count) {
				throw core::Exception(fmt::format("Data argument size should be equal to: {0}", this->count));
			}

			// Update fragments
			for (size_t i = 0; i < this->count; i++) {
				std::memcpy((void*)((uint64_t)this->mapper + i * this->mSize / this->count), &data[i], this->mSize / this->count);
			}

			// Notify changes
			if (!(this->memoryFlags & vk::MemoryPropertyFlagBits::eHostCoherent)) {
				ao::core::shared(this->device)->logical.flushMappedMemoryRanges(vk::MappedMemoryRange(this->memory, 0, this->mSize));
			}

			return this;
		}

		template<class T>
		DynamicArrayBuffer<T>* BasicDynamicArrayBuffer<T>::updateFragment(std::size_t index, T * data) {
			if (!this->hasBuffer()) {
				throw core::Exception("Buffer hasn't been initialized");
			}

			// Check index
			if (index >= this->count) {
				throw core::IndexOutOfRangeException(std::make_pair(static_cast<u64>(0), static_cast<u64>(this->count)));
			}

			// Map memory
			if (!this->hasMapper) {
				this->map();
			}

			// Copy into buffer
			std::memcpy((void*)((uint64_t)this->mapper + (index * this->mSize / this->count)), data, this->mSize / this->count);

			// Notify changes
			if (!(this->memoryFlags & vk::MemoryPropertyFlagBits::eHostCoherent)) {
				ao::core::shared(this->device)->logical.flushMappedMemoryRanges(vk::MappedMemoryRange(this->memory, this->offset(index), this->mSize / this->count));
			}

			return this;
		}

		template<class T>
		vk::DeviceSize BasicDynamicArrayBuffer<T>::offset(size_t index) {
			if (index >= this->count) {
				throw core::IndexOutOfRangeException(std::make_pair(static_cast<u64>(0), static_cast<u64>(this->count)));
			}
			return index * this->mSize / this->count;
		}

		template<class T>
		vk::Buffer & BasicDynamicArrayBuffer<T>::buffer() {
			return this->mBuffer;
		}

		template<class T>
		vk::DeviceSize BasicDynamicArrayBuffer<T>::size() {
			return this->mSize;
		}

		template<class T>
		DynamicArrayBuffer<T>* BasicDynamicArrayBuffer<T>::map() {
			if (this->hasMapper) {
				throw ao::core::Exception("Buffer is already mapped");
			}

			// Map entire buffer
			this->mapper = ao::core::shared(this->device)->logical.mapMemory(this->memory, 0, this->mSize);

			this->hasMapper = true;
			return this;
		}

		template<class T, size_t N>
		class BasicArrayBuffer : public ArrayBuffer<T, N> {
		public:
			/// <summary>
			/// Constructor
			/// </summary>
			/// <param name="device">Device</param>
			BasicArrayBuffer(std::weak_ptr<Device> device);

			/// <summary>
			/// Destructor
			/// </summary>
			virtual ~BasicArrayBuffer();

			/// <summary>
			/// Method to free buffer
			/// </summary>
			void free();

			/// <summary>
			/// Method to init buffer
			/// 
			/// If object already stores a buffer, it will free the old one
			/// </summary>
			/// <param name="usageFlags">Usage flags</param>
			/// <param name="sharingMode">Sharing mode</param>
			/// <param name="memoryFlags">Memory flags</param>
			/// <param name="size">Fragment size</param>
			/// <returns>This</returns>
			BasicArrayBuffer<T, N>* init(vk::BufferUsageFlags usageFlags, vk::SharingMode sharingMode, vk::MemoryPropertyFlags memoryFlags, vk::DeviceSize size);

			ArrayBuffer<T, N>* update(std::array<T, N> data) override;
			ArrayBuffer<T, N>* updateFragment(std::size_t index, T* data) override;
			vk::DeviceSize offset(size_t index) override;
			vk::Buffer& buffer() override;
			vk::DeviceSize size() override;
			ArrayBuffer<T, N>* map() override;

		protected:
			vk::MemoryPropertyFlags memoryFlags;

			vk::DeviceMemory memory;
			vk::DeviceSize mSize;
			vk::Buffer mBuffer;

			bool hasMapper;
			void* mapper;
		};

		/* IMPLEMENTATION */

		template<class T, size_t N>
		BasicArrayBuffer<T, N>::BasicArrayBuffer(std::weak_ptr<Device> device) : ArrayBuffer<T, N>(device), hasMapper(false) {}

		template<class T, size_t N>
		BasicArrayBuffer<T, N>::~BasicArrayBuffer() {
			this->free();
		}

		template<class T, size_t N>
		void BasicArrayBuffer<T, N>::free() {
			auto _device = ao::core::shared(this->device);

			if (this->hasMapper) {
				_device->logical.unmapMemory(this->memory);
				this->hasMapper = false;
			}
			if (this->mHasBuffer) {
				_device->logical.destroyBuffer(this->mBuffer);
				_device->logical.freeMemory(this->memory);
				this->mHasBuffer = false;
			}
		}

		template<class T, size_t N>
		BasicArrayBuffer<T, N>* BasicArrayBuffer<T, N>::init(vk::BufferUsageFlags usageFlags, vk::SharingMode sharingMode, vk::MemoryPropertyFlags memoryFlags, vk::DeviceSize size) {
			if (this->hasBuffer()) {
				this->free();
			}
			auto _device = ao::core::shared(this->device);

			// Get total size
			this->mSize = size * N;

			// Create buffer
			this->mBuffer = _device->logical.createBuffer(vk::BufferCreateInfo(vk::BufferCreateFlags(), this->mSize, usageFlags, sharingMode));

			// Get memory requirements
			vk::MemoryRequirements memRequirements = _device->logical.getBufferMemoryRequirements(this->mBuffer);

			// Allocate memory
			this->memory = _device->logical.allocateMemory(
				vk::MemoryAllocateInfo(memRequirements.size, _device->memoryType(memRequirements.memoryTypeBits, memoryFlags))
			);

			// Bind memory and buffer
			_device->logical.bindBufferMemory(this->mBuffer, this->memory, 0);
			this->memoryFlags = memoryFlags;
			this->mHasBuffer = true;

			return this;
		}

		template<class T, size_t N>
		ArrayBuffer<T, N>* BasicArrayBuffer<T, N>::update(std::array<T, N> data) {
			if (!this->hasBuffer()) {
				throw core::Exception("Buffer hasn't been initialized");
			}

			// Map memory
			if (!this->hasMapper) {
				this->map();
			}

			// Update fragments
			for (size_t i = 0; i < N; i++) {
				std::memcpy((void*)((uint64_t)this->mapper + i * this->mSize / N), &data[i], this->mSize / N);
			}

			// Notify changes
			if (!(this->memoryFlags & vk::MemoryPropertyFlagBits::eHostCoherent)) {
				ao::core::shared(this->device)->logical.flushMappedMemoryRanges(vk::MappedMemoryRange(this->memory, 0, this->mSize));
			}

			return this;
		}

		template<class T, size_t N>
		ArrayBuffer<T, N>* BasicArrayBuffer<T, N>::updateFragment(std::size_t index, T * data) {
			if (!this->hasBuffer()) {
				throw core::Exception("Buffer hasn't been initialized");
			}

			// Check index
			if (index >= N) {
				throw core::IndexOutOfRangeException(std::make_pair(static_cast<u64>(0), static_cast<u64>(N)));
			}

			// Map memory
			if (!this->hasMapper) {
				this->map();
			}

			// Copy into buffer
			std::memcpy((void*)((uint64_t)this->mapper + (index * this->mSize / N)), data, this->mSize / N);

			// Notify changes
			if (!(this->memoryFlags & vk::MemoryPropertyFlagBits::eHostCoherent)) {
				ao::core::shared(this->device)->logical.flushMappedMemoryRanges(vk::MappedMemoryRange(this->memory, this->offset(index), this->mSize / N));
			}

			return this;
		}

		template<class T, size_t N>
		vk::DeviceSize BasicArrayBuffer<T, N>::offset(size_t index) {
			if (index >= N) {
				throw core::IndexOutOfRangeException(std::make_pair(static_cast<u64>(0), static_cast<u64>(N)));
			}
			return index * this->mSize / N;
		}

		template<class T, size_t N>
		vk::Buffer & BasicArrayBuffer<T, N>::buffer() {
			return this->mBuffer;
		}

		template<class T, size_t N>
		vk::DeviceSize BasicArrayBuffer<T, N>::size() {
			return this->mSize;
		}

		template<class T, size_t N>
		ArrayBuffer<T, N>* BasicArrayBuffer<T, N>::map() {
			if (this->hasMapper) {
				throw ao::core::Exception("Buffer is already mapped");
			}

			// Map entire buffer
			this->mapper = ao::core::shared(this->device)->logical.mapMemory(this->memory, 0, this->mSize);

			this->hasMapper = true;
			return this;
		}
	}
}
