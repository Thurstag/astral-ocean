// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#include "semaphore_container.h"

#include <ao/core/utilities/pointers.h>

ao::vulkan::SemaphoreContainer::~SemaphoreContainer() {
    if (!this->map.empty()) {
        ao::vulkan::SemaphoreContainer::clear();
    }
}

void ao::vulkan::SemaphoreContainer::clear() {
    auto _device = ao::core::shared(this->device);
    std::vector<vk::Semaphore> destroyed;

    // Destroy semaphores
    for (auto& [key, value] : map) {
        for (auto& semaphore : value.waits) {
            if (std::find(destroyed.begin(), destroyed.end(), semaphore) == destroyed.end()) {
                _device->logical.destroySemaphore(semaphore);
                destroyed.push_back(semaphore);
            }
        }
        for (auto& semaphore : value.signals) {
            if (std::find(destroyed.begin(), destroyed.end(), semaphore) == destroyed.end()) {
                _device->logical.destroySemaphore(semaphore);
                destroyed.push_back(semaphore);
            }
        }
    }

    // Clear map
    this->map.clear();
}
