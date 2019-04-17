// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <ao/core/memory/ptr_iterator.hpp>

#include "allocator/allocator.h"
#include "buffer.h"

namespace ao::vulkan {
    /**
     * @brief Vulkan buffer with array interface
     *
     * @tparam N Array's size
     * @tparam T Type of elements
     */
    template<size_t N, class T>
    class Array : public Buffer {
       public:
        /**
         * @brief Construct a new Array object
         *
         * @param allocator Allocator
         * @param usage Buffer usage
         */
        Array(std::shared_ptr<Allocator> allocator, vk::BufferUsageFlags usage = vk::BufferUsageFlags())
            : Buffer(allocator, N * allocator->alignSize(sizeof(T)), usage), stride(allocator->alignSize(sizeof(T))) {}

        /**
         * @brief Construct a new Array object
         *
         * @param value Value to copy in each element
         * @param allocator Allocator
         * @param usage Buffer usage
         */
        Array(T const& value, std::shared_ptr<Allocator> allocator, vk::BufferUsageFlags usage = vk::BufferUsageFlags());

        /**
         * @brief Destroy the Array object
         *
         */
        virtual ~Array() = default;

        /**
         * @brief Operator[]
         *
         * @param index Index
         * @return T const& Value
         */
        virtual T const& operator[](size_t index) const {
            return *(reinterpret_cast<T*>(static_cast<char*>(*this->buffer_info->ptr) + (index * this->stride)));
        }

        /**
         * @brief Update element at index {index} with {update_code}
         *
         * @param index Index
         * @param update_code Function to apply
         */
        virtual void update(size_t index, std::function<void(T&)> update_code);

        /**
         * @brief Get begin iterator
         *
         * @return core::PtrIterator<T const> Iterator
         */
        virtual core::PtrIterator<T const> begin() const {
            return core::PtrIterator<T const>(static_cast<T*>(*this->buffer_info->ptr), this->stride);
        }

        /**
         * @brief Get end iterator
         *
         * @return core::PtrIterator<T const> Iterator
         */
        virtual core::PtrIterator<T const> end() const {
            return core::PtrIterator<T const>(static_cast<T*>(*this->buffer_info->ptr) + N);
        }

        /**
         * @brief Get size
         *
         * @return size_t Size
         */
        virtual constexpr size_t size() const {
            return N;
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

       protected:
        size_t stride;

        /**
         * @brief Get a reference of object at index {index}
         *
         * @param index Index
         * @return T& Value
         */
        virtual T& at(size_t index) {
            return *(reinterpret_cast<T*>(static_cast<char*>(*this->buffer_info->ptr) + (index * this->stride)));
        }
    };

    template<size_t N, class T>
    Array<N, T>::Array(T const& value, std::shared_ptr<Allocator> allocator, vk::BufferUsageFlags usage) : Array<N, T>(allocator, usage) {
        // Fill
        for (size_t i = 0; i < N; i++) {
            this->at(i) = value;
        }

        // Notify
        this->allocator_->invalidate(*this->buffer_info, 0, this->buffer_info->size);
    }

    template<size_t N, class T>
    void Array<N, T>::update(size_t index, std::function<void(T&)> update_code) {
        // Run update
        update_code(this->at(index));

        // Notify
        this->allocator_->invalidate(*this->buffer_info, this->offset(index), sizeof(T));
    }
}  // namespace ao::vulkan