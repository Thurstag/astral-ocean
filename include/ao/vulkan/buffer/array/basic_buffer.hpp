// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <ao/core/exception/index_out_of_range.h>

#include "../../exception/buffer_unitialized.h"
#include "buffer.hpp"

namespace ao::vulkan {
    /**
     * @brief Basic implementation of DynamicArrayBuffer
     *
     * @tparam T Buffer type
     */
    template<class T>
    class BasicDynamicArrayBuffer : public DynamicArrayBuffer<T> {
       public:
        /**
         * @brief Construct a new BasicDynamicArrayBuffer object
         *
         * @param count Count
         * @param device Device
         */
        BasicDynamicArrayBuffer(size_t count, std::shared_ptr<Device> device);

        /**
         * @brief Construct a new BasicDynamicArrayBuffer object
         *
         */
        BasicDynamicArrayBuffer(BasicDynamicArrayBuffer const&) = delete;

        /**
         * @brief Destroy the BasicDynamicArrayBuffer object
         *
         */
        virtual ~BasicDynamicArrayBuffer();

        /**
         * @brief Operator =
         *
         * @return BasicDynamicArrayBuffer& This
         */
        BasicDynamicArrayBuffer& operator=(BasicDynamicArrayBuffer const&) = delete;

        /**
         * @brief Free buffer
         *
         */
        void free() override;

        /**
         * @brief Initialize buffer
         *
         * @param usage_flags Usage flags
         * @param sharing_mode Sharing mode
         * @param memory_flags Memory flags
         * @param size Size
         * @return BasicArrayBuffer<T, N>* Buffer
         */
        BasicDynamicArrayBuffer<T>* init(vk::BufferUsageFlags usage_flags, vk::SharingMode sharing_mode, vk::MemoryPropertyFlags memory_flags,
                                         vk::DeviceSize size);

        DynamicArrayBuffer<T>* update(std::vector<T> const& data) override;
        DynamicArrayBuffer<T>* updateFragment(std::size_t index, T const* data) override;
        vk::DeviceSize offset(size_t index) const override;
        vk::Buffer buffer() override;
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
    BasicDynamicArrayBuffer<T>::BasicDynamicArrayBuffer(size_t count, std::shared_ptr<Device> device)
        : DynamicArrayBuffer<T>(count, device), Buffer(device), has_mapper(false), mapper(nullptr) {}

    template<class T>
    BasicDynamicArrayBuffer<T>::~BasicDynamicArrayBuffer() {
        BasicDynamicArrayBuffer<T>::free();
    }

    template<class T>
    void BasicDynamicArrayBuffer<T>::free() {
        if (this->has_mapper) {
            this->device->logical->unmapMemory(this->memory);
            this->has_mapper = false;
        }
        if (this->has_buffer) {
            this->device->logical->destroyBuffer(this->buffer_);
            this->device->logical->freeMemory(this->memory);
            this->has_buffer = false;
        }
    }

    template<class T>
    BasicDynamicArrayBuffer<T>* BasicDynamicArrayBuffer<T>::init(vk::BufferUsageFlags usage_flags, vk::SharingMode sharing_mode,
                                                                 vk::MemoryPropertyFlags memory_flags, vk::DeviceSize size) {
        if (this->hasBuffer()) {
            BasicDynamicArrayBuffer<T>::free();
        }

        // Get total size
        this->size_ = size * this->count;

        // Create buffer
        this->buffer_ = this->device->logical->createBuffer(vk::BufferCreateInfo(vk::BufferCreateFlags(), this->size_, usage_flags, sharing_mode));

        // Get memory requirements
        vk::MemoryRequirements mem_requirements = this->device->logical->getBufferMemoryRequirements(this->buffer_);

        // Allocate memory
        this->memory = this->device->logical->allocateMemory(
            vk::MemoryAllocateInfo(mem_requirements.size, this->device->memoryType(mem_requirements.memoryTypeBits, memory_flags)));

        // Bind memory and buffer
        this->device->logical->bindBufferMemory(this->buffer_, this->memory, 0);
        this->memory_flags = memory_flags;
        this->has_buffer = true;

        return this;
    }

    template<class T>
    DynamicArrayBuffer<T>* BasicDynamicArrayBuffer<T>::update(std::vector<T> const& data) {
        if (!this->hasBuffer()) {
            throw BufferUninitialized();
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
            this->device->logical->flushMappedMemoryRanges(vk::MappedMemoryRange(this->memory, 0, this->size_));
        }

        return this;
    }

