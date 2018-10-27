#include "ao_engine.h"

ao::vk::AOEngine::AOEngine(EngineSettings settings) {
	this->settings = settings;
}

ao::vk::AOEngine::~AOEngine() {
	this->freeVulkan();
}

void ao::vk::AOEngine::run() {
	this->initWindow();
	this->initVulkan();

	this->prepareVulkan();

	this->loop();
}

void ao::vk::AOEngine::initVulkan() {
	// Create instance
	ao::vk::utilities::vkAssert(ao::vk::utilities::createVkInstance(this->settings, this->instance, this->instanceExtensions()), "Fail to create instance");

	// TODO: Set-up debugging if validation layer is enabled
	if (this->settings.vkValidationLayers) {
		ao::vk::utilities::initDebugging(this->instance, VkDebugReportFlagBitsEXT::VK_DEBUG_REPORT_ERROR_BIT_EXT | VkDebugReportFlagBitsEXT::VK_DEBUG_REPORT_WARNING_BIT_EXT, VK_NULL_HANDLE);
	}

	// Get GPUs
	std::vector<VkPhysicalDevice> devices = ao::vk::utilities::vkPhysicalDevices(this->instance);

	// Check count
	if (devices.empty()) {
		throw ao::core::Exception("Unable to find GPUs");
	}

	// Select a VkPhysicalDevice & wrap it
	this->device = new AODevice(devices[this->selectVkPhysicalDevice(devices)]);

	LOGGER << LogLevel::DEBUG << "Select physical device: " << this->device->properties.deviceName;

	// Init logical device
	ao::vk::utilities::vkAssert(this->device->initLogicalDevice(this->deviceExtensions(), this->queueFlags(), this->commandPoolFlags()), "Fail to init logical device");

	// Get a graphics queue from the device
	vkGetDeviceQueue(this->device->logicalDevice, std::get<AO_GRAPHICS_QUEUE_INDEX>(this->device->queueFamilyIndices), 0, &this->queue);

	// Find suitable depth format
	ao::vk::utilities::vkAssert(ao::vk::utilities::getSupportedDepthFormat(this->device->device, this->device->depthFormat), "Fail to find suitable depth format");

	// Create swapChain
	this->swapchain = new AOSwapChain(&this->instance, this->device);

	// Create semaphores
	VkSemaphoreCreateInfo semaphoreInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
	ao::vk::utilities::vkAssert(vkCreateSemaphore(this->device->logicalDevice, &semaphoreInfo, nullptr, &this->semaphores.first), "Fail to create present semaphore");
	ao::vk::utilities::vkAssert(vkCreateSemaphore(this->device->logicalDevice, &semaphoreInfo, nullptr, &this->semaphores.second), "Fail to create render semaphore");

	// Create submit info
	this->submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
	this->submitInfo.pWaitDstStageMask = &this->submitPipelineStages;
	this->submitInfo.waitSemaphoreCount = 1;
	this->submitInfo.pWaitSemaphores = &this->semaphores.first;
	this->submitInfo.signalSemaphoreCount = 1;
	this->submitInfo.pSignalSemaphores = &this->semaphores.second;
}

void ao::vk::AOEngine::freeVulkan() {
	delete this->swapchain;

    /* TODO: 
	if (descriptorPool != VK_NULL_HANDLE) {
		vkDestroyDescriptorPool(device, descriptorPool, nullptr);
	}*/

	vkDestroyRenderPass(this->device->logicalDevice, this->renderPass, nullptr);

	for (uint32_t i = 0; i < frameBuffers.size(); i++) {
		vkDestroyFramebuffer(this->device->logicalDevice, this->frameBuffers[i], nullptr);
	}
	/*
	for (auto& shaderModule : shaderModules) {
		vkDestroyShaderModule(device, shaderModule, nullptr);
	}*/
	
	vkDestroyImageView(this->device->logicalDevice, std::get<2>(this->stencilBuffer), nullptr);
	vkDestroyImage(this->device->logicalDevice, std::get<0>(this->stencilBuffer), nullptr);
	vkFreeMemory(this->device->logicalDevice, std::get<1>(this->stencilBuffer), nullptr);

	vkDestroyPipelineCache(this->device->logicalDevice, this->pipelineCache, nullptr);

	vkDestroySemaphore(this->device->logicalDevice, this->semaphores.first, nullptr);
	vkDestroySemaphore(this->device->logicalDevice, this->semaphores.second, nullptr);

	for (VkFence& fence : this->waitingFences) {
		vkDestroyFence(this->device->logicalDevice, fence, nullptr);
	}

	delete this->device;
	vkDestroyInstance(this->instance, nullptr);
}

