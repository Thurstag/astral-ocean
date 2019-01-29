// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#include "device.h"

#include "../../utilities/vulkan.h"

ao::vulkan::Device::Device(vk::PhysicalDevice device) : physical(device) {
    this->extensions = ao::vulkan::utilities::vkExtensionProperties(device);

    // Check count
    if (this->extensions.empty()) {
        LOGGER << ao::core::Logger::Level::warning << "Extensions vector is empty";
    }
}

ao::vulkan::Device::~Device() {
    this->transfer_command_pool.reset();
    this->graphics_command_pool.reset();
}

void ao::vulkan::Device::initLogicalDevice(std::vector<char const*> device_extensions, std::vector<vk::PhysicalDeviceFeatures> const& device_features,
                                           vk::QueueFlags qflags, vk::QueueFlagBits default_queue, bool swapchain_support) {
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

    // Create command pools
    this->transfer_command_pool = std::make_unique<ao::vulkan::CommandPool>(this->logical, vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
                                                                            this->queues[vk::QueueFlagBits::eTransfer].index,
                                                                            ao::vulkan::CommandPoolAccessModeFlagBits::eConcurrent);
    this->graphics_command_pool = std::make_unique<ao::vulkan::CommandPool>(this->logical, vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
                                                                            this->queues[vk::QueueFlagBits::eGraphics].index,
                                                                            ao::vulkan::CommandPoolAccessModeFlagBits::eConcurrent);

    // Find suitable depth format
    this->depth_format = ao::vulkan::utilities::getSupportedDepthFormat(this->physical);
}

std::vector<vk::SurfaceFormatKHR> ao::vulkan::Device::surfaceFormatKHRs(vk::SurfaceKHR surface) {
    std::string error = "Fail to get supported surface formats";
    std::vector<vk::SurfaceFormatKHR> formats;
    u32 count;

    // Get count
    ao::vulkan::utilities::vkAssert(this->physical.getSurfaceFormatsKHR(surface, &count, nullptr), error);

    // Adapt vector
    formats.resize(count);

    // Get vk::SurfaceFormatKHRs
    ao::vulkan::utilities::vkAssert(this->physical.getSurfaceFormatsKHR(surface, &count, formats.data()), error);
    return formats;
}

std::vector<vk::Image> ao::vulkan::Device::swapChainImages(vk::SwapchainKHR swapChain) {
    std::string error = "Fail to get swap chain images";
    std::vector<vk::Image> images;
    u32 count;

    // Get count
    ao::vulkan::utilities::vkAssert(this->logical.getSwapchainImagesKHR(swapChain, &count, nullptr), error);

    // Adapt vector
    images.resize(count);

    // Get vk::Images
    ao::vulkan::utilities::vkAssert(this->logical.getSwapchainImagesKHR(swapChain, &count, images.data()), error);
    return images;
}

std::pair<vk::Image, vk::DeviceMemory> ao::vulkan::Device::createImage(u32 width, u32 height, vk::Format format, vk::ImageType type,
                                                                       vk::ImageTiling tilling, vk::ImageUsageFlags usage_flags,
                                                                       vk::MemoryPropertyFlags memory_flags) {
    std::pair<vk::Image, vk::DeviceMemory> pair;

    // Create image
    pair.first = this->logical.createImage(vk::ImageCreateInfo(vk::ImageCreateFlags(), type, format, vk::Extent3D(vk::Extent2D(width, height), 1), 1,
                                                               1, vk::SampleCountFlagBits::e1, tilling, usage_flags, vk::SharingMode::eExclusive, 0,
                                                               nullptr, vk::ImageLayout::eUndefined));

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
                                                  vk::ImageAspectFlags aspect_flags) {
    return this->logical.createImageView(vk::ImageViewCreateInfo(vk::ImageViewCreateFlags(), image, view_type, format, vk::ComponentMapping(),
                                                                 vk::ImageSubresourceRange(aspect_flags, 0, 1, 0, 1)));
}

void ao::vulkan::Device::processImage(vk::Image image, vk::Format format, vk::ImageLayout old_layout, vk::ImageLayout new_layout) {
    // Create command buffer
    vk::CommandBuffer cmd = this->graphics_command_pool->allocateCommandBuffers(vk::CommandBufferLevel::ePrimary, 1).front();

    // Create barrier
    vk::ImageMemoryBarrier barrier(
        vk::AccessFlags(), vk::AccessFlags(), old_layout, new_layout, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, image,
        vk::ImageSubresourceRange(
            new_layout == vk::ImageLayout::eDepthStencilAttachmentOptimal ? vk::ImageAspectFlagBits::eDepth : vk::ImageAspectFlagBits::eColor, 0, 1,
            0, 1));

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
    vk::Fence fence = this->logical.createFence(vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled));
    this->logical.resetFences(fence);

    // Submit command
    this->queues[vk::QueueFlagBits::eGraphics].queue.submit(vk::SubmitInfo().setCommandBufferCount(1).setPCommandBuffers(&cmd), fence);

    // Wait fence
    this->logical.waitForFences(fence, VK_TRUE, (std::numeric_limits<u64>::max)());

    // Free command/fence
    this->logical.destroyFence(fence);
    this->graphics_command_pool->freeCommandBuffers(cmd);
}

void ao::vulkan::Device::copyBufferToImage(vk::Buffer buffer, vk::Image image, u32 width, u32 height) {
    // Create command buffer
    vk::CommandBuffer cmd = this->transfer_command_pool->allocateCommandBuffers(vk::CommandBufferLevel::ePrimary, 1).front();

    cmd.begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
    cmd.copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal,
                          vk::BufferImageCopy(0, 0, 0, vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0, 0, 1), vk::Offset3D(),
                                              vk::Extent3D(vk::Extent2D(width, height), 1)));
    cmd.end();

    // Create fence
    vk::Fence fence = this->logical.createFence(vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled));
    this->logical.resetFences(fence);

    // Submit command
    this->queues[vk::QueueFlagBits::eTransfer].queue.submit(vk::SubmitInfo().setCommandBufferCount(1).setPCommandBuffers(&cmd), fence);

    // Wait fence
    this->logical.waitForFences(fence, VK_TRUE, (std::numeric_limits<u64>::max)());

    // Free command/fence
    this->logical.destroyFence(fence);
    this->transfer_command_pool->freeCommandBuffers(cmd);
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
