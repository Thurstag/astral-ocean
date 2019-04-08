// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#include "swapchain.h"

#include "../utilities/device.h"

ao::vulkan::Swapchain::Swapchain(std::shared_ptr<vk::Instance> instance, std::shared_ptr<Device> device)
    : instance(instance), device(device), frame_index(0), surface_images_count(0), state_(ao::vulkan::SwapchainState::eIdle) {}

ao::vulkan::Swapchain::~Swapchain() {
    for (auto& buffer : this->buffers) {
        this->device->logical()->destroyImageView(buffer.second);
    }

    this->device->logical()->destroySwapchainKHR(this->swapchain);
    this->instance->destroySurfaceKHR(this->surface);

    this->command_pool.reset();

    for (auto& framebuffer : this->frames) {
        this->device->logical()->destroyFramebuffer(framebuffer);
    }

    if (this->stencil_buffer) {
        this->destroyStencilBuffer();
    }
}

void ao::vulkan::Swapchain::init(u64& win_width, u64& win_height, bool vsync, bool stencil_buffer) {
    // Back-up swap chain
    vk::SwapchainKHR old = this->swapchain;
    bool first_init = !old;

    // Get physical device surface properties and formats
    vk::SurfaceCapabilitiesKHR capabilities = this->device->physical().getSurfaceCapabilitiesKHR(this->surface);

    // Find best swap chain size
    if (capabilities.currentExtent.width == (u32)-1) {
        this->extent_ = vk::Extent2D(static_cast<u32>(win_width), static_cast<u32>(win_height));
    } else {
        if (capabilities.currentExtent.width != win_width || capabilities.currentExtent.height != win_height) {
            LOGGER << ao::core::Logger::Level::debug
                   << fmt::format("Surface size is defined, change reference size from {0}x{1} to {2}x{3}", win_width, win_height,
                                  capabilities.currentExtent.width, capabilities.currentExtent.height);
        }

        this->extent_ = capabilities.currentExtent;
        win_width = capabilities.currentExtent.width;
        win_height = capabilities.currentExtent.height;
    }

    // Select best present mode
    vk::PresentModeKHR present_mode = vk::PresentModeKHR::eFifo;
    if (vsync) {
        present_mode = vk::PresentModeKHR::eFifo;
    } else {
        // Get present modes
        std::vector<vk::PresentModeKHR> present_modes = this->device->physical().getSurfacePresentModesKHR(surface);

        // Check size
        if (present_modes.empty()) {
            throw ao::core::Exception("vk::PresentModeKHR vector is empty");
        }

        for (auto& mode : present_modes) {
            if (mode == vk::PresentModeKHR::eMailbox) {
                present_mode = vk::PresentModeKHR::eMailbox;
                break;
            }
            if (mode == vk::PresentModeKHR::eImmediate) {
                present_mode = vk::PresentModeKHR::eImmediate;
            }
        }
    }

    LOGGER << ao::core::Logger::Level::info << fmt::format("Use present mode: {0}", vk::to_string(present_mode));

    // Determine surface image capacity
    if (first_init) {
        this->surface_images_count = capabilities.minImageCount + 1;
        if (capabilities.maxImageCount > 0 && this->surface_images_count > capabilities.maxImageCount) {
            this->surface_images_count = capabilities.maxImageCount;
        }
    }

    // Find the transformation of the surface
    vk::SurfaceTransformFlagBitsKHR transform = capabilities.currentTransform;
    if (capabilities.supportedTransforms & vk::SurfaceTransformFlagBitsKHR::eIdentity) {
        transform = vk::SurfaceTransformFlagBitsKHR::eIdentity;
    }

    // Find a supported composite alpha format
    vk::CompositeAlphaFlagBitsKHR composite_alpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
    for (auto composite : {vk::CompositeAlphaFlagBitsKHR::eOpaque, vk::CompositeAlphaFlagBitsKHR::ePreMultiplied,
                           vk::CompositeAlphaFlagBitsKHR::ePostMultiplied, vk::CompositeAlphaFlagBitsKHR::eInherit}) {
        if (capabilities.supportedCompositeAlpha & composite) {
            composite_alpha = composite;
            break;
        }
    }

    // Create info
    vk::SwapchainCreateInfoKHR create_info(vk::SwapchainCreateFlagsKHR(), surface, this->surface_images_count, this->surface_color_format,
                                           this->surface_color_space, vk::Extent2D(this->extent_.width, this->extent_.height), 1,
                                           vk::ImageUsageFlagBits::eColorAttachment, vk::SharingMode::eExclusive, 0, nullptr, transform,
                                           composite_alpha, present_mode, true, old);

    // Enable transfer source
    if (capabilities.supportedUsageFlags & vk::ImageUsageFlagBits::eTransferSrc) {
        create_info.imageUsage |= vk::ImageUsageFlagBits::eTransferSrc;
    }

    // Enable transfer destination
    if (capabilities.supportedUsageFlags & vk::ImageUsageFlagBits::eTransferDst) {
        create_info.imageUsage |= vk::ImageUsageFlagBits::eTransferDst;
    }

    // Create swap chain
    this->swapchain = this->device->logical()->createSwapchainKHR(create_info);

    // Free old swap chain
    if (old) {
        for (auto& buffer : this->buffers) {
            this->device->logical()->destroyImageView(buffer.second);
        }
        this->device->logical()->destroySwapchainKHR(old);

        this->state_ = ao::vulkan::SwapchainState::eReset;
    }

    // Get images
    std::vector<vk::Image> images = this->device->swapChainImages(this->swapchain);

    // Resize buffer vector
    this->buffers.resize(images.size());

    // Fill buffer vector
    for (size_t i = 0; i < images.size(); i++) {
        // Create info
        vk::ImageViewCreateInfo color_info(
            vk::ImageViewCreateFlags(), images[i], vk::ImageViewType::e2D, this->surface_color_format,
            vk::ComponentMapping(vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG, vk::ComponentSwizzle::eB, vk::ComponentSwizzle::eA),
            vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));

        // Add in buffer
        buffers[i].first = images[i];

        // Create view
        this->buffers[i].second = this->device->logical()->createImageView(color_info);
    }

    LOGGER << ao::core::Logger::Level::debug << fmt::format("Set-up a swap chain of {0} image{1}", buffers.size(), buffers.size() > 1 ? "s" : "");

    // Create stencil buffer
    if (stencil_buffer) {
        // Destroy old one
        if (this->stencil_buffer) {
            this->destroyStencilBuffer();
        }

        this->createStencilBuffer();
    }

    if (first_init) {
        // Create command pool
        this->command_pool =
            std::make_unique<ao::vulkan::CommandPool>(this->device->logical(), vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
                                                      this->device->queues()->at(vk::to_string(vk::QueueFlagBits::eGraphics)).family_index);

        // Create commands
        this->commands = this->command_pool->allocateCommandBuffers(vk::CommandBufferLevel::ePrimary, static_cast<u32>(this->buffers.size()));
    }
}