void ao::vk::AOEngine::createWaitingFences() {
	// Create info
	VkFenceCreateInfo fenceCreateInfo = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
	fenceCreateInfo.flags = VkFenceCreateFlagBits::VK_FENCE_CREATE_SIGNALED_BIT;
	
	// Resize vector
	this->waitingFences.resize(this->swapchain->commandBuffers.size());
	
	// Create fences
	for (VkFence& fence : this->waitingFences) {
		ao::vk::utilities::vkAssert(vkCreateFence(this->device->logicalDevice, &fenceCreateInfo, nullptr, &fence), "Fail to create fence");
	}
}

void ao::vk::AOEngine::createStencilBuffer() {
	/* CREATE INFO */
	VkImageCreateInfo imageInfo = { 
		VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		nullptr,
		0,
		VK_IMAGE_TYPE_2D,
		this->device->depthFormat,
		{ (uint32_t)this->settings.winSettings.width, (uint32_t)this->settings.winSettings.height, 1 },
		1,
		1,
		VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT,
		VK_IMAGE_TILING_OPTIMAL,
		VkImageUsageFlagBits::VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_SRC_BIT
	};

	VkMemoryAllocateInfo allocInfo = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO, nullptr, 0, 0 };

	VkImageViewCreateInfo depthStencilView = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO, nullptr, 0 };
	depthStencilView.viewType = VK_IMAGE_VIEW_TYPE_2D;
	depthStencilView.format = this->device->depthFormat;
	depthStencilView.subresourceRange = { VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT, 0, 1, 0, 1 };

	// Create image
	ao::vk::utilities::vkAssert(vkCreateImage(this->device->logicalDevice, &imageInfo, nullptr, &std::get<0>(this->stencilBuffer)), "Fail to create image for stencil buffer");

	// Get memory requirements
	VkMemoryRequirements memReqs;
	vkGetImageMemoryRequirements(this->device->logicalDevice, std::get<0>(this->stencilBuffer), &memReqs);
	allocInfo.allocationSize = memReqs.size;
	allocInfo.memoryTypeIndex = this->device->memoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	// Allocate memory
	ao::vk::utilities::vkAssert(vkAllocateMemory(this->device->logicalDevice, &allocInfo, nullptr, &std::get<1>(this->stencilBuffer)), "Fail to allocate memory for stencil buffer");
	ao::vk::utilities::vkAssert(vkBindImageMemory(this->device->logicalDevice, std::get<0>(this->stencilBuffer), std::get<1>(this->stencilBuffer), 0), "Fail to bind image memory for stencil buffer");

	// Create image view
	depthStencilView.image = std::get<0>(this->stencilBuffer);
	ao::vk::utilities::vkAssert(vkCreateImageView(this->device->logicalDevice, &depthStencilView, nullptr, &std::get<2>(this->stencilBuffer)), "Fail to create image view for stencil buffer");
}

void ao::vk::AOEngine::setUpRenderPass() {
	std::array<VkAttachmentDescription, 2> attachments;

	// Color attachment
	attachments[0].format = this->swapchain->colorFormat;
	attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
	attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	// Depth attachment
	attachments[1].format = this->device->depthFormat;
	attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
	attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference colorReference = { 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };
	VkAttachmentReference depthReference = { 1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };

	VkSubpassDescription subpassDescription = {};
	subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpassDescription.colorAttachmentCount = 1;
	subpassDescription.pColorAttachments = &colorReference;
	subpassDescription.pDepthStencilAttachment = &depthReference;
	subpassDescription.inputAttachmentCount = 0;
	subpassDescription.pInputAttachments = nullptr;
	subpassDescription.preserveAttachmentCount = 0;
	subpassDescription.pPreserveAttachments = nullptr;
	subpassDescription.pResolveAttachments = nullptr;

	// Subpass dependencies for layout transitions
	std::array<VkSubpassDependency, 2> dependencies;

	dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[0].dstSubpass = 0;
	dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	dependencies[1].srcSubpass = 0;
	dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	// Create info
	VkRenderPassCreateInfo renderPassInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
	renderPassInfo.attachmentCount = (uint32_t)attachments.size();
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpassDescription;
	renderPassInfo.dependencyCount = (uint32_t)dependencies.size();
	renderPassInfo.pDependencies = dependencies.data();

	// Create render pass
	ao::vk::utilities::vkAssert(vkCreateRenderPass(this->device->logicalDevice, &renderPassInfo, nullptr, &this->renderPass), "Fail to create render pass");
}

