// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include "../buffer.h"

namespace ao::vulkan {
    /**
     * @brief Tuple buffer
     *
     * @tparam T Buffer types
     */
    template<class... T>
    class TupleBuffer : public Buffer {
       public:
        /**
         * @brief Construct a new TupleBuffer object
         *
         * @param device Device
         */
        explicit TupleBuffer(std::weak_ptr<Device> device);

        /**
         * @brief Destroy the TupleBuffer object
         *
         */
        virtual ~TupleBuffer() = default;

        /**
         * @brief Update buffer
         *
         * @param data Data
         * @return TupleBuffer<T...>* Buffer
         */
        virtual TupleBuffer<T...>* update(T const*... data) = 0;

        /**
         * @brief Update a fragment of the buffer
         *
         * @param index Index
         * @param data Data
         * @return TupleBuffer<T...>* Buffer
         */
        virtual TupleBuffer<T...>* updateFragment(std::size_t index, void* const data) = 0;

        virtual bool hasBuffer() const override;

       protected:
        bool has_buffer;
    };

    template<class... T>
    TupleBuffer<T...>::TupleBuffer(std::weak_ptr<Device> device) : Buffer(device), has_buffer(false) {}

    template<class... T>
    bool TupleBuffer<T...>::hasBuffer() const {
        return this->has_buffer;
    }
}  // namespace ao::vulkan
