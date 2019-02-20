// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#include "device.h"

#include "../utilities/vulkan.h"
#include "fence.h"

ao::vulkan::Device::Device(vk::PhysicalDevice device) : physical(device) {}

ao::vulkan::Device::~Device() {
    this->transfer_command_pool.reset();
    this->graphics_command_pool.reset();
}

void ao::vulkan::Device::initLogicalDevice(vk::ArrayProxy<char const* const> device_extensions,
                                           vk::ArrayProxy<vk::PhysicalDeviceFeatures const> device_features,
                                           vk::ArrayProxy<QueueRequest const> requested_queues) {
    // Get queue families
    std::vector<vk::QueueFamilyProperties> queue_families = this->physical.getQueueFamilyProperties();

    // Check count
    if (queue_families.empty()) {
        throw ao::core::Exception("Device hasn't queues");
    }

    // Print a report of all queue families
    this->LOGGER << ao::core::Logger::Level::trace << fmt::format("Queue families:\n{}", ao::vulkan::utilities::report(queue_families));

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
    this->logical = this->physical.createDevice(device_info);
    volkLoadDevice(this->logical);

    // Init queue container
    this->queues = std::make_unique<ao::vulkan::QueueContainer>(this->logical, _queue_create_info, queue_families);

    // Create command pools
    if (this->queues->exists(vk::to_string(vk::QueueFlagBits::eTransfer))) {
        this->transfer_command_pool = std::make_unique<ao::vulkan::CommandPool>(
            this->logical, vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
            this->queues->at(vk::to_string(vk::QueueFlagBits::eTransfer)).family_index, ao::vulkan::CommandPoolAccessModeFlagBits::eConcurrent);
    }
    if (this->queues->exists(vk::to_string(vk::QueueFlagBits::eGraphics))) {
        this->graphics_command_pool = std::make_unique<ao::vulkan::CommandPool>(
            this->logical, vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
            this->queues->at(vk::to_string(vk::QueueFlagBits::eGraphics)).family_index, ao::vulkan::CommandPoolAccessModeFlagBits::eConcurrent);
    }

    // Find suitable depth format
    this->depth_format = ao::vulkan::utilities::getSupportedDepthFormat(this->physical);
}

std::pair<vk::Image, vk::DeviceMemory> ao::vulkan::Device::createImage(u32 width, u32 height, u32 mip_levels, u32 array_layers, vk::Format format,
                                                                       vk::ImageType type, vk::ImageTiling tilling, vk::ImageUsageFlags usage_flags,
                                                                       vk::MemoryPropertyFlags memory_flags) {
    std::pair<vk::Image, vk::DeviceMemory> pair;

    // Create image
    pair.first = this->logical.createImage(vk::ImageCreateInfo(vk::ImageCreateFlags(), type, format, vk::Extent3D(vk::Extent2D(width, height), 1),
                                                               mip_levels, array_layers, vk::SampleCountFlagBits::e1, tilling, usage_flags,
                                                               vk::SharingMode::eExclusive, 0, nullptr, vk::ImageLayout::eUndefined));

    // Get memory requirements
    vk::MemoryRequirements mem_requirements = this->logical.getImageMemoryRequirements(pair.first);

    // Allocate	memory
    pair.second =
        this->logical.allocateMemory(vk::MemoryAllocateInfo(mem_requirements.size, this->memoryType(mem_requirements.memoryTypeBits, memory_flags)));

    // Bind image and memory
    this->logical.bindImageMemory(pair.first, pair.second, 0);
    return pair;
}

vk::ImageView ao::vulkan::Device::createImageView(vk::Image image, vk::Format format, vk::ImageViewType view_type,
                                                  vk::ImageSubresourceRange subresource_range) {
    return this->logical.createImageView(
        vk::ImageViewCreateInfo(vk::ImageViewCreateFlags(), image, view_type, format, vk::ComponentMapping(), subresource_range));
}

