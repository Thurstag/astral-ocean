// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <memory>

#include "../buffer.h"

namespace ao::vulkan {
    /**
     * @brief Array of buffers
     *
     * @tparam T Buffer type
     */
    template<class T>
    class DynamicArrayBuffer : public virtual Buffer {
       public:
        /**
         * @brief Construct a new DynamicArrayBuffer object
         *
         * @param count Count
         * @param device Device
         */
        explicit DynamicArrayBuffer(size_t count, std::shared_ptr<Device> device);

        /**
         * @brief Destroy the DynamicArrayBuffer object
         *
         */
        virtual ~DynamicArrayBuffer() = default;

        /**
         * @brief Update buffers
         *
         * @param data Data
         * @return DynamicArrayBuffer<T>* Buffer
         */
        virtual DynamicArrayBuffer<T>* update(std::vector<T> const& data) = 0;

        /**
         * @brief Update a fragment of the array
         *
         * @param index Fragment's index
         * @param data Data
         * @return DynamicArrayBuffer<T>* Buffer
         */
        virtual DynamicArrayBuffer<T>* updateFragment(std::size_t index, T const* data) = 0;

        /**
         * @brief Capacity
         *
         * @return size_t Capacity
         */
        size_t capacity() const;

        virtual bool hasBuffer() const override;

       protected:
        size_t count;
        bool has_buffer;
    };

    template<class T>
    DynamicArrayBuffer<T>::DynamicArrayBuffer(size_t count, std::shared_ptr<Device> device) : Buffer(device), count(count), has_buffer(false) {}

    template<class T>
    size_t DynamicArrayBuffer<T>::capacity() const {
        return this->count;
    }

    template<class T>
    bool DynamicArrayBuffer<T>::hasBuffer() const {
        return this->has_buffer;
    }

    /**
     * @brief Array of buffers
     *
     * @tparam T Buffer type
     * @tparam N Array's size
     */
    template<class T, size_t N>
    class ArrayBuffer : public virtual Buffer {
       public:
        /**
         * @brief Construct a new ArrayBuffer object
         *
         * @param device Device
         */
        ArrayBuffer(std::shared_ptr<Device> device);

        /**
         * @brief Destroy the ArrayBuffer object
         *
         */
        virtual ~ArrayBuffer() = default;

        /**
         * @brief Update buffers
         *
         * @param data Data
         * @return ArrayBuffer<T, N>* Buffer
         */
        virtual ArrayBuffer<T, N>* update(std::array<T, N> const& data) = 0;

        /**
         * @brief Update a fragment of the array
         *
         * @param index Fragment's index
         * @param data Data
         * @return DynamicArrayBuffer<T>* Buffer
         */
        virtual ArrayBuffer<T, N>* updateFragment(std::size_t index, T const* data) = 0;

        virtual bool hasBuffer() const override;

       protected:
        bool has_buffer;
    };

    template<class T, size_t N>
    ArrayBuffer<T, N>::ArrayBuffer(std::shared_ptr<Device> device) : Buffer(device), has_buffer(false) {}

    template<class T, size_t N>
    bool ArrayBuffer<T, N>::hasBuffer() const {
        return has_buffer;
    }
}  // namespace ao::vulkan
