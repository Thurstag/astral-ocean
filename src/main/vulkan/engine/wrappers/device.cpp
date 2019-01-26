// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#include "device.h"

ao::vulkan::Device::Device(vk::PhysicalDevice const& device) : physical(device) {
    this->extensions = ao::vulkan::utilities::vkExtensionProperties(this->physical);

    // Check count
    if (this->extensions.empty()) {
        LOGGER << ao::core::Logger::Level::warning << "Extensions vector is empty";
    }
}

ao::vulkan::Device::~Device() {
    this->logical.destroyCommandPool(this->transfer_command_pool);
    this->logical.destroy();
}

void ao::vulkan::Device::initLogicalDevice(std::vector<char const*> device_extensions, std::vector<vk::PhysicalDeviceFeatures> const& device_features,
                                           vk::QueueFlags const qflags, vk::CommandPoolCreateFlags const cflags,
                                           vk::QueueFlagBits const default_queue, bool swapchain_support) {
    std::vector<vk::QueueFamilyProperties> queue_properties = this->physical.getQueueFamilyProperties();
    std::vector<vk::DeviceQueueCreateInfo> queue_infos;
    float const DEFAULT_QUEUE_PRIORITY = 0.0f;

    // clang-format off
    std::array<vk::QueueFlagBits, 5> const all_queue_flags = {
		vk::QueueFlagBits::eGraphics, vk::QueueFlagBits::eCompute, vk::QueueFlagBits::eTransfer,
        vk::QueueFlagBits::eSparseBinding, vk::QueueFlagBits::eProtected
	};
    // clang-format on

    // Check count
    if (queue_properties.empty()) {
        throw ao::core::Exception("Empty queueFamilyProperties");
    }

    // TODO: Print a report of all queues

    // Find queues
    std::map<u32, vk::QueueFlagBits> indexes;
    std::optional<u32> default_index;
    for (auto& flag : all_queue_flags) {
        if (qflags & flag) {
            u32 index = ao::vulkan::utilities::findQueueFamilyIndex(queue_properties, flag);

            // Check index
            if (index < 0) {
                throw ao::core::Exception(fmt::format("Fail to find a queueFamily that supports: {0}", to_string(flag)));
            }

            // Add index to set
            if (flag == default_queue) {
                default_index = index;
            }
            if (indexes.find(index) == indexes.end()) {
                indexes[index] = flag;
            }
        }
    }

    // Create info for queues
    for (auto& index : indexes) {
        queue_infos.push_back(vk::DeviceQueueCreateInfo(vk::DeviceQueueCreateFlags(), index.first, 1, &DEFAULT_QUEUE_PRIORITY));
    }

    // Request swap chain extension
    if (swapchain_support) {
        device_extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    }

    vk::DeviceCreateInfo device_info(vk::DeviceCreateFlags(), static_cast<u32>(queue_infos.size()), queue_infos.data());
    device_info.setPEnabledFeatures(device_features.data());

    // Add extensions
    if (!device_extensions.empty()) {
        device_info.setEnabledExtensionCount(static_cast<u32>(device_extensions.size()));
        device_info.setPpEnabledExtensionNames(device_extensions.data());
    }

    // Create device
    this->logical = this->physical.createDevice(device_info);

    // Build queue container
    if (!default_index) {
        throw core::Exception("Fail to find a queueFamily that supports graphics");
    }

    for (auto& flag : all_queue_flags) {
        // Find in map
        auto it = std::find_if(indexes.begin(), indexes.end(), [flag](auto const& pair) -> bool { return pair.second == flag; });

        // Check iterator
        if (it != indexes.end()) {
            this->queues[flag] = ao::vulkan::structs::QueueData(this->logical.getQueue(it->first, 0), it->first, queue_properties[it->first]);
        } else {
            this->queues[flag] = this->queues[default_queue];
        }
    }

    // Create command pool for transfert
    this->transfer_command_pool = this->logical.createCommandPool(
        vk::CommandPoolCreateInfo(vk::CommandPoolCreateFlagBits::eResetCommandBuffer, this->queues[vk::QueueFlagBits::eTransfer].index));
}

u32 ao::vulkan::Device::memoryType(u32 type_bits, vk::MemoryPropertyFlags const properties) const {
    vk::PhysicalDeviceMemoryProperties mem_properties = this->physical.getMemoryProperties();

    for (u32 i = 0; i < mem_properties.memoryTypeCount; i++) {
        if ((type_bits & 1) == 1) {
            if ((mem_properties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }
        type_bits >>= 1;
    }
    throw ao::core::Exception("Fail to find a matching memory type");
}
