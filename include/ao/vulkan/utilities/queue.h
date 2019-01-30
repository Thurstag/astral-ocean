// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <ao/core/utilities/types.h>
#include <vulkan/vulkan.hpp>

namespace ao::vulkan {
    /// <summary>
    /// Queue request
    /// </summary>
    struct QueueRequest {
       public:
        vk::QueueFlagBits flag;
        u32 count;

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="flag">flag</param>
        /// <param name="count">Count</param>
        QueueRequest(vk::QueueFlagBits flag, u32 count = 1) : flag(flag), count(count) {}
    };

    /// <summary>
    /// Queue create info
    /// </summary>
    struct QueueCreateInfo {
       public:
        QueueRequest request;
        u32 family_index;
        u32 start_index;

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="request">Request</param>
        /// <param name="family_index">Family index</param>
        /// <param name="start_index">Start index</param>
        QueueCreateInfo(QueueRequest request, u32 family_index, u32 start_index)
            : request(request), family_index(family_index), start_index(start_index) {}
    };
}  // namespace ao::vulkan
