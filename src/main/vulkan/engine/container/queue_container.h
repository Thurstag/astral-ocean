// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <mutex>

#include <ao/core/utilities/types.h>
#include <ao/core/memory/map_container.hpp>
#include <vulkan/vulkan.hpp>

#include "../../utilities/queue.h"

namespace ao::vulkan {
    namespace structs {
        /**
         * @brief Queue structure
         *
         */
        struct Queue {
           public:
            vk::QueueFlags flags;
            QueueLevel level;
            u32 family_index;
            vk::Queue value;

            /**
             * @brief Construct a new Queue object
             *
             */
            Queue() : Queue(nullptr, 0, QueueLevel::ePrimary, vk::QueueFlags()) {}

            /**
             * @brief Construct a new Queue object
             *
             * @param queue Vulkan queue
             * @param family_index Family index
             * @param level Level
             * @param flags Flags
             */
            explicit Queue(vk::Queue queue, u32 family_index, QueueLevel level, vk::QueueFlags flags)
                : value(queue), family_index(family_index), level(level), flags(flags) {}
        };
    };  // namespace structs

    /**
     * @brief Queue container
     *
     */
    class QueueContainer : public core::MapContainer<std::string, structs::Queue> {
       public:
        /**
         * @brief Construct a new QueueContainer object
         *
         */
        QueueContainer();

        /**
         * @brief Destroy the QueueContainer object
         *
         */
        virtual ~QueueContainer() = default;

        /**
         * @brief Submit to a queue that supports {flag}
         *
         * @param flag Queue flag
         * @param submits Submissions
         * @param fence fence
         */
        void submit(vk::QueueFlagBits flag, vk::ArrayProxy<vk::SubmitInfo const> submits, vk::Fence fence = nullptr);

       protected:
        std::map<vk::QueueFlagBits, std::pair<size_t, std::mutex>> cursors;
    };
}  // namespace ao::vulkan
