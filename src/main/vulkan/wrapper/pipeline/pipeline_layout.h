// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <vulkan/vulkan.hpp>

#include "../device.h"

namespace ao::vulkan {
    /**
     * @brief vk::PipelineLayout wrapper
     *
     */
    class PipelineLayout {
       public:
        /**
         * @brief Construct a new PipelineLayout object
         *
         * @param device Device
         * @param descriptor_layouts Descriptor layouts
         * @param push_constants Push Constants
         */
        PipelineLayout(std::weak_ptr<Device> device, std::vector<vk::DescriptorSetLayout> descriptor_layouts = {},
                       std::vector<vk::PushConstantRange> push_constants = {});

        /**
         * @brief Destroy the PipelineLayout object
         *
         */
        virtual ~PipelineLayout();

        /**
         * @brief Get pipeline layout
         *
         * @return vk::PipelineLayout Layout
         */
        vk::PipelineLayout value();

        /**
         * @brief Get descriptor layouts
         *
         * @return std::vector<vk::DescriptorSetLayout> const& Descriptor layouts
         */
        std::vector<vk::DescriptorSetLayout> const& descriptorLayouts();

        /**
         * @brief Get push constants
         *
         * @return std::vector<vk::PushConstantRange> const& Push constants
         */
        std::vector<vk::PushConstantRange> const& pushConstants();

       protected:
        std::weak_ptr<Device> device;

        std::vector<vk::DescriptorSetLayout> descriptor_layouts;
        std::vector<vk::PushConstantRange> push_constants;
        vk::PipelineLayout layout;
    };

}  // namespace ao::vulkan