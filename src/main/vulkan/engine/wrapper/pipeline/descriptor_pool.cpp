// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#include "descriptor_pool.h"

#include <ao/core/utilities/pointers.h>

ao::vulkan::DescriptorPool::DescriptorPool(std::weak_ptr<Device> device, vk::DescriptorPool pool) : device(device), pool(pool) {}

ao::vulkan::DescriptorPool::~DescriptorPool() {
    auto _device = ao::core::shared(this->device);

    // Free descriptor sets
    _device->logical.freeDescriptorSets(this->pool, this->descriptor_sets);

    // Destroy pool
    _device->logical.destroyDescriptorPool(this->pool);
}

std::vector<vk::DescriptorSet> ao::vulkan::DescriptorPool::allocateDescriptorSets(u32 count,
                                                                                  std::vector<vk::DescriptorSetLayout> descriptor_layouts) {
    auto sets =
        ao::core::shared(this->device)->logical.allocateDescriptorSets(vk::DescriptorSetAllocateInfo(this->pool, count, descriptor_layouts.data()));

    // Add descriptor sets
    for (auto& set : sets) {
        this->descriptor_sets.push_back(set);
    }

    return sets;
}

std::vector<vk::DescriptorSet> const& ao::vulkan::DescriptorPool::descriptorSets() {
    return this->descriptor_sets;
}