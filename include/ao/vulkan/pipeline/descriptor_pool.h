// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include "../wrapper/device.h"

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
         * @param device Device
         * @param create_info Create info
         */
        DescriptorPool(std::weak_ptr<Device> device, vk::DescriptorPoolCreateInfo create_info);

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
        std::vector<vk::DescriptorSet> allocateDescriptorSets(u32 count, vk::ArrayProxy<vk::DescriptorSetLayout> descriptor_layouts);

        /**
         * @brief Get descriptor sets
         *
         * @return std::vector<vk::DescriptorSet> Descriptor sets
         */
        std::vector<vk::DescriptorSet> const& descriptorSets() const {
            return this->descriptor_sets;
        }

       protected:
        std::weak_ptr<Device> device;

        std::vector<vk::DescriptorSet> descriptor_sets;
        std::shared_ptr<vk::DescriptorPool> pool;
    };

}  // namespace ao::vulkan