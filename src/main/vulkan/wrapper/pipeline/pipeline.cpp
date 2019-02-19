// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#include "pipeline.h"

#include <ao/core/utilities/pointers.h>

ao::vulkan::Pipeline::Pipeline(std::weak_ptr<Device> device, std::shared_ptr<PipelineLayout> layout, vk::Pipeline pipeline,
                               vk::PipelineCacheCreateInfo cache_create_info)
    : device(device), layout_(layout), pipeline(pipeline) {
    this->cache = ao::core::shared(device)->logical.createPipelineCache(cache_create_info);
}

ao::vulkan::Pipeline::~Pipeline() {
    auto _device = ao::core::shared(this->device);

    this->layout_.reset();

    if (this->cache) {
        if (this->before_cache_destruction) {
            (*this->before_cache_destruction)(this->cache);
        }

        _device->logical.destroyPipelineCache(this->cache);
    }
    if (this->pipeline) {
        _device->logical.destroyPipeline(this->pipeline);
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

void ao::vulkan::Pipeline::setBeforePipelineCacheDestruction(std::function<void(vk::PipelineCache)> callback) {
    this->before_cache_destruction = callback;
}