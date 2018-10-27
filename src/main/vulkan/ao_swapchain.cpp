#include "ao_swapchain.h"

ao::vk::AOSwapChain::AOSwapChain(VkInstance* instance, AODevice* device) {
	this->instance = instance;
	this->device = device;
}

ao::vk::AOSwapChain::~AOSwapChain() {
	if (this->swapChain) {
		for (auto buffer : this->buffers) {
			vkDestroyImageView(this->device->logical, buffer.second, nullptr);
		}
	}
	
	if (this->surface) {
		vkDestroySwapchainKHR(this->device->logical, swapChain, nullptr);
		vkDestroySurfaceKHR(*this->instance, surface, nullptr);
	}

	this->freeCommandBuffers();
	vkDestroyCommandPool(this->device->logical, this->commandPool, nullptr);
}

void ao::vk::AOSwapChain::init(uint64_t & width, uint64_t & height, bool vsync) {
	// Back-up swap chain
	VkSwapchainKHR old = this->swapChain;

	// Get physical device surface properties and formats
	VkSurfaceCapabilitiesKHR capabilities;
	ao::vk::utilities::vkAssert(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(this->device->physical, surface, &capabilities), "Fail to get physical device surface properties and formats");

	// Find best swap chain size
	VkExtent2D swapchainExtent;
	if (capabilities.currentExtent.width == (uint32_t)-1) {
		swapchainExtent = { (uint32_t)width, (uint32_t)height };
	}
	else {
		if (capabilities.currentExtent.width != width && capabilities.currentExtent.height != height) {
			LOGGER << LogLevel::WARN << "Surface size is defined, change reference size from " << width << "x" << height << " to " << capabilities.currentExtent.width << "x" << capabilities.currentExtent.height;
		}

		swapchainExtent = capabilities.currentExtent;
		width = capabilities.currentExtent.width;
		height = capabilities.currentExtent.height;
	}

	// Select best present mode
	VkPresentModeKHR presentMode = VkPresentModeKHR::VK_PRESENT_MODE_MAX_ENUM_KHR;
	if (vsync) {
		presentMode = VkPresentModeKHR::VK_PRESENT_MODE_FIFO_KHR;
	}
	else {
		// Get present modes
		std::vector<VkPresentModeKHR> presentModes = ao::vk::utilities::presentModeKHRs(this->device->physical, this->surface);

		// Check size
		if (presentModes.empty()) {
			throw ao::core::Exception("VkPresentModeKHR vector is empty");
		}

		for (VkPresentModeKHR& mode : presentModes) {
			if (mode == VkPresentModeKHR::VK_PRESENT_MODE_MAILBOX_KHR) {
				presentMode = VkPresentModeKHR::VK_PRESENT_MODE_MAILBOX_KHR;
				break;
			}
			if (mode == VkPresentModeKHR::VK_PRESENT_MODE_IMMEDIATE_KHR) {
				presentMode = VkPresentModeKHR::VK_PRESENT_MODE_IMMEDIATE_KHR;
			}
		}

		// Check mode
		if (presentMode == VkPresentModeKHR::VK_PRESENT_MODE_MAX_ENUM_KHR) {
			LOGGER << LogLevel::WARN << "Fail to find a suitable present mode, use VkPresentModeKHR::VK_PRESENT_MODE_FIFO_KHR";
			presentMode = VkPresentModeKHR::VK_PRESENT_MODE_FIFO_KHR;
		}
	}

	LOGGER << LogLevel::DEBUG << "Use present mode: " << presentMode;

	// Determine surface image capacity
	uint32_t countSurfaceImages = capabilities.minImageCount + 1;
	if (capabilities.maxImageCount > 0 && countSurfaceImages > capabilities.maxImageCount) {
		countSurfaceImages = capabilities.maxImageCount;
	}

	// Find the transformation of the surface
	VkSurfaceTransformFlagsKHR transform = capabilities.currentTransform;
	if (capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
		transform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	}

	// Find a supported composite alpha format
	VkCompositeAlphaFlagBitsKHR compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	for (VkCompositeAlphaFlagBitsKHR composite : { VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR, VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR, VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR, VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR }) {
		if (capabilities.supportedCompositeAlpha & composite) {
			compositeAlpha = composite;
			break;
		}
	}

	// Create info
	VkSwapchainCreateInfoKHR createInfo = { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
	createInfo.surface = surface;
	createInfo.minImageCount = countSurfaceImages;
	createInfo.imageFormat = colorFormat;
	createInfo.imageColorSpace = colorSpace;
	createInfo.imageExtent = { swapchainExtent.width, swapchainExtent.height };
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	createInfo.preTransform = (VkSurfaceTransformFlagBitsKHR)transform;
	createInfo.imageArrayLayers = 1;
	createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	createInfo.queueFamilyIndexCount = 0;
	createInfo.presentMode = presentMode;
	createInfo.oldSwapchain = old;
	createInfo.clipped = VK_TRUE;
	createInfo.compositeAlpha = compositeAlpha;

	// Enable transfer source
	if (capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT) {
		createInfo.imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	}

	// Enable transfer destination
	if (capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT) {
		createInfo.imageUsage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	}

	// Create swap chain
	ao::vk::utilities::vkAssert(vkCreateSwapchainKHR(this->device->logical, &createInfo, nullptr, &this->swapChain), "Fail to create swap chain");
	
	// Free old swap chain
	if (old) {
		for (auto& buffer : this->buffers) {
			vkDestroyImageView(this->device->logical, buffer.second, nullptr);
		}
		vkDestroySwapchainKHR(this->device->logical, old, nullptr);
	}

	// Get images
	std::vector<VkImage> images = ao::vk::utilities::swapChainImages(this->device->logical, this->swapChain);

	// Resize buffer vector
	this->buffers.resize(images.size());

	// Fill buffer vector
	for (size_t i = 0; i < images.size(); i++) {
		// Create info
		VkImageViewCreateInfo colorCreateInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
		colorCreateInfo.format = colorFormat;
		colorCreateInfo.components = {
			VK_COMPONENT_SWIZZLE_R,
			VK_COMPONENT_SWIZZLE_G,
			VK_COMPONENT_SWIZZLE_B,
			VK_COMPONENT_SWIZZLE_A
		};
		colorCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		colorCreateInfo.subresourceRange.baseMipLevel = 0;
		colorCreateInfo.subresourceRange.levelCount = 1;
		colorCreateInfo.subresourceRange.baseArrayLayer = 0;
		colorCreateInfo.subresourceRange.layerCount = 1;
		colorCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		colorCreateInfo.flags = 0;
		colorCreateInfo.image = images[i];

		// Add in buffer
		buffers[i].first = images[i];

		// Create view
		ao::vk::utilities::vkAssert(vkCreateImageView(this->device->logical, &colorCreateInfo, nullptr, &this->buffers[i].second), "Fail to create image view " + std::to_string(i));
	}

	LOGGER << LogLevel::DEBUG << "Set-up a swap chain with a buffer of " << buffers.size() << " image(s)";
}

void ao::vk::AOSwapChain::initSurface() {
	// Detect if a queue supports present
	std::vector<VkBool32> supportsPresent(this->device->queueFamilyProperties.size());
	for (uint32_t i = 0; i < supportsPresent.size(); i++) {
		vkGetPhysicalDeviceSurfaceSupportKHR(this->device->physical, i, this->surface, &supportsPresent[i]);
	}

	// Try to find a queue that support graphics & present
	int graphicsQueueIndex = -1, presentQueueIndex = -1;
	for (size_t i = 0; i < this->device->queueFamilyProperties.size(); i++) {
		if (this->device->queueFamilyProperties[i].queueFlags & VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT) {
			if (graphicsQueueIndex == -1) {
				graphicsQueueIndex = (int)i;
			}

			if (supportsPresent[i] == VK_TRUE) {
				graphicsQueueIndex = presentQueueIndex = (int)i;
				break;
			}
		}
	}

	// Try to find separate queues
	if (presentQueueIndex == -1) {
		for (size_t i = 0; i < supportsPresent.size(); i++) {
			if (supportsPresent[i] == VK_TRUE) {
				presentQueueIndex = (int)i;
				break;
			}
		}
	}

	// Check indexes
	if (graphicsQueueIndex == -1 || presentQueueIndex == -1) {
		throw ao::core::Exception("Fail to find a queue that supports graphics & present");
	}
	if (graphicsQueueIndex != presentQueueIndex) {
		throw ao::core::Exception("Separate queue for graphics & present is not supported"); // TODO: Add support
	}
	this->queueIndex = graphicsQueueIndex;

	// Get surface formats
	std::vector<VkSurfaceFormatKHR> formats = ao::vk::utilities::surfaceFormatKHRs(this->device->physical, this->surface);

	// Check size
	if (formats.empty()) {
		throw ao::core::Exception("VkSurfaceFormatKHR vector is empty");
	}

	// No prefered format case
	if (formats.size() == 1 && formats.front().format == VkFormat::VK_FORMAT_UNDEFINED) {
		this->colorFormat = VkFormat::VK_FORMAT_B8G8R8A8_UNORM;
		this->colorSpace = formats.front().colorSpace;
	}
	else { // Find VK_FORMAT_B8G8R8A8_UNORM
		bool found = false;

		for (VkSurfaceFormatKHR& format : formats) {
			if (format.format == VkFormat::VK_FORMAT_B8G8R8A8_UNORM) {
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

void ao::vk::AOSwapChain::initCommandPool() {
	// Create info
	VkCommandPoolCreateInfo cmdPoolInfo = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
	cmdPoolInfo.queueFamilyIndex = this->queueIndex;
	cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	// Create command pool
	ao::vk::utilities::vkAssert(vkCreateCommandPool(this->device->logical, &cmdPoolInfo, nullptr, &this->commandPool), "Fail to create command pool");
}

void ao::vk::AOSwapChain::createCommandBuffers() {
	// Create info
	VkCommandBufferAllocateInfo commandBufferAllocateInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
	commandBufferAllocateInfo.commandPool = commandPool;
	commandBufferAllocateInfo.level = VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	commandBufferAllocateInfo.commandBufferCount = (uint32_t)this->buffers.size();

	// Resize buffer vector
	this->commandBuffers.resize(this->buffers.size());

	// Allocate buffers
	ao::vk::utilities::vkAssert(vkAllocateCommandBuffers(this->device->logical, &commandBufferAllocateInfo, this->commandBuffers.data()), "Fail to allocate command buffers");
}

void ao::vk::AOSwapChain::initCommandBuffers(std::vector<VkFramebuffer>& frameBuffers, VkRenderPass& renderPass, ao::vk::WindowSettings& winSettings) {
	VkCommandBufferBeginInfo cmdBufInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };

	// Define clear values for all framebuffer attachments with
	std::array<VkClearValue, 2> clearValues;
	clearValues[0].color = { { 0.0f, 0.0f, 0.0f, 1.0f } };
	clearValues[1].depthStencil = { 1, 0 };

	VkRenderPassBeginInfo renderPassBeginInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
	renderPassBeginInfo.renderPass = renderPass;
	renderPassBeginInfo.renderArea.offset.x = 0;
	renderPassBeginInfo.renderArea.offset.y = 0;
	renderPassBeginInfo.renderArea.extent.width = (uint32_t)winSettings.width;
	renderPassBeginInfo.renderArea.extent.height = (uint32_t)winSettings.height;
	renderPassBeginInfo.clearValueCount = 2;
	renderPassBeginInfo.pClearValues = clearValues.data();

	for (int32_t i = 0; i < this->commandBuffers.size(); ++i) {
		// Set target frame buffer
		renderPassBeginInfo.framebuffer = frameBuffers[i];

		// Begin command buffer
		ao::vk::utilities::vkAssert(vkBeginCommandBuffer(this->commandBuffers[i], &cmdBufInfo), "Fail to begin command buffer");

		// Start the first sub pass specified in our default render pass setup by the base class
		// This will clear the color and depth attachment
		vkCmdBeginRenderPass(this->commandBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		// Update dynamic viewport state
		VkViewport viewport = { 0, 0, (float)winSettings.width, (float)winSettings.height, 0.0f, 1.0f };
		vkCmdSetViewport(this->commandBuffers[i], 0, 1, &viewport);

		// Update dynamic scissor state
		VkRect2D scissor = { 0, 0, (uint32_t)winSettings.width, (uint32_t)winSettings.height };
		vkCmdSetScissor(this->commandBuffers[i], 0, 1, &scissor);

		// TODO: Execute a function

		// End render pass & command buffer
		vkCmdEndRenderPass(this->commandBuffers[i]);
		ao::vk::utilities::vkAssert(vkEndCommandBuffer(this->commandBuffers[i]), "Fail to end command buffer");
	}
}

void ao::vk::AOSwapChain::freeCommandBuffers() {
	vkFreeCommandBuffers(this->device->logical, this->commandPool, (uint32_t)this->commandBuffers.size(), this->commandBuffers.data());
}

VkResult ao::vk::AOSwapChain::nextImage(VkSemaphore& present, uint32_t& imageIndex) {
	return vkAcquireNextImageKHR(this->device->logical, swapChain, UINT64_MAX, present, nullptr, &imageIndex);
}

VkResult ao::vk::AOSwapChain::enqueueImage(VkQueue & queue, uint32_t & imageIndex, VkSemaphore & render) {
	VkPresentInfoKHR presentInfo = { 
		VK_STRUCTURE_TYPE_PRESENT_INFO_KHR, 
		nullptr,
		1,
		&render,
		1,
		&swapChain,
		&imageIndex
	};
	return vkQueuePresentKHR(queue, &presentInfo);
}
