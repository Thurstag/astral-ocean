// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#include "semaphore_container.h"

ao::vulkan::SemaphoreContainer::~SemaphoreContainer() {
    if (!this->content.empty()) {
        ao::vulkan::SemaphoreContainer::clear();
    }
}

ao::vulkan::structs::Semaphores& ao::vulkan::SemaphoreContainer::operator[](size_t index) {
    return this->content[index];
}

void ao::vulkan::SemaphoreContainer::clear() {
    std::vector<vk::Semaphore> destroyed;

    // Destroy semaphores
    for (auto& value : this->content) {
        for (auto semaphore : value.waits) {
            if (std::find(destroyed.begin(), destroyed.end(), semaphore) == destroyed.end()) {
                this->device->destroySemaphore(semaphore);
                destroyed.push_back(semaphore);
            }
        }
        for (auto semaphore : value.signals) {
            if (std::find(destroyed.begin(), destroyed.end(), semaphore) == destroyed.end()) {
                this->device->destroySemaphore(semaphore);
                destroyed.push_back(semaphore);
            }
        }
    }

    // Clear map
    this->content.clear();
}

void ao::vulkan::SemaphoreContainer::resize(size_t size) {
    this->content.resize(size);
}
