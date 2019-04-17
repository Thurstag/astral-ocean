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
            constexpr auto ArgCount = sizeof...(T);

            return std::accumulate(sizes, sizes + ArgCount, size_t(0), std::plus<size_t>());
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
         * @return std::tuple_element<Index, std::tuple<T...>>::type const& Value
         */
        template<size_t Index, class... T>
        inline typename std::tuple_element<Index, std::tuple<T...>>::type const& get(Tuple<T...> const& tuple) {
            return *reinterpret_cast<typename std::tuple_element<Index, std::tuple<T...>>::type*>(static_cast<char*>(*tuple.info().ptr) +
                                                                                                  tuple.offset(Index));
        }

        /**
         * @brief Update element at index {Index} with {update_code}
         *
         * @tparam Index Index
         * @tparam T Tuple's types
         * @param tuple Tuple
         * @param update_code Function to apply
         */
        template<size_t Index, class... T>
        void update(Tuple<T...>& tuple, std::function<void(typename std::tuple_element<Index, std::tuple<T...>>::type&)> update_code) {
            update_code(*reinterpret_cast<typename std::tuple_element<Index, std::tuple<T...>>::type*>(static_cast<char*>(*tuple.info().ptr) +
                                                                                                       tuple.offset(Index)));

            // Notify allocator
            tuple.allocator()->invalidate(tuple.info(), tuple.offset(Index), sizeof(typename std::tuple_element<Index, std::tuple<T...>>::type));
        }
    }  // namespace
}  // namespace ao::vulkan