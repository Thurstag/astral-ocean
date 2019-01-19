// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#include <ao/core/utilities/pointers.h>

#include "swapchain.h"

ao::vulkan::SwapChain::SwapChain(std::weak_ptr<vk::Instance> _instance, std::weak_ptr<Device> _device) : instance(_instance), device(_device), commandBuffers(_device) {
	// Init helpers
	this->commandBufferHelpers.first[0].setColor(vk::ClearColorValue());
	this->commandBufferHelpers.first[1].setDepthStencil(vk::ClearDepthStencilValue(1));
}

ao::vulkan::SwapChain::~SwapChain() {
	if (auto _device = ao::core::shared(this->device)) {
		for (auto& buffer : this->buffers) {
			_device->logical.destroyImageView(buffer.second);
		}
		this->buffers.clear();

		_device->logical.destroySwapchainKHR(this->swapChain);
		if (auto _instance = ao::core::shared(this->instance)) {
			_instance->destroySurfaceKHR(this->surface);
		}

		this->freeCommandBuffers();
		_device->logical.destroyCommandPool(this->commandPool);
	}
}

void ao::vulkan::SwapChain::init(u64 & width, u64 & height, bool vsync) {
	// Back-up swap chain
	vk::SwapchainKHR old = this->swapChain;

	auto _device = ao::core::shared(this->device);

	// Get physical device surface properties and formats
	vk::SurfaceCapabilitiesKHR capabilities = _device->physical.getSurfaceCapabilitiesKHR(this->surface);

	// Find best swap chain size
	if (capabilities.currentExtent.width == (u32)-1) {
		this->currentExtent = vk::Extent2D(static_cast<u32>(width), static_cast<u32>(height));
	}
	else {
		if (capabilities.currentExtent.width != width || capabilities.currentExtent.height != height) {
			LOGGER << ao::core::LogLevel::warning << fmt::format("Surface size is defined, change reference size from {0}x{1} to {2}x{3}", width, height, capabilities.currentExtent.width, capabilities.currentExtent.height);
		}

		this->currentExtent = capabilities.currentExtent;
		width = capabilities.currentExtent.width;
		height = capabilities.currentExtent.height;
	}

	// Select best present mode
	vk::PresentModeKHR presentMode = vk::PresentModeKHR::eFifo;
	if (vsync) {
		presentMode = vk::PresentModeKHR::eFifo;
	}
	else {
		// Get present modes
		std::vector<vk::PresentModeKHR> presentModes = ao::vulkan::utilities::presentModeKHRs(_device->physical, this->surface);

		// Check size
		if (presentModes.empty()) {
			throw ao::core::Exception("vk::PresentModeKHR vector is empty");
		}

		for (auto& mode : presentModes) {
			if (mode == vk::PresentModeKHR::eMailbox) {
				presentMode = vk::PresentModeKHR::eMailbox;
				break;
			}
			if (mode == vk::PresentModeKHR::eImmediate) {
				presentMode = vk::PresentModeKHR::eImmediate;
			}
		}
	}

	LOGGER << ao::core::LogLevel::info << fmt::format("Use present mode: {0}", ao::vulkan::utilities::to_string(presentMode));

	// Determine surface image capacity
	u32 countSurfaceImages = capabilities.minImageCount + 1;
	if (capabilities.maxImageCount > 0 && countSurfaceImages > capabilities.maxImageCount) {
		countSurfaceImages = capabilities.maxImageCount;
	}

	// Find the transformation of the surface
	vk::SurfaceTransformFlagBitsKHR transform = capabilities.currentTransform;
	if (capabilities.supportedTransforms & vk::SurfaceTransformFlagBitsKHR::eIdentity) {
		transform = vk::SurfaceTransformFlagBitsKHR::eIdentity;
	}

	// Find a supported composite alpha format
	vk::CompositeAlphaFlagBitsKHR compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
	for (auto composite : { vk::CompositeAlphaFlagBitsKHR::eOpaque, vk::CompositeAlphaFlagBitsKHR::ePreMultiplied, vk::CompositeAlphaFlagBitsKHR::ePostMultiplied, vk::CompositeAlphaFlagBitsKHR::eInherit }) {
		if (capabilities.supportedCompositeAlpha & composite) {
			compositeAlpha = composite;
			break;
		}
	}

	// Create info
	vk::SwapchainCreateInfoKHR createInfo(
		vk::SwapchainCreateFlagsKHR(), surface, countSurfaceImages,
		colorFormat, colorSpace, vk::Extent2D(this->currentExtent.width, this->currentExtent.height),
		1, vk::ImageUsageFlagBits::eColorAttachment, vk::SharingMode::eExclusive, 0, nullptr,
		transform, compositeAlpha, presentMode, true, old
	);

	// Enable transfer source
	if (capabilities.supportedUsageFlags & vk::ImageUsageFlagBits::eTransferSrc) {
		createInfo.imageUsage |= vk::ImageUsageFlagBits::eTransferSrc;
	}

	// Enable transfer destination
	if (capabilities.supportedUsageFlags & vk::ImageUsageFlagBits::eTransferDst) {
		createInfo.imageUsage |= vk::ImageUsageFlagBits::eTransferDst;
	}

	// Create swap chain
	this->swapChain = _device->logical.createSwapchainKHR(createInfo);

	// Free old swap chain
	if (old) {
		for (auto& buffer : this->buffers) {
			_device->logical.destroyImageView(buffer.second);
		}
		_device->logical.destroySwapchainKHR(old);
	}

	// Get images
	std::vector<vk::Image> images = ao::vulkan::utilities::swapChainImages(_device->logical, this->swapChain);

	// Resize buffer vector
	this->buffers.resize(images.size());

	// Fill buffer vector
	for (size_t i = 0; i < images.size(); i++) {
		// Create info
		vk::ImageViewCreateInfo colorCreateInfo(
			vk::ImageViewCreateFlags(), images[i], vk::ImageViewType::e2D, colorFormat,
			vk::ComponentMapping(vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG, vk::ComponentSwizzle::eB, vk::ComponentSwizzle::eA),
			vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1)
		);

		// Add in buffer
		buffers[i].first = images[i];

		// Create view
		this->buffers[i].second = _device->logical.createImageView(colorCreateInfo);
	}

	LOGGER << ao::core::LogLevel::debug << fmt::format("Set-up a swap chain of {0} image{1}", buffers.size(), buffers.size() > 1 ? "s" : "");

	// Update helpers
	this->commandBufferHelpers.second = vk::Rect2D(vk::Offset2D(), this->currentExtent);
}

