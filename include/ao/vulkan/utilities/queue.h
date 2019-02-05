// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <ao/core/utilities/types.h>
#include <vulkan/vulkan.hpp>

namespace ao::vulkan {
    /**
     * @brief Queue request
     *
     */
    struct QueueRequest {
       public:
        vk::QueueFlagBits flag;
        u32 count;

        /**
         * @brief Construct a new QueueRequest object
         *
         * @param flag Queue flag
         * @param count Queue count
         */
        explicit QueueRequest(vk::QueueFlagBits flag, u32 count = 1) : flag(flag), count(count) {}
    };

    /**
     * @brief Queue create info
     *
     */
    struct QueueCreateInfo {
       public:
        QueueRequest request;
        u32 family_index;
        u32 start_index;

        /**
         * @brief Construct a new QueueCreateInfo object
         *
         * @param request Request
         * @param family_index Family index
         * @param start_index Start index
         */
        QueueCreateInfo(QueueRequest request, u32 family_index, u32 start_index)
            : request(request), family_index(family_index), start_index(start_index) {}
    };
}  // namespace ao::vulkan
