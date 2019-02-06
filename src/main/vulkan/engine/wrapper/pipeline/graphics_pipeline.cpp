// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#include "graphics_pipeline.h"

#include <ao/core/utilities/pointers.h>

ao::vulkan::GraphicsPipeline::GraphicsPipeline(
    std::weak_ptr<Device> device, std::shared_ptr<ao::vulkan::PipelineLayout> layout, vk::RenderPass render_pass,
    std::vector<vk::PipelineShaderStageCreateInfo> shader_stages, vk::PipelineVertexInputStateCreateInfo vertex_input_create_info,
    vk::PipelineInputAssemblyStateCreateInfo input_assembly_create_info, vk::PipelineTessellationStateCreateInfo tesselation_create_info,
    vk::PipelineViewportStateCreateInfo viewport_create_info, vk::PipelineRasterizationStateCreateInfo rasterization_create_info,
    vk::PipelineMultisampleStateCreateInfo multisample_create_info, vk::PipelineDepthStencilStateCreateInfo depth_stencil_create_info,
    vk::PipelineColorBlendStateCreateInfo color_blend_create_info, vk::PipelineDynamicStateCreateInfo dynamic_state_create_info, u32 subpass,
    vk::Pipeline base_pipeline, u32 base_pipeline_index)
    : ao::vulkan::Pipeline(device, layout, vk::Pipeline()) {
    auto _device = ao::core::shared(this->device);

    // Create cache
    this->cache = _device->logical.createPipelineCache(vk::PipelineCacheCreateInfo());

    vk::GraphicsPipelineCreateInfo create_info(vk::PipelineCreateFlags(), static_cast<u32>(shader_stages.size()), shader_stages.data(),
                                               &vertex_input_create_info, &input_assembly_create_info, &tesselation_create_info,
                                               &viewport_create_info, &rasterization_create_info, &multisample_create_info,
                                               &depth_stencil_create_info, &color_blend_create_info, &dynamic_state_create_info, layout->value(),
                                               render_pass, subpass, base_pipeline, base_pipeline_index);

    // Create pipeline
    this->pipeline = _device->logical.createGraphicsPipelines(this->cache, create_info).front();
}

ao::vulkan::GraphicsPipeline::~GraphicsPipeline() {
    ao::core::shared(this->device)->logical.destroyPipelineCache(this->cache);

    // TODO: Create callback to save cache
}