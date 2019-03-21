// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include "pipeline.h"

namespace ao::vulkan {
    /**
     * @brief Compute pipeline wrapper
     *
     */
    class ComputePipeline : public Pipeline {
       public:
        ComputePipeline(std::shared_ptr<vk::Device> device, std::shared_ptr<PipelineLayout> layout, vk::PipelineShaderStageCreateInfo shader_stage,
                        vk::PipelineCacheCreateInfo cache_create_info = vk::PipelineCacheCreateInfo(), vk::Pipeline base_pipeline = vk::Pipeline());

        /**
         * @brief Destroy the ComputePipeline object
         *
         */
        virtual ~ComputePipeline() = default;
    };
}  // namespace ao::vulkan