void ao::vulkan::Swapchain::initSurface() {
    // Detect if a queue supports present
    std::vector<vk::Bool32> support_present(this->device->physical().getQueueFamilyProperties().size());
    for (u32 i = 0; i < support_present.size(); i++) {
        support_present[i] = this->device->physical().getSurfaceSupportKHR(i, this->surface);
    }

    // Try to find a queue that supports present
    std::optional<std::string> queue_name;
    for (auto& [key, value] : *this->device->queues()) {
        if (support_present[value.family_index] == VK_TRUE) {
            queue_name = key;
            break;
        }
    }

    // Check index
    if (!queue_name) {
        throw core::Exception("Fail to find a queue that supports present");
    }

    LOGGER << ao::core::Logger::Level::trace << fmt::format("Use {0} queue to present images", *queue_name);
    this->present_queue = this->device->queues()->at(*queue_name).value;

    // Get surface formats
    std::vector<vk::SurfaceFormatKHR> formats = this->device->surfaceFormatKHRs(this->surface);

    // Check size
    if (formats.empty()) {
        throw ao::core::Exception("vk::SurfaceFormatKHR vector is empty");
    }

    // No prefered format case
    if (formats.size() == 1 && formats.front().format == vk::Format::eUndefined) {
        this->surface_color_format = vk::Format::eB8G8R8A8Unorm;
        this->surface_color_space = formats.front().colorSpace;
    } else {  // Find eB8G8R8A8Unorm
        bool found = false;

        for (auto& format : formats) {
            if (format.format == vk::Format::eB8G8R8A8Unorm) {
                this->surface_color_format = format.format;
                this->surface_color_space = format.colorSpace;
                found = true;
                break;
            }
        }

        // Select the first format
        if (!found) {
            this->surface_color_format = formats.front().format;
            this->surface_color_space = formats.front().colorSpace;
        }
    }
}

