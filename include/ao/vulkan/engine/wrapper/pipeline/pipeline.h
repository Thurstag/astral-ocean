// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include "descriptor_pool.h"
#include "pipeline_layout.h"

namespace ao::vulkan {
    /**
     * @brief vk::Pipeline wrapper
     *
     */
    class Pipeline {
       public:
        /**
         * @brief Construct a new Pipeline object
         *
         */
        Pipeline(){};

        /**
         * @brief Construct a new Pipeline object
         *
         * @param device Device
         * @param layout Layout
         * @param pipeline Pipeline
         */
        Pipeline(std::weak_ptr<Device> device, std::shared_ptr<PipelineLayout> layout, vk::Pipeline pipeline);

        /**
         * @brief Destroy the Pipeline object
         *
         */
        virtual ~Pipeline();

        /**
         * @brief Get descriptor pools
         *
         * @return std::vector<DescriptorPool>& Pools
         */
        std::vector<DescriptorPool>& pools();

        /**
         * @brief Get pipeline layout
         *
         * @return std::shared_ptr<PipelineLayout> Pipeline layout
         */
        std::shared_ptr<PipelineLayout> layout();

        /**
         * @brief Get pipeline
         *
         * @return vk::Pipeline Pipeline
         */
        vk::Pipeline value();

       protected:
        std::weak_ptr<Device> device;

        std::shared_ptr<PipelineLayout> layout_;
        std::vector<DescriptorPool> pools_;
        vk::Pipeline pipeline;
    };

}  // namespace ao::vulkan