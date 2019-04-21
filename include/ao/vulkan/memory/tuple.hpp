// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include "allocator/allocator.h"
#include "buffer.h"

namespace ao::vulkan {
    namespace {
        /**
         * @brief Calculate size of template arguments
         *
         * @tparam T Types
         * @param allocator Allocator
         * @return size_t Size
         */
        template<class... T>
        inline size_t sizeOf(std::shared_ptr<Allocator> allocator) {
            size_t sizes[] = {allocator->alignSize(sizeof(T))...};

            return std::accumulate(sizes, sizes + sizeof...(T), size_t(0), std::plus<size_t>());
        }
    }  // namespace

    /**
     * @brief Vulkan buffer with tuple interface
     *
     * @tparam T
     */
    template<class... T>
    class Tuple : public Buffer {
       public:
        /**
         * @brief Construct a new Tuple object
         *
         * @param allocator Allocator
         * @param usage Buffer usage
         */
        Tuple(std::shared_ptr<Allocator> allocator, vk::BufferUsageFlags usage = vk::BufferUsageFlags());

        /**
         * @brief Destroy the Tuple object
         *
         */
        virtual ~Tuple() = default;

        /**
         * @brief Get offset of object at index {index}
         *
         * @param index Index
         * @return vk::DeviceSize Offset
         */
        virtual vk::DeviceSize offset(size_t index) const {
            return this->offsets[index];
        }

        /**
         * @brief Invalidate {count} objects from {index}
         *
         * @param index Index
         * @param count Count
         */
        virtual void invalidate(size_t index, size_t count = 1) {
            this->allocator_->invalidate(*this->buffer_info, this->offset(index),
                                         std::accumulate(this->offsets.begin() + index, this->offsets.begin() + index + count, vk::DeviceSize(0),
                                                         std::plus<vk::DeviceSize>()));
        }

       protected:
        std::array<vk::DeviceSize, sizeof...(T)> offsets;
    };

    template<class... T>
    Tuple<T...>::Tuple(std::shared_ptr<Allocator> allocator, vk::BufferUsageFlags usage) : Buffer(allocator, sizeOf<T...>(allocator), usage) {
        size_t sizes[] = {allocator->alignSize(sizeof(T))...};

        vk::DeviceSize offset = 0;
        for (size_t i = 0; i < sizeof...(T); i++) {
            this->offsets[i] = offset;

            offset += sizes[i];
        }
    }

    namespace {
        /**
         * @brief Get element at index {Index}
         *
         * @tparam Index Index
         * @tparam T Tuple's types
         * @param tuple Tuple
         * @return std::tuple_element<Index, std::tuple<T...>>::type& Value
         */
        template<size_t Index, class... T>
        inline typename std::tuple_element<Index, std::tuple<T...>>::type& get(Tuple<T...>& tuple) {
            return *reinterpret_cast<typename std::tuple_element<Index, std::tuple<T...>>::type*>(static_cast<char*>(*tuple.info().ptr) +
                                                                                                  tuple.offset(Index));
        }
    }  // namespace
}  // namespace ao::vulkan