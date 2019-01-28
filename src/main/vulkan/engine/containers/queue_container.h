// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <ao/core/utilities/types.h>
#include <ao/core/memory/map_container.hpp>
#include <vulkan/vulkan.hpp>

namespace ao::vulkan {
    namespace structs {
        struct QueueData {
            vk::QueueFamilyProperties properties;
            vk::Queue queue;
            u32 index;

            /// <summary>
            /// Constructor
            /// </summary>
            QueueData() = default;

            /// <summary>
            /// Constructor
            /// </summary>
            /// <param name="queue">Queue</param>
            /// <param name="index">Index</param>
            /// <param name="properties">Properties</param>
            explicit QueueData(vk::Queue queue, u32 index, vk::QueueFamilyProperties properties)
                : queue(queue), index(index), properties(properties) {}
        };
    }  // namespace structs

    /// <summary>
    /// Queue container
    /// </summary>
    class QueueContainer : public core::MapContainer<vk::QueueFlagBits, structs::QueueData> {
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
