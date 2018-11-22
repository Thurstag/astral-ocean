#pragma once

#include <ao/core/exception/index_out_of_range.h>
#include <ao/core/utilities/pointers.h>

#include "buffer.hpp"

namespace ao {
	namespace vulkan {
		template<class... T>
		class BasicTupleBuffer : public TupleBuffer<T...> {
		public:
			/// <summary>
			/// Constructor
			/// </summary>
			/// <param name="device">Device</param>
			BasicTupleBuffer(std::weak_ptr<Device> device);

			/// <summary>
			/// Destructor
			/// </summary>
			virtual ~BasicTupleBuffer();

			/// <summary>
			/// Method to free buffer
			/// </summary>
			void free();

			/// <summary>
			/// Method to map memory
			/// </summary>
			/// <returns>This</returns>
			BasicTupleBuffer<T...>* map();

			/// <summary>
			/// Method to init buffer
			/// 
			/// If object already stores a buffer, it will free the old one
			/// </summary>
			/// <param name="usageFlags">Usage flags</param>
			/// <param name="sharingMode">Sharing mode</param>
			/// <param name="memoryFlags">Memory flags</param>
			/// <param name="sizes">Fragment sizes</param>
			/// <returns>This</returns>
			BasicTupleBuffer<T...>* init(vk::BufferUsageFlags usageFlags, vk::SharingMode sharingMode, vk::MemoryPropertyFlags memoryFlags, std::vector<vk::DeviceSize> sizes);

			TupleBuffer<T...>* update(T*... data) override;
			TupleBuffer<T...>* updateFragment(std::size_t index, void* data) override;
			vk::Buffer& buffer() override;
			vk::DeviceSize size() override;
			vk::DeviceSize offset(size_t index) override;

		protected:
			std::vector<std::pair<vk::DeviceSize, void*>> fragments;    // In pair, First = fragment's size / Second fragment's mapper
			vk::MemoryPropertyFlags memoryFlags;
			std::vector<vk::DeviceSize> offsets;

			vk::DeviceMemory memory;
			vk::DeviceSize mSize;
			vk::Buffer mBuffer;

			bool hasMapper;
		};

		/* IMPLEMENTATION */

		template<class ...T>
		BasicTupleBuffer<T...>::BasicTupleBuffer(std::weak_ptr<Device> device) : TupleBuffer<T...>(device), hasMapper(false) {
			this->fragments.resize(sizeof...(T));
			this->offsets.resize(sizeof...(T));
		}

		template<class ...T>
		BasicTupleBuffer<T...>::~BasicTupleBuffer() {
			this->free();
		}

		template<class ...T>
		void BasicTupleBuffer<T...>::free() {
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

		template<class ...T>
		BasicTupleBuffer<T...>* BasicTupleBuffer<T...>::map() {
			if (this->hasMapper) {
				throw ao::core::Exception("Buffer is already mapped");
			}

			auto _device = ao::core::shared(this->device);
			u64 offset = 0;

			// Map each fragment
			for (size_t i = 0; i < sizeof...(T); i++) {
				this->fragments[i].second = _device->logical.mapMemory(this->memory, offset, this->fragments[i].first);

				this->offsets[i] = offset;
				offset += this->fragments[i].first;
			}

			this->hasMapper = true;
			return this;
		}

		template<class ...T>
		BasicTupleBuffer<T...>* BasicTupleBuffer<T...>::init(vk::BufferUsageFlags usageFlags, vk::SharingMode sharingMode, vk::MemoryPropertyFlags memoryFlags, std::vector<vk::DeviceSize> sizes) {
			if (this->hasBuffer()) {
				this->free();
			}
			auto _device = ao::core::shared(this->device);

			// Check sizes' size
			if (sizes.size() != sizeof...(T)) {
				throw core::Exception("Sizes argument should have the same size as template arguments");
			}

			// Init map
			for (size_t i = 0; i < sizes.size(); i++) {
				this->fragments[i].first = sizes[i];
			}

			// Get total size
			this->mSize = std::accumulate(sizes.begin(), sizes.end(), vk::DeviceSize(0), std::plus<vk::DeviceSize>());

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

		template<class ...T>
		TupleBuffer<T...>* BasicTupleBuffer<T...>::update(T*... data) {
			if (!this->hasBuffer()) {
				throw core::Exception("Buffer hasn't been initialized");
			}
			std::vector<void*> _data = { data... };

			// Map memory
			if (!this->hasMapper) {
				this->map();
			}

			// Update fragments
			for (size_t i = 0; i < _data.size(); i++) {
				std::memcpy(this->fragments[i].second, _data[i], this->fragments[i].first);
			}

			// Notify changes
			if (!(this->memoryFlags & vk::MemoryPropertyFlagBits::eHostCoherent)) {
				ao::core::shared(this->device)->logical.flushMappedMemoryRanges(vk::MappedMemoryRange(this->memory, 0, this->mSize));
			}

			return this;
		}

		template<class ...T>
		TupleBuffer<T...>* BasicTupleBuffer<T...>::updateFragment(std::size_t index, void* data) {
			if (!this->hasBuffer()) {
				throw core::Exception("Buffer hasn't been initialized");
			}

			// Check index
			if (index >= sizeof...(T)) {
				throw core::IndexOutOfRangeException(std::make_pair(static_cast<u64>(0), static_cast<u64>(sizeof...(T))));
			}

			// Map memory
			if (!this->hasMapper) {
				this->map();
			}

			// Copy into buffer
			std::memcpy(this->fragments[index].second, data, this->fragments[index].first);

			// Notify changes
			if (!(this->memoryFlags & vk::MemoryPropertyFlagBits::eHostCoherent)) {
				ao::core::shared(this->device)->logical.flushMappedMemoryRanges(vk::MappedMemoryRange(this->memory, this->offset(index), this->fragments[index].first));
			}

			return this;
		}

		template<class ...T>
		vk::Buffer & BasicTupleBuffer<T...>::buffer() {
			return this->mBuffer;
		}

		template<class ...T>
		vk::DeviceSize BasicTupleBuffer<T...>::size() {
			return this->mSize;
		}

		template<class ...T>
		vk::DeviceSize BasicTupleBuffer<T...>::offset(size_t index) {
			if (!this->hasMapper) {
				throw ao::core::Exception("Buffer is not mapped");
			}
			if (index >= sizeof...(T)) {
				throw core::IndexOutOfRangeException(std::make_pair(static_cast<u64>(0), static_cast<u64>(sizeof...(T))));
			}
			return this->offsets[index];
		}
	}
}