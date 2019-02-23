// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#include "descriptor_pool.h"

ao::vulkan::DescriptorPool::DescriptorPool(std::shared_ptr<vk::Device> device, vk::DescriptorPoolCreateInfo create_info) : device(device) {
    this->pool = std::make_shared<vk::DescriptorPool>(this->device->createDescriptorPool(create_info));
}

ao::vulkan::DescriptorPool::~DescriptorPool() {
    // Free descriptor sets
    if (!this->descriptor_sets.empty()) {
        this->device->freeDescriptorSets(*this->pool, this->descriptor_sets);
    }

    if (this->pool.use_count() == 1) {
        this->device->destroyDescriptorPool(*this->pool);
    }
}

std::vector<vk::DescriptorSet> ao::vulkan::DescriptorPool::allocateDescriptorSets(u32 count,
                                                                                  vk::ArrayProxy<vk::DescriptorSetLayout> descriptor_layouts) {
    auto descriptors = this->device->allocateDescriptorSets(vk::DescriptorSetAllocateInfo(*this->pool, count, descriptor_layouts.data()));

    // Add descriptor sets
    for (auto& descriptor : descriptors) {
        this->descriptor_sets.push_back(descriptor);
    }

    return descriptors;
}