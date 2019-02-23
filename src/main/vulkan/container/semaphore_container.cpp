// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#include "semaphore_container.h"

ao::vulkan::SemaphoreContainer::~SemaphoreContainer() {
    if (!this->map.empty()) {
        ao::vulkan::SemaphoreContainer::clear();
    }
}

void ao::vulkan::SemaphoreContainer::clear() {
    std::vector<vk::Semaphore> destroyed;

    // Destroy semaphores
    for (auto& [key, value] : map) {
        for (auto& semaphore : value.waits) {
            if (std::find(destroyed.begin(), destroyed.end(), semaphore) == destroyed.end()) {
                this->device->destroySemaphore(semaphore);
                destroyed.push_back(semaphore);
            }
        }
        for (auto& semaphore : value.signals) {
            if (std::find(destroyed.begin(), destroyed.end(), semaphore) == destroyed.end()) {
                this->device->destroySemaphore(semaphore);
                destroyed.push_back(semaphore);
            }
        }
    }

    // Clear map
    this->map.clear();
}
