// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <ao/core/utilities/types.h>
#include <vulkan/vulkan.hpp>

namespace ao::vulkan {
    enum class QueueLevel { ePrimary, eSecondary };

    /**
     * @brief Queue request
     *
     */
    struct QueueRequest {
       public:
        vk::QueueFlagBits flag;
        u32 secondary_count;
        u32 primary_count;

        /**
         * @brief Construct a new QueueRequest object
         *
         * @param flag Queue flag
         * @param primary_count Primary queue count
         * @param secondary_count Secondary queue count
         */
        explicit QueueRequest(vk::QueueFlagBits flag, u32 primary_count = 1, u32 secondary_count = 0)
            : flag(flag), primary_count(primary_count), secondary_count(secondary_count) {}

        /**
         * @brief Get total count
         *
         * @return u32 Count
         */
        u32 count() {
            return this->secondary_count + this->primary_count;
        }
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
