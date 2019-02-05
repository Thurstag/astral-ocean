// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <ao/core/utilities/types.h>
#include <ao/core/memory/map_container.hpp>
#include <vulkan/vulkan.hpp>

namespace ao::vulkan {
    namespace structs {
        /**
         * @brief Queue structure
         *
         */
        struct Queue {
           public:
            u32 family_index;
            vk::Queue queue;

            /**
             * @brief Construct a new Queue object
             *
             */
            Queue() : Queue(nullptr, 0) {}

            /**
             * @brief Construct a new Queue object
             *
             * @param queue Vulkan queue
             * @param family_index Family index
             */
            explicit Queue(vk::Queue queue, u32 family_index) : queue(queue), family_index(family_index) {}
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
        QueueContainer() = default;

        /**
         * @brief Destroy the QueueContainer object
         *
         */
        virtual ~QueueContainer() = default;
    };
}  // namespace ao::vulkan
