// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#include "queue_container.h"

#include <optional>

#include <ao/core/exception/exception.h>
#include <fmt/format.h>

ao::vulkan::QueueContainer::QueueContainer() {
    // clang-format off
    std::array<vk::QueueFlagBits, 5> flags = {
        vk::QueueFlagBits::eGraphics,  vk::QueueFlagBits::eCompute,       
        vk::QueueFlagBits::eTransfer, vk::QueueFlagBits::eProtected, 
        vk::QueueFlagBits::eSparseBinding
    };
    // clang-format on

    // Init cursors
    for (auto flag : flags) {
        this->cursors[flag].first = 0;
    }
}

void ao::vulkan::QueueContainer::submit(vk::QueueFlagBits flag, vk::ArrayProxy<vk::SubmitInfo const> submits, vk::Fence fence) {
    std::optional<std::string> secondary_queue_key;
    std::optional<std::string> primary_queue_key;
    std::optional<size_t> secondary_queue_cursor;
    std::optional<size_t> primary_queue_cursor;

    // Lock
    std::lock_guard<std::mutex> lock(this->cursors[flag].second);

    // Search best queue
    size_t i = 0;
    for (auto [key, queue] : this->map) {
        if (queue.flags & flag) {
            if (i <= this->cursors[flag].first) {
                if (queue.level == ao::vulkan::QueueLevel::eSecondary && !secondary_queue_key) {
                    secondary_queue_key = key;
                    secondary_queue_cursor = i;
                } else if (queue.level == ao::vulkan::QueueLevel::ePrimary && !primary_queue_key) {
                    primary_queue_key = key;
                    primary_queue_cursor = i;
                }
            } else {
                if (queue.level == ao::vulkan::QueueLevel::eSecondary) {
                    secondary_queue_key = key;
                    secondary_queue_cursor = i;

                    break;
                } else if (queue.level == ao::vulkan::QueueLevel::ePrimary) {
                    primary_queue_key = key;
                    primary_queue_cursor = i;
                }
            }
        }

        i++;
    }

    if (secondary_queue_key) {
        this->cursors[flag].first = *secondary_queue_cursor;

        this->map[*secondary_queue_key].value.submit(submits, fence);
    } else if (primary_queue_key) {
        this->cursors[flag].first = *primary_queue_cursor;

        this->map[*primary_queue_key].value.submit(submits, fence);
    } else {
        throw ao::core::Exception(fmt::format("Fail to find a queue that supports {}", vk::to_string(flag)));
    }
}