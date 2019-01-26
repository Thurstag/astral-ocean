// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <ao/core/exception/index_out_of_range.h>

#include "buffer.hpp"

namespace ao::vulkan {
    /// <summary>
    /// BasicDynamicArrayBuffer class
    /// </summary>
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
        /// <param name="usage_flags">Usage flags</param>
        /// <param name="sharing_mode">Sharing mode</param>
        /// <param name="memory_flags">Memory flags</param>
        /// <param name="size">Fragment size</param>
        /// <returns>This</returns>
        BasicDynamicArrayBuffer<T>* init(vk::BufferUsageFlags usage_flags, vk::SharingMode sharing_mode, vk::MemoryPropertyFlags memory_flags,
                                         vk::DeviceSize size);

        DynamicArrayBuffer<T>* update(std::vector<T> const& data) override;
        DynamicArrayBuffer<T>* updateFragment(std::size_t index, T const* data) override;
        vk::DeviceSize offset(size_t index) const override;
        vk::Buffer const& buffer() const override;
        vk::DeviceSize size() const override;
        DynamicArrayBuffer<T>* map() override;

       protected:
        vk::MemoryPropertyFlags memory_flags;

        vk::DeviceMemory memory;
        vk::DeviceSize size_;
        vk::Buffer buffer_;

