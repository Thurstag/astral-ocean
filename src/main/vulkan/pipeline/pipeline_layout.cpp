// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#include "pipeline_layout.h"

ao::vulkan::PipelineLayout::PipelineLayout(std::shared_ptr<vk::Device> device, std::vector<vk::DescriptorSetLayout> descriptor_layouts,
                                           std::vector<vk::PushConstantRange> push_constants)
    : device(device), descriptor_layouts(descriptor_layouts), push_constants(push_constants) {
    this->layout = this->device->createPipelineLayout(
        vk::PipelineLayoutCreateInfo(vk::PipelineLayoutCreateFlags(), static_cast<u32>(descriptor_layouts.size()), descriptor_layouts.data(),
                                     static_cast<u32>(push_constants.size()), push_constants.data()));
}

ao::vulkan::PipelineLayout::~PipelineLayout() {
    // Destroy descriptor layouts
    for (auto& layout : this->descriptor_layouts) {
        this->device->destroyDescriptorSetLayout(layout);
    }

    if (this->layout) {
        this->device->destroyPipelineLayout(this->layout);
    }
}