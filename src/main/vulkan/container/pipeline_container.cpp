// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#include "pipeline_container.h"

ao::vulkan::PipelineContainer::~PipelineContainer() {
    for (auto [key, value] : this->map) {
        delete value;
    }
}

void ao::vulkan::PipelineContainer::clear() {
    for (auto [key, value] : this->map) {
        delete value;
    }

    this->map.clear();
}

void ao::vulkan::PipelineContainer::setBeforePipelineCacheDestruction(std::function<void(std::string, vk::PipelineCache)> callback) {
    for (auto [key, value] : this->map) {
        value->setBeforePipelineCacheDestruction([key = std::string(key), callback](vk::PipelineCache cache) { callback(key, cache); });
    }
}