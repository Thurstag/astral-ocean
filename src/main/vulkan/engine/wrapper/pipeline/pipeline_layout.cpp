// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#include "pipeline_layout.h"

#include <ao/core/utilities/pointers.h>

ao::vulkan::PipelineLayout::PipelineLayout(std::weak_ptr<Device> device, std::vector<vk::DescriptorSetLayout> descriptor_layouts,
                                           std::vector<vk::PushConstantRange> push_constants)
    : descriptor_layouts(descriptor_layouts), push_constants(push_constants) {
    this->layout = ao::core::shared(device)->logical.createPipelineLayout(
        vk::PipelineLayoutCreateInfo(vk::PipelineLayoutCreateFlags(), static_cast<u32>(descriptor_layouts.size()), descriptor_layouts.data(),
                                     static_cast<u32>(push_constants.size()), push_constants.data()));
}

ao::vulkan::PipelineLayout::~PipelineLayout() {
    auto _device = ao::core::shared(this->device);

    // Destroy descriptor layouts
    for (auto& layout : this->descriptor_layouts) {
        _device->logical.destroyDescriptorSetLayout(layout);
    }

    if (this->layout) {
        _device->logical.destroyPipelineLayout(this->layout);
    }
}

vk::PipelineLayout ao::vulkan::PipelineLayout::value() {
    return this->layout;
}

std::vector<vk::DescriptorSetLayout> const& ao::vulkan::PipelineLayout::descriptorLayouts() {
    return this->descriptor_layouts;
}

std::vector<vk::PushConstantRange> const& ao::vulkan::PipelineLayout::pushConstants() {
    return this->push_constants;
}