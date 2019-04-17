// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <ao/core/exception/exception.h>
#include <fmt/format.h>

namespace ao::vulkan {
    /**
     * @brief
     *
     */
    class UnknownAllocation : public core::Exception {
       public:
        /**
         * @brief Construct a new UnknownAllocation object
         *
         */
        UnknownAllocation() : core::Exception("Unknown allocation"){};

        /**
         * @brief Destroy the UnknownAllocation object
         *
         */
        virtual ~UnknownAllocation() = default;
    };
}  // namespace ao::vulkan