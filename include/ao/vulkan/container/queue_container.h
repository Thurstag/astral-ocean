// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <functional>
#include <mutex>

#include <ao/core/utilities/optional.h>
#include <ao/core/utilities/types.h>
#include <ao/core/memory/map_container.hpp>
#include <vulkan/vulkan.hpp>

#include "../utilities/queue.h"
#include "../wrapper/fence.h"

namespace ao::vulkan {
    namespace structs {
        /**
         * @brief Queue structure
         *
         */
        struct Queue {
           public:
            vk::QueueFlags flags;
            QueueUsage usage;
            u32 family_index;
            vk::Queue value;

            /**
             * @brief Construct a new Queue object
             *
             */
            Queue() : Queue(nullptr, 0, QueueUsage::eManual, vk::QueueFlags()) {}

            /**
             * @brief Construct a new Queue object
             *
             * @param queue Vulkan queue
             * @param family_index Family index
             * @param usage Usage
             * @param flags Flags
             */
            explicit Queue(vk::Queue queue, u32 family_index, QueueUsage usage, vk::QueueFlags flags)
                : value(queue), family_index(family_index), usage(usage), flags(flags) {}
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
         * @param device Device
         * @param queue_create_info Queue create info
         * @param queue_families Queue families
         */
        QueueContainer(std::shared_ptr<vk::Device> device, std::vector<QueueCreateInfo> const& queue_create_info,
                       std::vector<vk::QueueFamilyProperties> const& queue_families);

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
         * @param fence Fence
         */
        void submit(vk::QueueFlagBits flag, vk::ArrayProxy<vk::SubmitInfo const> submits, Fence fence = Fence());

       protected:
        std::map<vk::QueueFlagBits, u32> queue_families;
        std::map<std::string, Fence> fences;
        std::shared_ptr<vk::Device> device;

        std::mutex mutex;

        /**
         * @brief Find a queue that supports {flag}
         *
         * @param flag Flag
         * @param predicate Predicate
         * @return std::optional<std::string> Queue's name
         */
        std::optional<std::string> findQueue(vk::QueueFlagBits flag, std::function<bool(structs::Queue const&)> predicate) const;
    };
}  // namespace ao::vulkan
