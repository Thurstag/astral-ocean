// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#include "fence.h"

#include <ao/core/exception/exception.h>

ao::vulkan::Fence::Fence(std::shared_ptr<vk::Device> device)
    : device(device), status_(std::make_shared<ao::vulkan::FenceStatus>(ao::vulkan::FenceStatus::eUnknown)) {
    // Create fence
    this->fence = std::make_shared<vk::Fence>(this->device->createFence(vk::FenceCreateInfo()));

    // Reset it
    this->device->resetFences(*this->fence);
}

ao::vulkan::Fence::~Fence() {
    if (this->fence.use_count() == 1 && this->status() != ao::vulkan::FenceStatus::eDestroyed) {
        this->destroy();
    }
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
    if (*this->status_ == ao::vulkan::FenceStatus::eUnknown) {
        return ao::vulkan::Fence::ToStatus(this->device->getFenceStatus(*this->fence));
    }
    return *this->status_;
}

void ao::vulkan::Fence::destroy() {
    (this->assert)();

    this->device->destroyFence(*this->fence);
    *this->status_ = ao::vulkan::FenceStatus::eDestroyed;
}

void ao::vulkan::Fence::reset() {
    (this->assert)();

    this->device->resetFences(*this->fence);
}

void ao::vulkan::Fence::wait(u64 timeout) const {
    (this->assert)();

    this->device->waitForFences(*this->fence, VK_TRUE, timeout);
}

void(ao::vulkan::Fence::assert)() const {
    if (*this->status_ == ao::vulkan::FenceStatus::eDestroyed) {
        throw ao::core::Exception("Fence is already destroyed");
    }
}