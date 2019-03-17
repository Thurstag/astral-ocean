// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <initializer_list>

#include <ao/core/exception/index_out_of_range.h>

#include "../../exception/buffer_unitialized.h"
#include "../../utilities/device.h"
#include "buffer.hpp"

namespace ao::vulkan {
    /**
     * @brief Basic implementation of TupleBuffer
     *
     * @tparam T Buffer types
     */
    template<class... T>
    class BasicTupleBuffer : public TupleBuffer<T...> {
       public:
        /**
         * @brief Construct a new BasicTupleBuffer object
         *
         * @param device Device
         */
        explicit BasicTupleBuffer(std::shared_ptr<Device> device);

        /**
         * @brief Construct a new BasicTupleBuffer object
         *
         */
        BasicTupleBuffer(BasicTupleBuffer const&) = delete;

        /**
         * @brief Destroy the BasicTupleBuffer object
         *
         */
        virtual ~BasicTupleBuffer();

        /**
         * @brief Operator =
         *
         * @return BasicTupleBuffer& This
         */
        BasicTupleBuffer& operator=(BasicTupleBuffer const&) = delete;

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
         * @param sizes Sizes
         * @return BasicTupleBuffer<T...>* Buffer
         */
        BasicTupleBuffer<T...>* init(vk::BufferUsageFlags usage_flags, vk::SharingMode sharing_mode, vk::MemoryPropertyFlags memory_flags,
                                     std::initializer_list<vk::DeviceSize> const& sizes);

        TupleBuffer<T...>* update(T const*... data) override;
        TupleBuffer<T...>* updateFragment(std::size_t index, void* const data) override;
        vk::Buffer buffer() override;
        vk::DeviceSize size() const override;
        vk::DeviceSize offset(size_t index) const override;
        BasicTupleBuffer<T...>* map() override;

       protected:
        std::vector<std::pair<vk::DeviceSize, void*>> fragments;  // In pair, First = fragment's size / Second fragment's mapper
        vk::MemoryPropertyFlags memory_flags;
        std::vector<vk::DeviceSize> offsets;

        vk::DeviceMemory memory;
        vk::DeviceSize size_;
        vk::Buffer buffer_;
        bool has_mapper;
    };

    template<class... T>
    BasicTupleBuffer<T...>::BasicTupleBuffer(std::shared_ptr<Device> device) : TupleBuffer<T...>(device), Buffer(device), has_mapper(false) {
        this->fragments.resize(sizeof...(T));
        this->offsets.resize(sizeof...(T));
    }

    template<class... T>
    BasicTupleBuffer<T...>::~BasicTupleBuffer() {
        BasicTupleBuffer<T...>::free();
    }

    template<class... T>
    void BasicTupleBuffer<T...>::free() {
        if (this->has_mapper) {
            this->device->logical()->unmapMemory(this->memory);
            this->has_mapper = false;
        }
        if (this->has_buffer) {
            this->device->logical()->destroyBuffer(this->buffer_);
            this->device->logical()->freeMemory(this->memory);
            this->has_buffer = false;
        }
    }

    template<class... T>
    BasicTupleBuffer<T...>* BasicTupleBuffer<T...>::map() {
        if (this->has_mapper) {
            throw ao::core::Exception("Buffer is already mapped");
        }

        // Map each fragment
        u64 offset = 0;
        for (size_t i = 0; i < sizeof...(T); i++) {
            this->fragments[i].second = this->device->logical()->mapMemory(this->memory, offset, this->fragments[i].first);

            this->offsets[i] = offset;
            offset += this->fragments[i].first;
        }

        this->has_mapper = true;
        return this;
    }

    template<class... T>
    BasicTupleBuffer<T...>* BasicTupleBuffer<T...>::init(vk::BufferUsageFlags usage_flags, vk::SharingMode sharing_mode,
                                                         vk::MemoryPropertyFlags memory_flags, std::initializer_list<vk::DeviceSize> const& sizes) {
        if (this->hasBuffer()) {
            BasicTupleBuffer<T...>::free();
        }

        // Check sizes' size
        if (sizes.size() != sizeof...(T)) {
            throw core::Exception("Sizes argument should have the same size as template arguments");
        }

        // Init map
        size_t i = 0;
        for (auto& size : sizes) {
            this->fragments[i].first = size;
            i++;
        }

        // Get total size
        this->size_ = std::accumulate(sizes.begin(), sizes.end(), vk::DeviceSize(0), std::plus<vk::DeviceSize>());

        // Create buffer
        this->buffer_ = this->device->logical()->createBuffer(vk::BufferCreateInfo(vk::BufferCreateFlags(), this->size_, usage_flags, sharing_mode));

        // Get memory requirements
        vk::MemoryRequirements mem_requirements = this->device->logical()->getBufferMemoryRequirements(this->buffer_);

        // Allocate memory
        this->memory = this->device->logical()->allocateMemory(vk::MemoryAllocateInfo(
            mem_requirements.size, ao::vulkan::utilities::memoryType(this->device->physical(), mem_requirements.memoryTypeBits, memory_flags)));

        // Bind memory and buffer
        this->device->logical()->bindBufferMemory(this->buffer_, this->memory, 0);
        this->memory_flags = memory_flags;
        this->has_buffer = true;

        return this;
    }

    template<class... T>
    TupleBuffer<T...>* BasicTupleBuffer<T...>::update(T const*... data) {
        if (!this->hasBuffer()) {
            throw BufferUninitialized();
        }
        std::vector<void const*> _data = {data...};

        // Map memory
        if (!this->has_mapper) {
            this->map();
        }

        // Update fragments
        for (size_t i = 0; i < _data.size(); i++) {
            std::memcpy(this->fragments[i].second, _data[i], this->fragments[i].first);
        }

        // Notify changes
        if (!(this->memory_flags & vk::MemoryPropertyFlagBits::eHostCoherent)) {
            this->device->logical()->flushMappedMemoryRanges(vk::MappedMemoryRange(this->memory, 0, this->size_));
        }

        return this;
    }

    template<class... T>
    TupleBuffer<T...>* BasicTupleBuffer<T...>::updateFragment(std::size_t index, void* const data) {
        if (!this->hasBuffer()) {
            throw BufferUninitialized();
        }

        // Check index
        if (index >= sizeof...(T)) {
            throw core::IndexOutOfRangeException(std::make_pair(static_cast<u64>(0), static_cast<u64>(sizeof...(T))));
        }

        // Map memory
        if (!this->has_mapper) {
            this->map();
        }

        // Copy into buffer
        std::memcpy(this->fragments[index].second, data, this->fragments[index].first);

        // Notify changes
        if (!(this->memory_flags & vk::MemoryPropertyFlagBits::eHostCoherent)) {
            this->device->logical()->flushMappedMemoryRanges(vk::MappedMemoryRange(this->memory, this->offset(index), this->fragments[index].first));
        }

        return this;
    }

    template<class... T>
    vk::Buffer BasicTupleBuffer<T...>::buffer() {
        return this->buffer_;
    }

    template<class... T>
    vk::DeviceSize BasicTupleBuffer<T...>::size() const {
        return this->size_;
    }

    template<class... T>
    vk::DeviceSize BasicTupleBuffer<T...>::offset(size_t index) const {
        if (index >= sizeof...(T)) {
            throw core::IndexOutOfRangeException(std::make_pair(static_cast<u64>(0), static_cast<u64>(sizeof...(T))));
        }
        return this->offsets[index];
    }
}  // namespace ao::vulkan
