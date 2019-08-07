// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#include "device.h"

#include "../utilities/vulkan.h"
#include "fence.h"

ao::vulkan::Device::Device(vk::PhysicalDevice device) : physical_(device) {}

ao::vulkan::Device::~Device() {
    this->transfer_command_pool.reset();
    this->graphics_command_pool.reset();
}

void ao::vulkan::Device::initLogicalDevice(vk::ArrayProxy<char const* const> device_extensions,
                                           vk::ArrayProxy<vk::PhysicalDeviceFeatures const> device_features,
                                           vk::ArrayProxy<QueueRequest const> requested_queues) {
    // Get queue families
    std::vector<vk::QueueFamilyProperties> queue_families = this->physical_.getQueueFamilyProperties();

    // Check count
    if (queue_families.empty()) {
        throw ao::core::Exception("Device hasn't queues");
    }

    // Print a report of all queue families
    LOG_MSG(trace) << fmt::format("Queue families:\n{}", ao::vulkan::utilities::report(queue_families));

    // Build queue priority list
    std::vector<float> queue_priorities(
        std::max_element(queue_families.begin(), queue_families.end(),
                         [](vk::QueueFamilyProperties a, vk::QueueFamilyProperties b) { return a.queueCount < b.queueCount; })
            ->queueCount,
        0.0f);

    // Find best family for each request queues
    std::vector<vk::DeviceQueueCreateInfo> queue_create_info;
    std::vector<ao::vulkan::QueueCreateInfo> _queue_create_info;
    for (auto& request : requested_queues) {
        u32 family_index = ao::vulkan::utilities::findQueueFamilyIndex(queue_families, request.flag);

        // Check if there is already a create info for this family
        auto it = std::find_if(queue_create_info.begin(), queue_create_info.end(),
                               [family_index](vk::DeviceQueueCreateInfo& create_info) { return create_info.queueFamilyIndex == family_index; });
        if (it != queue_create_info.end()) {
            if (it->queueCount + request.count() <= queue_families[family_index].queueCount) {
                _queue_create_info.push_back(ao::vulkan::QueueCreateInfo(request, family_index, it->queueCount));

                it->queueCount += request.count();
            } else if (it->queueCount + request.count() > queue_families[family_index].queueCount &&
                       request.count() <= queue_families[family_index].queueCount) {
                it->queueCount += (std::max)(it->queueCount, request.count());

                _queue_create_info.push_back(ao::vulkan::QueueCreateInfo(request, family_index, 0));
            } else {
                throw ao::core::Exception(fmt::format("Queue request count ({}) exceeds family index ({})'s queue capacity: {}", request.count(),
                                                      family_index, queue_families[family_index].queueCount));
            }

            continue;
        }

        _queue_create_info.push_back(ao::vulkan::QueueCreateInfo(request, family_index, 0));
        queue_create_info.push_back(vk::DeviceQueueCreateInfo(vk::DeviceQueueCreateFlags(), family_index, request.count(), queue_priorities.data()));
    }

    vk::DeviceCreateInfo device_info(vk::DeviceCreateFlags(), static_cast<u32>(queue_create_info.size()), queue_create_info.data());
    device_info.setPEnabledFeatures(device_features.data());

    // Add extensions
    if (!device_extensions.empty()) {
        device_info.setEnabledExtensionCount(static_cast<u32>(device_extensions.size()));
        device_info.setPpEnabledExtensionNames(device_extensions.data());
    }

    // Create device
    this->logical_ = std::make_shared<vk::Device>(this->physical_.createDevice(device_info));
    volkLoadDevice(*this->logical_);

    // Init queue container
    this->queues_ = std::make_unique<ao::vulkan::QueueContainer>(this->logical_, _queue_create_info, queue_families);

    // Create command pools
    if (this->queues_->exists(vk::to_string(vk::QueueFlagBits::eTransfer))) {
        this->transfer_command_pool = std::make_unique<ao::vulkan::CommandPool>(
            this->logical_, vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
            this->queues_->at(vk::to_string(vk::QueueFlagBits::eTransfer)).family_index, ao::vulkan::CommandPoolAccessMode::eConcurrent);
    }
    if (this->queues_->exists(vk::to_string(vk::QueueFlagBits::eGraphics))) {
        this->graphics_command_pool = std::make_unique<ao::vulkan::CommandPool>(
            this->logical_, vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
            this->queues_->at(vk::to_string(vk::QueueFlagBits::eGraphics)).family_index, ao::vulkan::CommandPoolAccessMode::eConcurrent);
    }
}

ao::vulkan::CommandPool& ao::vulkan::Device::transferPool() {
    if (!this->transfer_command_pool) {
        throw ao::core::Exception("Transfer command pool is disabled, request a transfer queue to enable it");
    }

    return *this->transfer_command_pool;
}

ao::vulkan::CommandPool& ao::vulkan::Device::graphicsPool() {
    if (!this->graphics_command_pool) {
        throw ao::core::Exception("Graphics command pool is disabled, request a graphics queue to enable it");
    }

    return *this->graphics_command_pool;
}
