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
    std::vector<vk::Semaphore> cleared;

    // Destroy semaphores
    if (auto _device = ao::core::shared(this->device)) {
        for (auto& pair : map) {
            for (auto& semaphore : pair.second.waits) {
                if (std::find(cleared.begin(), cleared.end(), semaphore) == cleared.end()) {
                    _device->logical.destroySemaphore(semaphore);
                    cleared.push_back(semaphore);
                }
            }
            for (auto& semaphore : pair.second.signals) {
                if (std::find(cleared.begin(), cleared.end(), semaphore) == cleared.end()) {
                    _device->logical.destroySemaphore(semaphore);
                    cleared.push_back(semaphore);
                }
            }
        }
    }

    // Clear map
    this->map.clear();
    cleared.clear();
}
