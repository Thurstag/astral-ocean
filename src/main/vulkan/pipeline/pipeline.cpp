// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#include "pipeline.h"

ao::vulkan::Pipeline::Pipeline(std::shared_ptr<vk::Device> device, std::shared_ptr<PipelineLayout> layout, vk::Pipeline pipeline,
                               vk::PipelineCacheCreateInfo cache_create_info)
    : device(device), layout_(layout) {
    this->cache = this->device->createPipelineCache(cache_create_info);

    this->pipeline =
        std::unique_ptr<vk::Pipeline, std::function<void(vk::Pipeline*)>>(new vk::Pipeline(pipeline), [device = *device](vk::Pipeline* pipeline) {
            device.destroyPipeline(*pipeline);
            delete pipeline;
        });
}

ao::vulkan::Pipeline::~Pipeline() {
    this->layout_.reset();

    if (this->cache) {
        if (this->before_cache_destruction) {
            (*this->before_cache_destruction)(this->cache);
        }

        this->device->destroyPipelineCache(this->cache);
    }
}