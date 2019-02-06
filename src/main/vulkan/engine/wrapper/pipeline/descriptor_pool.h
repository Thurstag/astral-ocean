// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include "../device.h"

namespace ao::vulkan {
    /**
     * @brief vk::DescriptorPool wrapper
     *
     */
    class DescriptorPool {
       public:
        /**
         * @brief Construct a new DescriptorPool object
         *
         */
        DescriptorPool() = default;

        /**
         * @brief Construct a new DescriptorPool object
         *
         * @param device Device
         * @param pool Pool
         */
        DescriptorPool(std::weak_ptr<Device> device, std::shared_ptr<vk::DescriptorPool> pool);

        /**
         * @brief Destroy the DescriptorPool object
         *
         */
        virtual ~DescriptorPool();

        /**
         * @brief Allocate descriptor sets
         *
         * @param count Count
         * @param descriptor_layouts Layouts
         * @return std::vector<vk::DescriptorSet> Descriptor sets
         */
        std::vector<vk::DescriptorSet> allocateDescriptorSets(u32 count, std::vector<vk::DescriptorSetLayout> descriptor_layouts);

        /**
         * @brief Get descriptor sets
         *
         * @return std::vector<vk::DescriptorSet> Descriptor sets
         */
        std::vector<vk::DescriptorSet> const& descriptorSets();

       protected:
        std::weak_ptr<Device> device;

        std::vector<vk::DescriptorSet> descriptor_sets;
        std::shared_ptr<vk::DescriptorPool> pool;
    };

}  // namespace ao::vulkan