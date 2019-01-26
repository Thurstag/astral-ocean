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
    /// <summary>
    /// Wrapper for vulkan pipelines (TODO: Refactor me)
    /// </summary>
    struct Pipeline {
       public:
        vk::PipelineStageFlags submit_pipeline_stages;
        std::vector<vk::PipelineLayout> layouts;
        std::vector<vk::Pipeline> pipelines;

        vk::PipelineCache cache;

        /// <summary>
        /// Constructor
        /// </summary>
        Pipeline() = default;

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="device">Device</param>
        explicit Pipeline(std::weak_ptr<Device> device);

        /// <summary>
        /// Destructor
        /// </summary>
        virtual ~Pipeline();

       protected:
        std::weak_ptr<Device> device;
    };
}  // namespace ao::vulkan