void ao::vk::AOEngine::setUpFrameBuffers() {
	std::array<VkImageView, 2> attachments;

	// Depth/Stencil attachment is the same for all frame buffers
	attachments[1] = std::get<2>(this->stencilBuffer);

	// Create info
	VkFramebufferCreateInfo frameBufferCreateInfo = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
	frameBufferCreateInfo.renderPass = renderPass;
	frameBufferCreateInfo.attachmentCount = 2;
	frameBufferCreateInfo.pAttachments = attachments.data();
	frameBufferCreateInfo.width = (uint32_t)this->settings.winSettings.width;
	frameBufferCreateInfo.height = (uint32_t)this->settings.winSettings.height;
	frameBufferCreateInfo.layers = 1;

	// Create frame buffers
	this->frameBuffers.resize(this->swapchain->buffers.size());
	for (uint32_t i = 0; i < frameBuffers.size(); i++) {
		attachments[0] = this->swapchain->buffers[i].second;
		ao::vk::utilities::vkAssert(vkCreateFramebuffer(this->device->logicalDevice, &frameBufferCreateInfo, nullptr, &this->frameBuffers[i]), "Fail to create frame buffer " + std::to_string(i));
	}
}

void ao::vk::AOEngine::prepareVulkan() {
	/*if (this->device.debugMarkers) {
	   // TODO
	}*/

	// Init surface
	this->initSurface(this->swapchain->surface);
	this->swapchain->initSurface();

	// Init command pool
	this->swapchain->initCommandPool();

	// Init swap chain
	this->swapchain->init(this->settings.winSettings.width, this->settings.winSettings.height);

	// Create command buffers
	this->swapchain->createCommandBuffers();

	// Create waiting fences
	this->createWaitingFences();

	// Create stencil buffer
	this->createStencilBuffer();

	// Set-up render pass
	this->setUpRenderPass();

	// Create pipeline cache
	VkPipelineCacheCreateInfo pipelineCacheCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO };
	ao::vk::utilities::vkAssert(vkCreatePipelineCache(this->device->logicalDevice, &pipelineCacheCreateInfo, nullptr, &this->pipelineCache), "Fail to create pipeline cache");

	// Set-up frame buffer
	this->setUpFrameBuffers();

	// Init command buffers
	this->swapchain->initCommandBuffers(this->frameBuffers, this->renderPass, this->settings.winSettings);
}

void ao::vk::AOEngine::loop() {
	while (this->loopingCondition()) {
		this->onLoopIteration();

		// Render frame
		this->render();
	}
}

void ao::vk::AOEngine::onLoopIteration() {}

void ao::vk::AOEngine::render() {
	vkDeviceWaitIdle(this->device->logicalDevice);

	// Prepare frame
	this->prepareFrame();

	// Edit submit info
	this->submitInfo.commandBufferCount = 1;
	this->submitInfo.pCommandBuffers = &this->swapchain->commandBuffers[this->frameBufferIndex];

	// Submit to queue
	ao::vk::utilities::vkAssert(vkQueueSubmit(this->queue, 1, &this->submitInfo, nullptr), "Fail to submit to queue");

	// Submit frame
	this->submitFrame();

	vkDeviceWaitIdle(this->device->logicalDevice);
}

void ao::vk::AOEngine::prepareFrame() {
	VkResult result = this->swapchain->nextImage(this->semaphores.first, this->frameBufferIndex);

	// Check result
	if (result == VkResult::VK_ERROR_OUT_OF_DATE_KHR || result == VkResult::VK_SUBOPTIMAL_KHR) {
		LOGGER << LogLevel::DEBUG << "Swap chain is no longer compatible, re-create it";

		// TODO
		return;
	}
	ao::vk::utilities::vkAssert(result, "Fail to get next image from swap chain");
}

void ao::vk::AOEngine::submitFrame() {
	VkResult result = this->swapchain->enqueueImage(this->queue, this->frameBufferIndex, this->semaphores.second);

	// Check result
	if (result == VkResult::VK_ERROR_OUT_OF_DATE_KHR) {
		LOGGER << LogLevel::DEBUG << "Swap chain is no longer compatible, re-create it";

	    // TODO
		return;
	}
	else if (result == VkResult::VK_SUCCESS || result == VkResult::VK_SUBOPTIMAL_KHR) {
		ao::vk::utilities::vkAssert(vkQueueWaitIdle(this->queue), "Fail to wait queue idle");
	}
	ao::vk::utilities::vkAssert(result, "Fail to enqueue image");
}

std::vector<char const*> ao::vk::AOEngine::deviceExtensions() {
	return std::vector<char const*>();
}

VkQueueFlags ao::vk::AOEngine::queueFlags() {
	return VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT | VkQueueFlagBits::VK_QUEUE_COMPUTE_BIT;
}

VkCommandPoolCreateFlags ao::vk::AOEngine::commandPoolFlags() {
	return VkCommandPoolCreateFlagBits::VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
}

uint8_t ao::vk::AOEngine::selectVkPhysicalDevice(std::vector<VkPhysicalDevice>& devices) {
	return 0;    // First device
}
