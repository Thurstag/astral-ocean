// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <ao/core/exception/exception.h>
#include <ao/core/utilities/types.h>
#include <fmt/format.h>
#include <vulkan/vulkan.hpp>

#include "../container/queue_container.h"
#include "../wrapper/command_pool.h"
#include "../wrapper/fence.h"

namespace ao::vulkan::utilities {

    /**
     * @brief Find a suitable vk::Format in an list of formats based on requirements
     *
     * @param physical_device Device
     * @param formats Formats
     * @param tiling Tiling
     * @param features Format features
     * @return vk::Format Format
     */
    inline vk::Format suitableFormat(vk::PhysicalDevice physical_device, std::initializer_list<vk::Format> formats, vk::ImageTiling tiling,
                                     vk::FormatFeatureFlags features) {
        for (auto& format : formats) {
            vk::FormatProperties properties = physical_device.getFormatProperties(format);

            if ((tiling == vk::ImageTiling::eLinear && (properties.linearTilingFeatures & features) == features) ||
                (tiling == vk::ImageTiling::eOptimal && (properties.optimalTilingFeatures & features) == features)) {
                return format;
            }
        }
        throw core::Exception("Fail to find a suitable vk::Format");
    }

    /**
     * @brief Find best vk::Format for depth stencil attachment
     *
     * @param physical_device Device
     * @return vk::Format Format
     */
    inline vk::Format bestDepthStencilFormat(vk::PhysicalDevice physical_device) {
        return suitableFormat(physical_device, {vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint},
                              vk::ImageTiling::eOptimal, vk::FormatFeatureFlagBits::eDepthStencilAttachment);
    }

    /**
     * @brief Find memory type
     *
     * @param device
     * @param memory_type_bits Type bits
     * @param properties Properties
     * @return u32 Index
     */
    inline u32 memoryType(vk::PhysicalDevice device, u32 memory_type_bits, vk::MemoryPropertyFlags properties) {
        auto mem_properties = device.getMemoryProperties();

        for (u32 i = 0; i < mem_properties.memoryTypeCount; i++) {
            if ((memory_type_bits & 1) == 1) {
                if ((mem_properties.memoryTypes[i].propertyFlags & properties) == properties) {
                    return i;
                }
            }
            memory_type_bits >>= 1;
        }
        throw ao::core::Exception("Fail to find a matching memory type");
    }

    /**
     * @brief Create a vk::Image
     *
     * @param device Logical device
     * @param physical_device Physical device
     * @param width Width
     * @param height height
     * @param mip_levels Mip levels
     * @param array_layers Array layers
     * @param format Format
     * @param type Type
     * @param tilling Tilling
     * @param usage_flags Usage flags
     * @param memory_flags Memory flags
     * @return std::pair<vk::Image, vk::DeviceMemory> Image
     */
    inline std::pair<vk::Image, vk::DeviceMemory> createImage(vk::Device device, vk::PhysicalDevice physical_device, u32 width, u32 height,
                                                              u32 mip_levels, u32 array_layers, vk::Format format, vk::ImageType type,
                                                              vk::ImageTiling tiling, vk::ImageUsageFlags usage_flags,
                                                              vk::MemoryPropertyFlags memory_flags) {
        std::pair<vk::Image, vk::DeviceMemory> pair;

        // Create image
        pair.first = device.createImage(vk::ImageCreateInfo(vk::ImageCreateFlags(), type, format, vk::Extent3D(vk::Extent2D(width, height), 1),
                                                            mip_levels, array_layers, vk::SampleCountFlagBits::e1, tiling, usage_flags,
                                                            vk::SharingMode::eExclusive, 0, nullptr, vk::ImageLayout::eUndefined));

        // Get memory requirements
        vk::MemoryRequirements mem_requirements = device.getImageMemoryRequirements(pair.first);

        // Allocate	memory
        pair.second = device.allocateMemory(
            vk::MemoryAllocateInfo(mem_requirements.size, memoryType(physical_device, mem_requirements.memoryTypeBits, memory_flags)));

        // Bind image and memory
        device.bindImageMemory(pair.first, pair.second, 0);
        return pair;
    }

    /**
     * @brief Create a vk::ImageView
     *
     * @param device Device
     * @param image
     * @param format
     * @param view_type
     * @param subresource_range
     * @return vk::ImageView
     */
    inline vk::ImageView createImageView(vk::Device device, vk::Image image, vk::Format format, vk::ImageViewType view_type,
                                         vk::ImageSubresourceRange subresource_range) {
        return device.createImageView(
            vk::ImageViewCreateInfo(vk::ImageViewCreateFlags(), image, view_type, format, vk::ComponentMapping(), subresource_range));
    }

    /**
     * @brief Update image's layout
     *
     * @param device Device
     * @param graphics_command_pool Graphics command pool
     * @param queue_container Queue container
     * @param image Image
     * @param format Format
     * @param subresource_range Subresource range
     * @param old_layout Old layout
     * @param new_layout New layout
     */
    inline void updateImageLayout(vk::Device device, CommandPool& graphics_command_pool, QueueContainer& queue_container, vk::Image image,
                                  vk::Format format, vk::ImageSubresourceRange subresource_range, vk::ImageLayout old_layout,
                                  vk::ImageLayout new_layout) {
        // Create command buffer
        vk::CommandBuffer cmd = graphics_command_pool.allocateCommandBuffers(vk::CommandBufferLevel::ePrimary, 1).front();

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
        Fence fence(std::make_shared<vk::Device>(device));

        // Submit command
        queue_container.submit(vk::QueueFlagBits::eGraphics, vk::SubmitInfo().setCommandBufferCount(1).setPCommandBuffers(&cmd), fence);

        // Wait fence
        fence.wait();

        // Free command/fence
        graphics_command_pool.freeCommandBuffers(cmd);
        fence.destroy();
    }

    /**
     * @brief Copy vk::Buffer into a vk::Image
     *
     * @param device Device
     * @param transfer_command_pool Transfer command pool
     * @param queue_container Queue container
     * @param buffer Buffer
     * @param image Image
     * @param regions Regions
     */
    inline void copyBufferToImage(vk::Device device, CommandPool& transfer_command_pool, QueueContainer& queue_container, vk::Buffer buffer,
                                  vk::Image image, vk::ArrayProxy<vk::BufferImageCopy const> regions) {
        // Create command buffer
        vk::CommandBuffer cmd = transfer_command_pool.allocateCommandBuffers(vk::CommandBufferLevel::ePrimary, 1).front();

        cmd.begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
        cmd.copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal, regions);
        cmd.end();

        // Create fence
        Fence fence(std::make_shared<vk::Device>(device));

        // Submit command
        queue_container.submit(vk::QueueFlagBits::eTransfer, vk::SubmitInfo().setCommandBufferCount(1).setPCommandBuffers(&cmd), fence);

        // Wait fence
        fence.wait();

        // Free command/fence
        transfer_command_pool.freeCommandBuffers(cmd);
        fence.destroy();
    }
}  // namespace ao::vulkan::utilities