    template<class T>
    DynamicArrayBuffer<T>* BasicDynamicArrayBuffer<T>::updateFragment(std::size_t index, T const* data) {
        if (!this->hasBuffer()) {
            throw BufferUninitialized();
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
            this->device->logical->flushMappedMemoryRanges(vk::MappedMemoryRange(this->memory, this->offset(index), this->size_ / this->count));
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
    vk::Buffer BasicDynamicArrayBuffer<T>::buffer() {
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
        this->mapper = this->device->logical->mapMemory(this->memory, 0, this->size_);

        this->has_mapper = true;
        return this;
    }

    /**
     * @brief Basic implementation of ArrayBuffer
     *
     * @tparam T Buffer type
     * @tparam N Array's size
     */
    template<class T, size_t N>
    class BasicArrayBuffer : public ArrayBuffer<T, N> {
       public:
        /**
         * @brief Construct a new BasicArrayBuffer object
         *
         * @param device Device
         */
        explicit BasicArrayBuffer(std::shared_ptr<Device> device);

        /**
         * @brief Construct a new BasicDynamicArrayBuffer object
         *
         */
        BasicArrayBuffer(BasicArrayBuffer const&) = delete;

        /**
         * @brief Operator =
         *
         * @return BasicArrayBuffer& This
         */
        BasicArrayBuffer& operator=(BasicArrayBuffer const&) = delete;

        /**
         * @brief Destroy the BasicArrayBuffer object
         *
         */
        virtual ~BasicArrayBuffer();

        /**
         * @brief Free buffer
         *
         */
        void free() override;

        /**
         * @brief Initialize buffer
         *
         * @param usage_flags Usage flags
         * @param sharing_mode Sharing mode
         * @param memory_flags Memory flags
         * @param size Size
         * @return BasicArrayBuffer<T, N>* Buffer
         */
        BasicArrayBuffer<T, N>* init(vk::BufferUsageFlags usage_flags, vk::SharingMode sharing_mode, vk::MemoryPropertyFlags memory_flags,
                                     vk::DeviceSize size);

        ArrayBuffer<T, N>* update(std::array<T, N> const& data) override;
        ArrayBuffer<T, N>* updateFragment(std::size_t index, T const* data) override;
        vk::DeviceSize offset(size_t index) const override;
        vk::Buffer buffer() override;
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
    BasicArrayBuffer<T, N>::BasicArrayBuffer(std::shared_ptr<Device> device)
        : ArrayBuffer<T, N>(device), Buffer(device), has_mapper(false), mapper(nullptr) {}

    template<class T, size_t N>
    BasicArrayBuffer<T, N>::~BasicArrayBuffer() {
        BasicArrayBuffer<T, N>::free();
    }

    template<class T, size_t N>
    void BasicArrayBuffer<T, N>::free() {
        if (this->has_mapper) {
            this->device->logical->unmapMemory(this->memory);
            this->has_mapper = false;
        }
        if (this->has_buffer) {
            this->device->logical->destroyBuffer(this->buffer_);
            this->device->logical->freeMemory(this->memory);
            this->has_buffer = false;
        }
    }

    template<class T, size_t N>
    BasicArrayBuffer<T, N>* BasicArrayBuffer<T, N>::init(vk::BufferUsageFlags usage_flags, vk::SharingMode sharing_mode,
                                                         vk::MemoryPropertyFlags memory_flags, vk::DeviceSize size) {
        if (this->hasBuffer()) {
            BasicArrayBuffer<T, N>::free();
        }

        // Get total size
        this->size_ = size * N;

        // Create buffer
        this->buffer_ = this->device->logical->createBuffer(vk::BufferCreateInfo(vk::BufferCreateFlags(), this->size_, usage_flags, sharing_mode));

        // Get memory requirements
        vk::MemoryRequirements mem_requirements = this->device->logical->getBufferMemoryRequirements(this->buffer_);

        // Allocate memory
        this->memory = this->device->logical->allocateMemory(
            vk::MemoryAllocateInfo(mem_requirements.size, this->device->memoryType(mem_requirements.memoryTypeBits, memory_flags)));

        // Bind memory and buffer
        this->device->logical->bindBufferMemory(this->buffer_, this->memory, 0);
        this->memory_flags = memory_flags;
        this->has_buffer = true;

        return this;
    }

    template<class T, size_t N>
    ArrayBuffer<T, N>* BasicArrayBuffer<T, N>::update(std::array<T, N> const& data) {
        if (!this->hasBuffer()) {
            throw BufferUninitialized();
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
            this->device->logical->flushMappedMemoryRanges(vk::MappedMemoryRange(this->memory, 0, this->size_));
        }

        return this;
    }

    template<class T, size_t N>
    ArrayBuffer<T, N>* BasicArrayBuffer<T, N>::updateFragment(std::size_t index, T const* data) {
        if (!this->hasBuffer()) {
            throw BufferUninitialized();
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
            this->device->logical->flushMappedMemoryRanges(vk::MappedMemoryRange(this->memory, this->offset(index), this->size_ / N));
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
    vk::Buffer BasicArrayBuffer<T, N>::buffer() {
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
        this->mapper = this->device->logical->mapMemory(this->memory, 0, this->size_);

        this->has_mapper = true;
        return this;
    }
}  // namespace ao::vulkan
