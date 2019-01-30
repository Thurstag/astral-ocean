// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <ao/core/utilities/types.h>
#include <ao/core/memory/map_container.hpp>
#include <vulkan/vulkan.hpp>

namespace ao::vulkan {
    namespace structs {
        struct Queue {
           public:
            u32 family_index;
            vk::Queue queue;

            /// <summary>
            /// Constructor
            /// </summary>
            Queue() : Queue(nullptr, 0) {}

            /// <summary>
            /// Constructor
            /// </summary>
            /// <param name="queue">Queue</param>
            /// <param name="family_index">Family index</param>
            Queue(vk::Queue queue, u32 family_index) : queue(queue), family_index(family_index) {}
        };
    };  // namespace structs

    /// <summary>
    /// Queue container
    /// </summary>
    class QueueContainer : public core::MapContainer<std::string, structs::Queue> {
       public:
        /// <summary>
        /// Constructor
        /// </summary>
        QueueContainer() = default;

        /// <summary>
        /// Destructor
        /// </summary>
        virtual ~QueueContainer() = default;
    };
}  // namespace ao::vulkan