        bool has_mapper;
        void* mapper;
    };

    template<class T>
    BasicDynamicArrayBuffer<T>::BasicDynamicArrayBuffer(size_t count, std::weak_ptr<Device> device)
        : DynamicArrayBuffer<T>(count, device), has_mapper(false) {}

    template<class T>
    BasicDynamicArrayBuffer<T>::~BasicDynamicArrayBuffer() {
        this->free();
    }

    template<class T>
    void BasicDynamicArrayBuffer<T>::free() {
        auto _device = ao::core::shared(this->device);

        if (this->has_mapper) {
            _device->logical.unmapMemory(this->memory);
            this->has_mapper = false;
        }
        if (this->has_buffer) {
            _device->logical.destroyBuffer(this->buffer_);
            _device->logical.freeMemory(this->memory);
            this->has_buffer = false;
        }
    }

    template<class T>
    BasicDynamicArrayBuffer<T>* BasicDynamicArrayBuffer<T>::init(vk::BufferUsageFlags usage_flags, vk::SharingMode sharing_mode,
                                                                 vk::MemoryPropertyFlags memory_flags, vk::DeviceSize size) {
        if (this->hasBuffer()) {
            this->free();
        }
        auto _device = ao::core::shared(this->device);

        // Get total size
        this->size_ = size * this->count;

        // Create buffer
        this->buffer_ = _device->logical.createBuffer(vk::BufferCreateInfo(vk::BufferCreateFlags(), this->size_, usage_flags, sharing_mode));

        // Get memory requirements
        vk::MemoryRequirements mem_requirements = _device->logical.getBufferMemoryRequirements(this->buffer_);

        // Allocate memory
        this->memory = _device->logical.allocateMemory(
            vk::MemoryAllocateInfo(mem_requirements.size, _device->memoryType(mem_requirements.memoryTypeBits, memory_flags)));

        // Bind memory and buffer
        _device->logical.bindBufferMemory(this->buffer_, this->memory, 0);
        this->memory_flags = memory_flags;
        this->has_buffer = true;

        return this;
    }

    template<class T>
    DynamicArrayBuffer<T>* BasicDynamicArrayBuffer<T>::update(std::vector<T> const& data) {
        if (!this->hasBuffer()) {
            throw core::Exception("Buffer hasn't been initialized");
        }

        // Map memory
        if (!this->has_mapper) {
            this->map();
        }

        // Check sizes
        if (data.size() != this->count) {
            throw core::Exception(fmt::format("Data argument size should be equal to: {0}", this->count));
        }

        // Update fragments
        for (size_t i = 0; i < this->count; i++) {
            std::memcpy((void*)((u64)this->mapper + i * this->size_ / this->count), &data[i], this->size_ / this->count);
        }

        // Notify changes
        if (!(this->memory_flags & vk::MemoryPropertyFlagBits::eHostCoherent)) {
            ao::core::shared(this->device)->logical.flushMappedMemoryRanges(vk::MappedMemoryRange(this->memory, 0, this->size_));
        }

        return this;
    }

    template<class T>
    DynamicArrayBuffer<T>* BasicDynamicArrayBuffer<T>::updateFragment(std::size_t index, T const* data) {
        if (!this->hasBuffer()) {
            throw core::Exception("Buffer hasn't been initialized");
        }

        // Check index
        if (index >= this->count) {
            throw core::IndexOutOfRangeException(std::make_pair(static_cast<u64>(0), static_cast<u64>(this->count)));
        }

        // Map memory
        if (!this->has_mapper) {
            this->map();
        }

        // Copy into buffer
        std::memcpy((void*)((u64)this->mapper + (index * this->size_ / this->count)), data, this->size_ / this->count);

        // Notify changes
        if (!(this->memory_flags & vk::MemoryPropertyFlagBits::eHostCoherent)) {
            ao::core::shared(this->device)
                ->logical.flushMappedMemoryRanges(vk::MappedMemoryRange(this->memory, this->offset(index), this->size_ / this->count));
        }

        return this;
    }

    template<class T>
    vk::DeviceSize BasicDynamicArrayBuffer<T>::offset(size_t index) const {
        if (index >= this->count) {
            throw core::IndexOutOfRangeException(std::make_pair(static_cast<u64>(0), static_cast<u64>(this->count)));
        }
        return index * this->size_ / this->count;
    }

    template<class T>
    vk::Buffer const& BasicDynamicArrayBuffer<T>::buffer() const {
        return this->buffer_;
    }

    template<class T>
    vk::DeviceSize BasicDynamicArrayBuffer<T>::size() const {
        return this->size_;
    }

    template<class T>
    DynamicArrayBuffer<T>* BasicDynamicArrayBuffer<T>::map() {
        if (this->has_mapper) {
            throw ao::core::Exception("Buffer is already mapped");
        }

        // Map entire buffer
        this->mapper = ao::core::shared(this->device)->logical.mapMemory(this->memory, 0, this->size_);

        this->has_mapper = true;
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
        /// <param name="usage_flags">Usage flags</param>
        /// <param name="sharing_mode">Sharing mode</param>
        /// <param name="memory_flags">Memory flags</param>
        /// <param name="size">Fragment size</param>
        /// <returns>This</returns>
        BasicArrayBuffer<T, N>* init(vk::BufferUsageFlags usage_flags, vk::SharingMode sharing_mode, vk::MemoryPropertyFlags memory_flags,
                                     vk::DeviceSize size);

        ArrayBuffer<T, N>* update(std::array<T, N> const& data) override;
        ArrayBuffer<T, N>* updateFragment(std::size_t index, T const* data) override;
        vk::DeviceSize offset(size_t index) const override;
        vk::Buffer const& buffer() const override;
        vk::DeviceSize size() const override;
        ArrayBuffer<T, N>* map() override;

       protected:
        vk::MemoryPropertyFlags memory_flags;

        vk::DeviceMemory memory;
        vk::DeviceSize size_;
        vk::Buffer buffer_;

        bool has_mapper;
        void* mapper;
    };

    template<class T, size_t N>
    BasicArrayBuffer<T, N>::BasicArrayBuffer(std::weak_ptr<Device> device) : ArrayBuffer<T, N>(device), has_mapper(false) {}

    template<class T, size_t N>
    BasicArrayBuffer<T, N>::~BasicArrayBuffer() {
        this->free();
    }

    template<class T, size_t N>
    void BasicArrayBuffer<T, N>::free() {
        auto _device = ao::core::shared(this->device);

        if (this->has_mapper) {
            _device->logical.unmapMemory(this->memory);
            this->has_mapper = false;
        }
        if (this->has_buffer) {
            _device->logical.destroyBuffer(this->buffer_);
            _device->logical.freeMemory(this->memory);
            this->has_buffer = false;
        }
    }

    template<class T, size_t N>
    BasicArrayBuffer<T, N>* BasicArrayBuffer<T, N>::init(vk::BufferUsageFlags usage_flags, vk::SharingMode sharing_mode,
                                                         vk::MemoryPropertyFlags memory_flags, vk::DeviceSize size) {
        if (this->hasBuffer()) {
            this->free();
        }
        auto _device = ao::core::shared(this->device);

        // Get total size
        this->size_ = size * N;

        // Create buffer
        this->buffer_ = _device->logical.createBuffer(vk::BufferCreateInfo(vk::BufferCreateFlags(), this->size_, usage_flags, sharing_mode));

        // Get memory requirements
        vk::MemoryRequirements mem_requirements = _device->logical.getBufferMemoryRequirements(this->buffer_);

        // Allocate memory
        this->memory = _device->logical.allocateMemory(
            vk::MemoryAllocateInfo(mem_requirements.size, _device->memoryType(mem_requirements.memoryTypeBits, memory_flags)));

        // Bind memory and buffer
        _device->logical.bindBufferMemory(this->buffer_, this->memory, 0);
        this->memory_flags = memory_flags;
        this->has_buffer = true;

        return this;
    }

    template<class T, size_t N>
    ArrayBuffer<T, N>* BasicArrayBuffer<T, N>::update(std::array<T, N> const& data) {
        if (!this->hasBuffer()) {
            throw core::Exception("Buffer hasn't been initialized");
        }

        // Map memory
        if (!this->has_mapper) {
            this->map();
        }

        // Update fragments
        for (size_t i = 0; i < N; i++) {
            std::memcpy((void*)((u64)this->mapper + i * this->size_ / N), &data[i], this->size_ / N);
        }

        // Notify changes
        if (!(this->memory_flags & vk::MemoryPropertyFlagBits::eHostCoherent)) {
            ao::core::shared(this->device)->logical.flushMappedMemoryRanges(vk::MappedMemoryRange(this->memory, 0, this->size_));
        }

        return this;
    }

    template<class T, size_t N>
    ArrayBuffer<T, N>* BasicArrayBuffer<T, N>::updateFragment(std::size_t index, T const* data) {
        if (!this->hasBuffer()) {
            throw core::Exception("Buffer hasn't been initialized");
        }

        // Check index
        if (index >= N) {
            throw core::IndexOutOfRangeException(std::make_pair(static_cast<u64>(0), static_cast<u64>(N)));
        }

        // Map memory
        if (!this->has_mapper) {
            this->map();
        }

        // Copy into buffer
        std::memcpy((void*)((u64)this->mapper + (index * this->size_ / N)), data, this->size_ / N);

        // Notify changes
        if (!(this->memory_flags & vk::MemoryPropertyFlagBits::eHostCoherent)) {
            ao::core::shared(this->device)
                ->logical.flushMappedMemoryRanges(vk::MappedMemoryRange(this->memory, this->offset(index), this->size_ / N));
        }

        return this;
    }

    template<class T, size_t N>
    vk::DeviceSize BasicArrayBuffer<T, N>::offset(size_t index) const {
        if (index >= N) {
            throw core::IndexOutOfRangeException(std::make_pair(static_cast<u64>(0), static_cast<u64>(N)));
        }
        return index * this->size_ / N;
    }

    template<class T, size_t N>
    vk::Buffer const& BasicArrayBuffer<T, N>::buffer() const {
        return this->buffer_;
    }

    template<class T, size_t N>
    vk::DeviceSize BasicArrayBuffer<T, N>::size() const {
        return this->size_;
    }

    template<class T, size_t N>
    ArrayBuffer<T, N>* BasicArrayBuffer<T, N>::map() {
        if (this->has_mapper) {
            throw ao::core::Exception("Buffer is already mapped");
        }

        // Map entire buffer
        this->mapper = ao::core::shared(this->device)->logical.mapMemory(this->memory, 0, this->size_);

        this->has_mapper = true;
        return this;
    }
}  // namespace ao::vulkan
