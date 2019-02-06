// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#include "pipeline.h"

#include <ao/core/utilities/pointers.h>

ao::vulkan::Pipeline::Pipeline(std::weak_ptr<Device> device, std::shared_ptr<PipelineLayout> layout, vk::Pipeline pipeline)
    : device(device), layout_(layout), pipeline(pipeline) {}

ao::vulkan::Pipeline::~Pipeline() {
    this->layout_.reset();

    if (this->pipeline) {
        ao::core::shared(this->device)->logical.destroyPipeline(this->pipeline);
    }

    this->pools_.clear();
}

std::vector<ao::vulkan::DescriptorPool>& ao::vulkan::Pipeline::pools() {
    return this->pools_;
}

std::shared_ptr<ao::vulkan::PipelineLayout> ao::vulkan::Pipeline::layout() {
    return this->layout_;
}

vk::Pipeline ao::vulkan::Pipeline::value() {
    return this->pipeline;
}