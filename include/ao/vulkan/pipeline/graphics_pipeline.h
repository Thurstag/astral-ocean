// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include "pipeline.h"

namespace ao::vulkan {
    class GraphicsPipeline : public Pipeline {
       public:
        /**
         * @brief Construct a new Graphics Pipeline object
         *
         * @param device Device
         * @param layout Layout
         * @param render_pass Render pass
         * @param shader_stages Shader stages
         * @param vertex_input_create_info Vertex input info
         * @param input_assembly_create_info Input assembly info
         * @param tesselation_create_info Tesselation info
         * @param viewport_create_info Viewport info
         * @param rasterization_create_info Rasterization info
         * @param multisample_create_info Multisample info
         * @param depth_stencil_create_info Depth stencil info
         * @param color_blend_create_info Color blend info
         * @param dynamic_state_create_info Dynamic state info
         * @param cache_create_info Pipeline cache create info
         * @param subpass Subpass
         * @param base_pipeline Base pipeline
         */
        GraphicsPipeline(std::shared_ptr<vk::Device> device, std::shared_ptr<PipelineLayout> layout, vk::RenderPass render_pass,
                         vk::ArrayProxy<vk::PipelineShaderStageCreateInfo> shader_stages,
                         std::optional<vk::PipelineVertexInputStateCreateInfo> vertex_input_create_info = std::nullopt,
                         std::optional<vk::PipelineInputAssemblyStateCreateInfo> input_assembly_create_info = std::nullopt,
                         std::optional<vk::PipelineTessellationStateCreateInfo> tesselation_create_info = std::nullopt,
                         std::optional<vk::PipelineViewportStateCreateInfo> viewport_create_info = std::nullopt,
                         std::optional<vk::PipelineRasterizationStateCreateInfo> rasterization_create_info = std::nullopt,
                         std::optional<vk::PipelineMultisampleStateCreateInfo> multisample_create_info = std::nullopt,
                         std::optional<vk::PipelineDepthStencilStateCreateInfo> depth_stencil_create_info = std::nullopt,
                         std::optional<vk::PipelineColorBlendStateCreateInfo> color_blend_create_info = std::nullopt,
                         std::optional<vk::PipelineDynamicStateCreateInfo> dynamic_state_create_info = std::nullopt,
                         vk::PipelineCacheCreateInfo cache_create_info = vk::PipelineCacheCreateInfo(), u32 subpass = 0,
                         vk::Pipeline base_pipeline = vk::Pipeline());

        /**
         * @brief Destroy the Graphics Pipeline object
         *
         */
        virtual ~GraphicsPipeline() = default;
    };
}  // namespace ao::vulkan
