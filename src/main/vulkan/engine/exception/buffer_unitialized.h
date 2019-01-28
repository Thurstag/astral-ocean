// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <ao/core/exception/exception.h>

namespace ao::vulkan {
    class BufferUninitialized : public core::Exception {
       public:
        /// <summary>
        /// Constructor
        /// </summary>
        BufferUninitialized() : core::Exception("Buffer hasn't been initialized"){};

        /// <summary>
        /// Destructor
        /// </summary>
        virtual ~BufferUninitialized() = default;
    };
}  // namespace ao::vulkan
