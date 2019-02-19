// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <ao/core/exception/exception.h>

namespace ao::vulkan {
    /**
     * @brief Vulkan buffer uninitialized exception
     *
     */
    class BufferUninitialized : public core::Exception {
       public:
        /**
         * @brief Construct a new BufferUninitialized object
         *
         */
        BufferUninitialized() : core::Exception("Buffer hasn't been initialized"){};

        /**
         * @brief Destroy the BufferUninitialized object
         *
         */
        virtual ~BufferUninitialized() = default;
    };
}  // namespace ao::vulkan
