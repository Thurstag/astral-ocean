// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#include "compute_pipeline.h"

ao::vulkan::ComputePipeline::ComputePipeline(std::shared_ptr<vk::Device> device, std::shared_ptr<PipelineLayout> layout,
                                             vk::PipelineShaderStageCreateInfo shader_stage, vk::PipelineCacheCreateInfo cache_create_info,
                                             vk::Pipeline base_pipeline)
    : ao::vulkan::Pipeline(device, layout, vk::Pipeline(), cache_create_info) {
    this->pipeline = this->device
                         ->createComputePipelines(
                             this->cache, vk::ComputePipelineCreateInfo(vk::PipelineCreateFlags(), shader_stage, layout->value(), base_pipeline))
                         .front();
}