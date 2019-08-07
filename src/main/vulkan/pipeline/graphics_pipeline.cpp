// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#include "graphics_pipeline.h"

ao::vulkan::GraphicsPipeline::GraphicsPipeline(std::shared_ptr<vk::Device> device, std::shared_ptr<PipelineLayout> layout, vk::RenderPass render_pass,
                                               vk::ArrayProxy<vk::PipelineShaderStageCreateInfo> shader_stages,
                                               std::optional<vk::PipelineVertexInputStateCreateInfo> vertex_input_create_info,
                                               std::optional<vk::PipelineInputAssemblyStateCreateInfo> input_assembly_create_info,
                                               std::optional<vk::PipelineTessellationStateCreateInfo> tesselation_create_info,
                                               std::optional<vk::PipelineViewportStateCreateInfo> viewport_create_info,
                                               std::optional<vk::PipelineRasterizationStateCreateInfo> rasterization_create_info,
                                               std::optional<vk::PipelineMultisampleStateCreateInfo> multisample_create_info,
                                               std::optional<vk::PipelineDepthStencilStateCreateInfo> depth_stencil_create_info,
                                               std::optional<vk::PipelineColorBlendStateCreateInfo> color_blend_create_info,
                                               std::optional<vk::PipelineDynamicStateCreateInfo> dynamic_state_create_info,
                                               vk::PipelineCacheCreateInfo cache_create_info, u32 subpass, vk::Pipeline base_pipeline)
    : ao::vulkan::Pipeline(device, layout, vk::Pipeline(), cache_create_info) {
    vk::GraphicsPipelineCreateInfo create_info(
        vk::PipelineCreateFlags(), static_cast<u32>(shader_stages.size()), shader_stages.data(),
        vertex_input_create_info ? &(*vertex_input_create_info) : nullptr, input_assembly_create_info ? &(*input_assembly_create_info) : nullptr,
        tesselation_create_info ? &(*tesselation_create_info) : nullptr, viewport_create_info ? &(*viewport_create_info) : nullptr,
        rasterization_create_info ? &(*rasterization_create_info) : nullptr, multisample_create_info ? &(*multisample_create_info) : nullptr,
        depth_stencil_create_info ? &(*depth_stencil_create_info) : nullptr, color_blend_create_info ? &(*color_blend_create_info) : nullptr,
        dynamic_state_create_info ? &(*dynamic_state_create_info) : nullptr, layout->value(), render_pass, subpass, base_pipeline);

    // Create pipeline
    *this->pipeline = this->device->createGraphicsPipelines(this->cache, create_info).front();
}