void ao::vulkan::Swapchain::createFramebuffers(vk::RenderPass render_pass) {
    // Depth/Stencil attachment is the same for all framebuffers
    std::array<vk::ImageView, 2> attachments;
    if (this->stencil_buffer) {
        attachments[1] = std::get<2>(*this->stencil_buffer);
    }

    // Create framebuffers
    this->frames.resize(this->buffers.size());
    for (u32 i = 0; i < frames.size(); i++) {
        attachments[0] = this->buffers[i].second;

        this->frames[i] = this->device->logical()->createFramebuffer(
            vk::FramebufferCreateInfo(vk::FramebufferCreateFlags(), render_pass, static_cast<u32>(attachments.size()), attachments.data(),
                                      this->extent_.width, this->extent_.height, 1));
    }
}

void ao::vulkan::Swapchain::createStencilBuffer() {
    // Find best depth format
    auto depth_format = ao::vulkan::utilities::bestDepthStencilFormat(this->device->physical());

    // Create image and it's view
    auto image = ao::vulkan::utilities::createImage(*this->device->logical(), this->device->physical(), this->extent_.width, this->extent_.height, 1,
                                                    1, depth_format, vk::ImageType::e2D, vk::ImageTiling::eOptimal,
                                                    vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::MemoryPropertyFlagBits::eDeviceLocal);
    vk::ImageView view = ao::vulkan::utilities::createImageView(*this->device->logical(), image.first, depth_format, vk::ImageViewType::e2D,
                                                                vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eDepth, 0, 1, 0, 1));

    // Assign
    this->stencil_buffer = std::make_optional(std::make_tuple(image.first, image.second, view));

    // Change image's layout
    ao::vulkan::utilities::updateImageLayout(*this->device->logical(), this->device->graphicsPool(), *this->device->queues(),
                                             std::get<0>(*this->stencil_buffer), depth_format,
                                             vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eDepth, 0, 1, 0, 1), vk::ImageLayout::eUndefined,
                                             vk::ImageLayout::eDepthStencilAttachmentOptimal);
}

void ao::vulkan::Swapchain::destroyFramebuffers() {
    for (auto& frame : this->frames) {
        this->device->logical()->destroyFramebuffer(frame);
    }
    this->frames.clear();
}

void ao::vulkan::Swapchain::destroyStencilBuffer() {
    this->device->logical()->destroyImageView(std::get<2>(*this->stencil_buffer));
    this->device->logical()->destroyImage(std::get<0>(*this->stencil_buffer));
    this->device->logical()->freeMemory(std::get<1>(*this->stencil_buffer));
}

vk::Result ao::vulkan::Swapchain::acquireNextImage(vk::Semaphore acquire) {
    this->state_ = ao::vulkan::SwapchainState::eAcquireImage;

    return this->device->logical()->acquireNextImageKHR(this->swapchain, (std::numeric_limits<u64>::max)(), acquire, nullptr, &this->frame_index);
}

vk::Result ao::vulkan::Swapchain::enqueueImage(vk::ArrayProxy<vk::Semaphore> waiting_semaphores) {
    vk::PresentInfoKHR present_info(static_cast<u32>(waiting_semaphores.size()), waiting_semaphores.empty() ? nullptr : waiting_semaphores.data(), 1,
                                    &this->swapchain, &this->frame_index);
    this->state_ = ao::vulkan::SwapchainState::eEnqueueImage;

    // Pass a pointer to don't trigger an exception
    return this->present_queue.presentKHR(&present_info);
}

ao::vulkan::Swapchain* ao::vulkan::Swapchain::setSurface(vk::SurfaceKHR surface) {
    this->surface = surface;
    return this;
}
