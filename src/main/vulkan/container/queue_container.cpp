// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#include "queue_container.h"

#include <optional>

#include <ao/core/exception/exception.h>
#include <fmt/format.h>

ao::vulkan::QueueContainer::QueueContainer(vk::Device device, std::vector<QueueCreateInfo> const& queue_create_info,
                                           std::vector<vk::QueueFamilyProperties> const& queue_families) {
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

    // Build queue map
    for (auto& create_info : queue_create_info) {
        u32 j = 0;

        // Primary queues
        for (u32 i = 0; i < create_info.request.primary_count; i++) {
            this->map[fmt::format("{}{}", vk::to_string(create_info.request.flag), j == 0 ? "" : fmt::format("-{}", j + 1))] =
                ao::vulkan::structs::Queue(device.getQueue(create_info.family_index, create_info.start_index + j), create_info.family_index,
                                           ao::vulkan::QueueLevel::ePrimary, queue_families[create_info.family_index].queueFlags);

            j++;
        }

        // Secondary queues
        for (u32 i = 0; i < create_info.request.secondary_count; i++) {
            this->map[fmt::format("{}{}", vk::to_string(create_info.request.flag), j == 0 ? "" : fmt::format("-{}", j + 1))] =
                ao::vulkan::structs::Queue(device.getQueue(create_info.family_index, create_info.start_index + j), create_info.family_index,
                                           ao::vulkan::QueueLevel::eSecondary, queue_families[create_info.family_index].queueFlags);

            j++;
        }

        this->queue_families[create_info.request.flag] = create_info.family_index;
    }
}

void ao::vulkan::QueueContainer::submit(vk::QueueFlagBits flag, vk::ArrayProxy<vk::SubmitInfo const> submits, vk::Fence fence) {
    std::optional<std::string> queue;

    // Find queue
    switch (flag) {
        case vk::QueueFlagBits::eTransfer:
            queue = this->findQueue(flag, [families = &this->queue_families, flag](ao::vulkan::structs::Queue const& queue) {
                return queue.family_index == (*families)[flag];
            });
            break;

        case vk::QueueFlagBits::eCompute:
        case vk::QueueFlagBits::eGraphics:
            queue = this->findQueue(flag, [flag](ao::vulkan::structs::Queue const& queue) { return (queue.flags & flag) ? true : false; });
            break;

        default:
            throw ao::core::Exception(fmt::format("Unsupported vk::QueueFlagBits: {}", vk::to_string(flag)));
    }

    // Check queue
    if (!queue) {
        throw ao::core::Exception(fmt::format("Fail to find a queue that supports: {}", vk::to_string(flag)));
    }

    // Submit
    this->LOGGER << ao::core::Logger::Level::trace << fmt::format("Submit into '{}' queue for flag: {}", *queue, vk::to_string(flag));
    this->map[*queue].value.submit(submits, fence);
}

std::optional<std::string> ao::vulkan::QueueContainer::findQueue(vk::QueueFlagBits flag,
                                                                 std::function<bool(ao::vulkan::structs::Queue const&)> predicate) {
    std::optional<std::string> secondary_queue_key;
    std::optional<std::string> primary_queue_key;
    std::optional<size_t> secondary_queue_cursor;
    std::optional<size_t> primary_queue_cursor;

    // Lock
    std::lock_guard<std::mutex> lock(this->cursors[flag].second);

    // Search best queue
    size_t i = 0;
    for (auto [key, queue] : this->map) {
        if (predicate(queue)) {
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

        return *secondary_queue_key;
    } else if (primary_queue_key) {
        this->cursors[flag].first = *primary_queue_cursor;

        return *primary_queue_key;
    }
    return std::nullopt;
}