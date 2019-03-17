// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <ao/core/utilities/types.h>
#include <vulkan/vulkan.hpp>

namespace ao::vulkan {
    enum class QueueUsage { eManual, eAutomatic };

    /**
     * @brief Queue request
     *
     */
    struct QueueRequest {
       public:
        vk::QueueFlagBits flag;
        u32 automatic_count;
        u32 manual_count;

        /**
         * @brief Construct a new QueueRequest object
         *
         * @param flag Queue flag
         * @param manual_count Manual queue count
         * @param automatic_count Automatic queue count
         */
        explicit QueueRequest(vk::QueueFlagBits flag, u32 manual_count = 1, u32 automatic_count = 0)
            : flag(flag), manual_count(manual_count), automatic_count(automatic_count) {}

        /**
         * @brief Get total count
         *
         * @return u32 Count
         */
        u32 count() const {
            return this->automatic_count + this->manual_count;
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
