// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <initializer_list>

#include <ao/core/exception/index_out_of_range.h>
#include <ao/core/utilities/pointers.h>

#include "buffer.hpp"

namespace ao::vulkan {
    /// <summary>
    /// BasicTupleBuffer class
    /// </summary>
    template<class... T>
    class BasicTupleBuffer : public TupleBuffer<T...> {
       public:
        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="device">Device</param>
        explicit BasicTupleBuffer(std::weak_ptr<Device> device);

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
        /// <param name="memory_flags">Memory flags</param>
        /// <param name="sizes">Fragment sizes</param>
        /// <returns>This</returns>
        BasicTupleBuffer<T...>* init(vk::BufferUsageFlags usageFlags, vk::SharingMode sharingMode, vk::MemoryPropertyFlags memoryFlags,
                                     std::initializer_list<vk::DeviceSize> const& sizes);

        TupleBuffer<T...>* update(T const*... data) override;
        TupleBuffer<T...>* updateFragment(std::size_t index, void* const data) override;
        vk::Buffer const& buffer() const override;
        vk::DeviceSize size() const override;
        vk::DeviceSize offset(size_t index) const override;

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
    BasicTupleBuffer<T...>::BasicTupleBuffer(std::weak_ptr<Device> device) : TupleBuffer<T...>(device), has_mapper(false) {
        this->fragments.resize(sizeof...(T));
        this->offsets.resize(sizeof...(T));
    }

    template<class... T>
    BasicTupleBuffer<T...>::~BasicTupleBuffer() {
        this->free();
    }

    template<class... T>
    void BasicTupleBuffer<T...>::free() {
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

    template<class... T>
    BasicTupleBuffer<T...>* BasicTupleBuffer<T...>::map() {
        if (this->has_mapper) {
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

        this->has_mapper = true;
        return this;
    }

    template<class... T>
    BasicTupleBuffer<T...>* BasicTupleBuffer<T...>::init(vk::BufferUsageFlags usageFlags, vk::SharingMode sharingMode,
                                                         vk::MemoryPropertyFlags memoryFlags, std::initializer_list<vk::DeviceSize> const& sizes) {
        if (this->hasBuffer()) {
            this->free();
        }
        auto _device = ao::core::shared(this->device);

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
        this->buffer_ = _device->logical.createBuffer(vk::BufferCreateInfo(vk::BufferCreateFlags(), this->size_, usageFlags, sharingMode));

        // Get memory requirements
        vk::MemoryRequirements memRequirements = _device->logical.getBufferMemoryRequirements(this->buffer_);

        // Allocate memory
        this->memory = _device->logical.allocateMemory(
            vk::MemoryAllocateInfo(memRequirements.size, _device->memoryType(memRequirements.memoryTypeBits, memoryFlags)));

        // Bind memory and buffer
        _device->logical.bindBufferMemory(this->buffer_, this->memory, 0);
        this->memory_flags = memoryFlags;
        this->has_buffer = true;

        return this;
    }

    template<class... T>
    TupleBuffer<T...>* BasicTupleBuffer<T...>::update(T const*... data) {
        if (!this->hasBuffer()) {
            throw core::Exception("Buffer hasn't been initialized");
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
            ao::core::shared(this->device)->logical.flushMappedMemoryRanges(vk::MappedMemoryRange(this->memory, 0, this->size_));
        }

        return this;
    }

    template<class... T>
    TupleBuffer<T...>* BasicTupleBuffer<T...>::updateFragment(std::size_t index, void* const data) {
        if (!this->hasBuffer()) {
            throw core::Exception("Buffer hasn't been initialized");
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
            ao::core::shared(this->device)
                ->logical.flushMappedMemoryRanges(vk::MappedMemoryRange(this->memory, this->offset(index), this->fragments[index].first));
        }

        return this;
    }

    template<class... T>
    vk::Buffer const& BasicTupleBuffer<T...>::buffer() const {
        return this->buffer_;
    }

    template<class... T>
    vk::DeviceSize BasicTupleBuffer<T...>::size() const {
        return this->size_;
    }

    template<class... T>
    vk::DeviceSize BasicTupleBuffer<T...>::offset(size_t index) const {
        if (!this->has_mapper) {
            throw ao::core::Exception("Buffer is not mapped");
        }
        if (index >= sizeof...(T)) {
            throw core::IndexOutOfRangeException(std::make_pair(static_cast<u64>(0), static_cast<u64>(sizeof...(T))));
        }
        return this->offsets[index];
    }
}  // namespace ao::vulkan