void ao::vulkan::Device::processImage(vk::Image image, vk::Format format, vk::ImageSubresourceRange subresource_range, vk::ImageLayout old_layout,
                                      vk::ImageLayout new_layout) {
    if (!this->graphics_command_pool) {
        throw ao::core::Exception("Graphics command pool is disabled, request a graphics queue to enable it");
    }

    // Create command buffer
    vk::CommandBuffer cmd = this->graphics_command_pool->allocateCommandBuffers(vk::CommandBufferLevel::ePrimary, 1).front();

    // Create barrier
    vk::ImageMemoryBarrier barrier(vk::AccessFlags(), vk::AccessFlags(), old_layout, new_layout, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED,
                                   image, subresource_range);

    // Add stencil
    if (new_layout == vk::ImageLayout::eDepthStencilAttachmentOptimal &&
        (format == vk::Format::eD32SfloatS8Uint || format == vk::Format::eD24UnormS8Uint)) {
        barrier.subresourceRange.aspectMask |= vk::ImageAspectFlagBits::eStencil;
    }

    // Define stages
    vk::PipelineStageFlags src_stage;
    vk::PipelineStageFlags dst_stage;
    if (old_layout == vk::ImageLayout::eUndefined && new_layout == vk::ImageLayout::eTransferDstOptimal) {
        barrier.srcAccessMask = vk::AccessFlags();
        barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

        src_stage = vk::PipelineStageFlagBits::eTopOfPipe;
        dst_stage = vk::PipelineStageFlagBits::eTransfer;
    } else if (old_layout == vk::ImageLayout::eTransferDstOptimal && new_layout == vk::ImageLayout::eShaderReadOnlyOptimal) {
        barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
        barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

        src_stage = vk::PipelineStageFlagBits::eTransfer;
        dst_stage = vk::PipelineStageFlagBits::eFragmentShader;
    } else if (old_layout == vk::ImageLayout::eUndefined && new_layout == vk::ImageLayout::eDepthStencilAttachmentOptimal) {
        barrier.srcAccessMask = vk::AccessFlags();
        barrier.dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite;

        src_stage = vk::PipelineStageFlagBits::eTopOfPipe;
        dst_stage = vk::PipelineStageFlagBits::eEarlyFragmentTests;
    } else {
        throw ao::core::Exception(fmt::format("Unsupported layout transition: {} -> {}", vk::to_string(old_layout), vk::to_string(new_layout)));
    }

    // Bind barrier
    cmd.begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
    cmd.pipelineBarrier(src_stage, dst_stage, vk::DependencyFlags(), {}, {}, barrier);
    cmd.end();

    // Create fence
    vulkan::Fence fence(this->logical);

    // Submit command
    this->queues->submit(vk::QueueFlagBits::eGraphics, vk::SubmitInfo().setCommandBufferCount(1).setPCommandBuffers(&cmd), fence);

    // Wait fence
    fence.wait();

    // Free command/fence
    this->graphics_command_pool->freeCommandBuffers(cmd);
    fence.destroy();
}

void ao::vulkan::Device::copyBufferToImage(vk::Buffer buffer, vk::Image image, vk::ArrayProxy<vk::BufferImageCopy const> regions) {
    if (!this->transfer_command_pool) {
        throw ao::core::Exception("Transfer command pool is disabled, request a graphics queue to enable it");
    }

    // Create command buffer
    vk::CommandBuffer cmd = this->transfer_command_pool->allocateCommandBuffers(vk::CommandBufferLevel::ePrimary, 1).front();

    cmd.begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
    cmd.copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal, regions);
    cmd.end();

    // Create fence
    vulkan::Fence fence(this->logical);

    // Submit command
    this->queues->submit(vk::QueueFlagBits::eTransfer, vk::SubmitInfo().setCommandBufferCount(1).setPCommandBuffers(&cmd), fence);

    // Wait fence
    fence.wait();

    // Free command/fence
    this->transfer_command_pool->freeCommandBuffers(cmd);
    fence.destroy();
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
