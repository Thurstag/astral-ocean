// Copyright 2018 Astral-Ocean Project
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
         * @param pushconstants PushConstants
         */
        PipelineLayout(std::weak_ptr<Device> device, std::vector<vk::DescriptorSetLayout> descriptor_layouts = {},
                       std::vector<vk::PushConstantRange> pushconstants = {});

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

       protected:
        std::weak_ptr<Device> device;

        std::vector<vk::DescriptorSetLayout> descriptor_layouts;
        std::vector<vk::PushConstantRange> pushconstants;
        vk::PipelineLayout layout;
    };

}  // namespace ao::vulkan