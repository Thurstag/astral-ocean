// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <ao/core/memory/ptr_iterator.hpp>

#include "allocator/allocator.h"
#include "buffer.h"

namespace ao::vulkan {

    /**
     * @brief Vulkan buffer with std::vector interface
     *
     * @tparam T Type of elements
     */
    template<class T>
    class Vector : public Buffer {
       public:
        /**
         * @brief Construct a new Vector object
         *
         * @param capacity Capacity
         * @param allocator Allocator
         * @param usage Buffer usage
         */
        Vector(size_t capacity, std::shared_ptr<Allocator> allocator, vk::BufferUsageFlags usage = vk::BufferUsageFlags())
            : Buffer(allocator, capacity * allocator->alignSize(sizeof(T)), usage), capacity(capacity), stride(allocator->alignSize(sizeof(T))) {}

        /**
         * @brief Construct a new Vector object
         *
         * @param capacity Capacity
         * @param value Value to copy in each element
         * @param allocator Allocator
         * @param usage Buffer usage
         */
        Vector(size_t capacity, T const& value, std::shared_ptr<Allocator> allocator, vk::BufferUsageFlags usage = vk::BufferUsageFlags());

        /**
         * @brief Destroy the vector object
         *
         */
        virtual ~Vector() = default;

        /**
         * @brief Operator[]
         *
         * @param index Index
         * @return T& Value
         */
        virtual T& operator[](size_t index) {
            return *(reinterpret_cast<T*>(static_cast<char*>(*this->buffer_info->ptr) + (index * this->stride)));
        }

        /**
         * @brief Get a reference of object at index {index}
         *
         * @param index Index
         * @return T& Value
         */
        virtual T& at(size_t index) {
            return *(reinterpret_cast<T*>(static_cast<char*>(*this->buffer_info->ptr) + (index * this->stride)));
        }

        /**
         * @brief Get begin iterator
         *
         * @return core::PtrIterator<T> Iterator
         */
        virtual core::PtrIterator<T> begin() {
            return core::PtrIterator<T>(static_cast<T*>(*this->buffer_info->ptr), this->stride);
        }

        /**
         * @brief Get end iterator
         *
         * @return core::PtrIterator<T> Iterator
         */
        virtual core::PtrIterator<T> end() {
            return core::PtrIterator<T>(static_cast<T*>(*this->buffer_info->ptr) + this->capacity);
        }

        /**
         * @brief Get size
         *
         * @return size_t Size
         */
        virtual size_t size() const {
            return this->capacity;
        }

        /**
         * @brief Get offset of object at index {index}
         *
         * @param index Index
         * @return vk::DeviceSize Offset
         */
        virtual vk::DeviceSize offset(size_t index) const {
            return this->stride * index;
        }

        /**
         * @brief Invalidate {count} objects from {index}
         *
         * @param index Index
         * @param count Count
         */
        virtual void invalidate(size_t index, size_t count = 1) {
            this->allocator_->invalidate(*this->buffer_info, this->offset(index), count * this->stride);
        }

       protected:
        size_t capacity;
        size_t stride;
    };

    template<class T>
    Vector<T>::Vector(size_t capacity, T const& value, std::shared_ptr<Allocator> allocator, vk::BufferUsageFlags usage)
        : Vector<T>::Vector(capacity, allocator, usage) {
        // Fill
        for (size_t i = 0; i < capacity; i++) {
            this->at(i) = value;
        }

        // Notify
        this->allocator_->invalidate(*this->buffer_info, 0, this->buffer_info->size);
    }
}  // namespace ao::vulkan