void ao::vulkan::SwapChain::initSurface() {
	auto _device = ao::core::shared(this->device);

	// Detect if a queue supports present
	std::vector<vk::Bool32> supportsPresent(_device->physical.getQueueFamilyProperties().size());
	for (u32 i = 0; i < supportsPresent.size(); i++) {
		supportsPresent[i] = _device->physical.getSurfaceSupportKHR(i, this->surface);
	}
	
	// Try to find a queue that supports present
	std::optional<vk::QueueFlagBits> flag;
	for (auto& pair : _device->queues) {
		if (supportsPresent[pair.second.index] == VK_TRUE) {
			flag = pair.first;

			LOGGER << ao::core::LogLevel::debug << fmt::format("Use {0} queue to present images", to_string(pair.first));
			break;
		}
	}

	// Check index
	if (!flag) {
		throw core::Exception("Fail to find a queue that supports present");
	}
	this->presentQueue = _device->queues[*flag].queue;

	// Get surface formats
	std::vector<vk::SurfaceFormatKHR> formats = ao::vulkan::utilities::surfaceFormatKHRs(_device->physical, this->surface);

	// Check size
	if (formats.empty()) {
		throw ao::core::Exception("vk::SurfaceFormatKHR vector is empty");
	}

	// No prefered format case
	if (formats.size() == 1 && formats.front().format == vk::Format::eUndefined) {
		this->colorFormat = vk::Format::eB8G8R8A8Unorm;
		this->colorSpace = formats.front().colorSpace;
	}
	else { // Find VK_FORMAT_B8G8R8A8_UNORM
		bool found = false;

		for (auto& format : formats) {
			if (format.format == vk::Format::eB8G8R8A8Unorm) {
				colorFormat = format.format;
				colorSpace = format.colorSpace;
				found = true;
				break;
			}
		}

		// Select the first format
		if (!found) {
			this->colorFormat = formats.front().format;
			this->colorSpace = formats.front().colorSpace;
		}
	}
}

void ao::vulkan::SwapChain::initCommandPool() {
	if (auto _device = ao::core::shared(this->device)) {
		this->commandPool = _device->logical.createCommandPool(vk::CommandPoolCreateInfo(vk::CommandPoolCreateFlagBits::eResetCommandBuffer, _device->queues[vk::QueueFlagBits::eGraphics].index));
	}
}

void ao::vulkan::SwapChain::createCommandBuffers() {
	// Allocate buffers
	std::vector<vk::CommandBuffer> buffers = ao::core::shared(this->device)->logical.allocateCommandBuffers(
		vk::CommandBufferAllocateInfo(this->commandPool, vk::CommandBufferLevel::ePrimary, static_cast<u32>(this->buffers.size()))
	);

	// Add to container
	this->commandBuffers["primary"] = ao::vulkan::CommandBufferData(buffers, this->commandPool);
}

void ao::vulkan::SwapChain::freeCommandBuffers() {
	this->commandBuffers.clear();
}

vk::Result ao::vulkan::SwapChain::nextImage(vk::Semaphore& acquire, u32& imageIndex) {
	if (auto _device = ao::core::shared(this->device)) {
		return _device->logical.acquireNextImageKHR(this->swapChain, (std::numeric_limits<u64>::max)(), acquire, nullptr, &imageIndex);
	}
	return vk::Result::eErrorDeviceLost;
}

vk::Result ao::vulkan::SwapChain::enqueueImage(u32 & imageIndex, std::vector<vk::Semaphore> & waitSemaphores) {
	vk::PresentInfoKHR presentInfo(static_cast<u32>(waitSemaphores.size()), waitSemaphores.empty() ? nullptr : waitSemaphores.data(), 1, &this->swapChain, &imageIndex);

	// Pass a pointer to don't trigger an exception
	return this->presentQueue.presentKHR(&presentInfo);
}
