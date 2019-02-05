// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <vector>

#include <ao/core/utilities/pointers.h>
#include <vulkan/vulkan.hpp>

#include "device.h"
#include "swapchain.h"

namespace ao::vulkan {
    /**
     * @brief Wrapper for vulkan pipelines (TODO: Refactor me)
     *
     */
    struct Pipeline {
       public:
        vk::PipelineStageFlags submit_pipeline_stages;
        std::vector<vk::PipelineLayout> layouts;
        std::vector<vk::Pipeline> pipelines;

        vk::PipelineCache cache;

        /**
         * @brief Construct a new Pipeline object
         *
         */
        Pipeline() = default;

        /**
         * @brief Construct a new Pipeline object
         *
         * @param device Device
         */
        explicit Pipeline(std::weak_ptr<Device> device);

        /**
         * @brief Destroy the Pipeline object
         *
         */
        virtual ~Pipeline();

       protected:
        std::weak_ptr<Device> device;
    };
}  // namespace ao::vulkan
