// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#include "fence.h"

#include <ao/core/exception/exception.h>

ao::vulkan::Fence::Fence(std::shared_ptr<vk::Device> device) : device(device) {
    // Create fence
    this->fence = std::shared_ptr<std::pair<vk::Fence, FenceStatus>>(
        new std::pair<vk::Fence, FenceStatus>(this->device->createFence(vk::FenceCreateInfo()), ao::vulkan::FenceStatus::eUnknown),
        [device = *device](std::pair<vk::Fence, FenceStatus>* fence) {
            if (fence->second != ao::vulkan::FenceStatus::eDestroyed) {
                device.destroyFence(fence->first);
            }
            delete fence;
        });

    // Reset it
    this->device->resetFences(this->fence->first);
}

ao::vulkan::FenceStatus ao::vulkan::Fence::ToStatus(vk::Result result) {
    switch (result) {
        case vk::Result::eErrorDeviceLost:
            return ao::vulkan::FenceStatus::eDeviceLost;

        case vk::Result::eSuccess:
            return ao::vulkan::FenceStatus::eSignaled;

        case vk::Result::eNotReady:
            return ao::vulkan::FenceStatus::eUnsignaled;

        default:
            return ao::vulkan::FenceStatus::eUnknown;
    }
}

ao::vulkan::FenceStatus ao::vulkan::Fence::status() const {
    if (this->fence->second == ao::vulkan::FenceStatus::eUnknown) {
        return ao::vulkan::Fence::ToStatus(this->device->getFenceStatus(this->fence->first));
    }
    return this->fence->second;
}

void ao::vulkan::Fence::destroy() {
    (this->assert)();

    this->device->destroyFence((*this->fence).first);
    this->fence->second = ao::vulkan::FenceStatus::eDestroyed;
}

void ao::vulkan::Fence::reset() {
    (this->assert)();

    this->device->resetFences(this->fence->first);
}

void ao::vulkan::Fence::wait(u64 timeout) const {
    (this->assert)();

    this->device->waitForFences(this->fence->first, VK_TRUE, timeout);
}

void(ao::vulkan::Fence::assert)() const {
    if (this->fence->second == ao::vulkan::FenceStatus::eDestroyed) {
        throw ao::core::Exception("Fence is already destroyed");
    }
}