#include "ao_engine.h"

ao::vulkan::AOEngine::AOEngine(EngineSettings settings) {
	this->_settings = settings;
}

ao::vulkan::AOEngine::~AOEngine() {
	// Execute plugins' onInit()
	this->pluginsMutex.lock();
	{
		for (size_t i = 0; i < this->plugins.size(); i++) {
			this->plugins[i]->beforeDestroy();
		}
	}
	this->pluginsMutex.unlock();

	this->freeVulkan();
	this->freePlugins();
}

void ao::vulkan::AOEngine::run() {
	this->initWindow();
	this->initVulkan();

	this->prepareVulkan();

	// Execute plugins' onInit()
	this->pluginsMutex.lock();
	{
		for (size_t i = 0; i < this->plugins.size(); i++) {
			this->plugins[i]->onInit();
		}
	}
	this->pluginsMutex.unlock();

	this->loop();
}

void ao::vulkan::AOEngine::add(ao::core::Plugin<AOEngine> * plugin) {
	this->pluginsMutex.lock();
	{
		this->plugins.push_back(plugin);
	}
	this->pluginsMutex.unlock();
}

void ao::vulkan::AOEngine::initVulkan() {
	// Create instance
	this->instance = utilities::createVkInstance(this->_settings, this->instanceExtensions());

	// TODO: Set-up debugging if validation layer is enabled
	//if (this->_settings.vkValidationLayers) {
	//	ao::vulkan::utilities::initDebugging(this->instance, vk::DebugReportFlagBitsEXT::VK_DEBUG_REPORT_ERROR_BIT_EXT | vk::DebugReportFlagBitsEXT::VK_DEBUG_REPORT_WARNING_BIT_EXT, VK_NULL_HANDLE);
	//}

	// Get GPUs
	std::vector<vk::PhysicalDevice> devices = ao::vulkan::utilities::vkPhysicalDevices(this->instance);

	// Check count
	if (devices.empty()) {
		throw ao::core::Exception("Unable to find GPUs");
	}

	// Select a vk::PhysicalDevice & wrap it
	this->device = new AODevice(devices[this->selectVkPhysicalDevice(devices)]);

	LOGGER << LogLevel::INFO << "Select physical device: " << this->device->physical.getProperties().deviceName;

	// Init logical device
	this->device->initLogicalDevice(this->deviceExtensions(), this->queueFlags(), this->commandPoolFlags());

	// Get a graphics queue from the device
	this->queue = this->device->logical.getQueue(std::get<AO_GRAPHICS_QUEUE_INDEX>(this->device->queueFamilyIndices), 0);

	// Find suitable depth format
	this->device->depthFormat = ao::vulkan::utilities::getSupportedDepthFormat(this->device->physical);

	// Create swapChain
	this->swapchain = new AOSwapChain(&this->instance, this->device);

	// Create semaphores
	this->semaphores.first = this->device->logical.createSemaphore(vk::SemaphoreCreateInfo());
	this->semaphores.second = this->device->logical.createSemaphore(vk::SemaphoreCreateInfo());

	// Create submit info
	this->submitInfo = vk::SubmitInfo(1, &this->semaphores.first, &this->submitPipelineStages, 0, nullptr, 1, &this->semaphores.second);
}

void ao::vulkan::AOEngine::freeVulkan() {
	delete this->swapchain;

    /* TODO: 
	if (descriptorPool != VK_NULL_HANDLE) {
		vkDestroyDescriptorPool(device, descriptorPool, nullptr);
	}*/

	this->device->logical.destroyRenderPass(this->renderPass);

	for (uint32_t i = 0; i < frameBuffers.size(); i++) {
		this->device->logical.destroyFramebuffer(this->frameBuffers[i]);
	}
	this->frameBuffers.clear();

	/*
	for (auto& shaderModule : shaderModules) {
		vkDestroyShaderModule(device, shaderModule, nullptr);
	}*/
	
	this->device->logical.destroyImageView(std::get<2>(this->stencilBuffer));
	this->device->logical.destroyImage(std::get<0>(this->stencilBuffer));
	this->device->logical.freeMemory(std::get<1>(this->stencilBuffer));

	this->device->logical.destroyPipelineCache(this->pipelineCache);

	this->device->logical.destroySemaphore(this->semaphores.first);
	this->device->logical.destroySemaphore(this->semaphores.second);

	for (vk::Fence& fence : this->waitingFences) {
		this->device->logical.destroyFence(fence);
	}
	this->waitingFences.clear();

	delete this->device;
	this->instance.destroy();
}

void ao::vulkan::AOEngine::createWaitingFences() {
	// Resize vector
	this->waitingFences.resize(this->swapchain->commandBuffers.size());
	
	// Create fences
	for (vk::Fence& fence : this->waitingFences) {
		fence = this->device->logical.createFence(vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled));
	}
}

void ao::vulkan::AOEngine::createStencilBuffer() {
	// Create info
	vk::ImageCreateInfo imageInfo(
		vk::ImageCreateFlags(), vk::ImageType::e2D, this->device->depthFormat,
		vk::Extent3D(static_cast<uint32_t>(this->_settings.window.width), static_cast<uint32_t>(this->_settings.window.height), 1),
		1, 1, vk::SampleCountFlagBits::e1, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eTransferSrc
	);

	vk::MemoryAllocateInfo allocInfo;

	vk::ImageViewCreateInfo depthStencilView(
		vk::ImageViewCreateFlags(), vk::Image(), vk::ImageViewType::e2D,
		this->device->depthFormat, vk::ComponentMapping(),
		vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil, 0, 1, 0, 1)
	);

	// Create image
	std::get<0>(this->stencilBuffer) = this->device->logical.createImage(imageInfo);

	// Get memory requirements
	vk::MemoryRequirements memReqs = this->device->logical.getImageMemoryRequirements(std::get<0>(this->stencilBuffer));
	allocInfo.setAllocationSize(memReqs.size);
	allocInfo.setMemoryTypeIndex(this->device->memoryType(memReqs.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal));

	// Allocate memory
	std::get<1>(this->stencilBuffer) = this->device->logical.allocateMemory(allocInfo);
	this->device->logical.bindImageMemory(std::get<0>(this->stencilBuffer), std::get<1>(this->stencilBuffer), 0);

	// Create image view
	depthStencilView.setImage(std::get<0>(this->stencilBuffer));
	std::get<2>(this->stencilBuffer) = this->device->logical.createImageView(depthStencilView);
}

void ao::vulkan::AOEngine::setUpRenderPass() {
	std::array<vk::AttachmentDescription, 2> attachments;

	// Color attachment
	attachments[0] = vk::AttachmentDescription(
		vk::AttachmentDescriptionFlags(), this->swapchain->colorFormat, vk::SampleCountFlagBits::e1,
		vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, vk::AttachmentLoadOp::eDontCare,
		vk::AttachmentStoreOp::eDontCare, vk::ImageLayout::eUndefined, vk::ImageLayout::ePresentSrcKHR
	);

	// Depth attachment
	attachments[1] = vk::AttachmentDescription(
		vk::AttachmentDescriptionFlags(), this->device->depthFormat, vk::SampleCountFlagBits::e1,
		vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, vk::AttachmentLoadOp::eClear,
		vk::AttachmentStoreOp::eDontCare, vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal
	);

	vk::AttachmentReference colorReference(0, vk::ImageLayout::eColorAttachmentOptimal);
	vk::AttachmentReference depthReference(1, vk::ImageLayout::eDepthStencilAttachmentOptimal);

	vk::SubpassDescription subpassDescription(
		vk::SubpassDescriptionFlags(), vk::PipelineBindPoint::eGraphics,
		0, nullptr, 1, &colorReference, nullptr, &depthReference
	);

	// Subpass dependencies for layout transitions
	std::array<vk::SubpassDependency, 2> dependencies;

	dependencies[0] = vk::SubpassDependency(
		VK_SUBPASS_EXTERNAL, 0, vk::PipelineStageFlagBits::eBottomOfPipe,
		vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::AccessFlagBits::eMemoryRead,
		vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite,
		vk::DependencyFlagBits::eByRegion
	);

	dependencies[1] = vk::SubpassDependency(
		0, VK_SUBPASS_EXTERNAL, vk::PipelineStageFlagBits::eColorAttachmentOutput,
		vk::PipelineStageFlagBits::eBottomOfPipe, vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite,
		vk::AccessFlagBits::eMemoryRead,
		vk::DependencyFlagBits::eByRegion
	);

	// Create info
	vk::RenderPassCreateInfo renderPassInfo(
		vk::RenderPassCreateFlags(), static_cast<uint32_t>(attachments.size()),
		attachments.data(), 1, &subpassDescription, static_cast<uint32_t>(dependencies.size()),
		dependencies.data()
	);

	// Create render pass
	this->renderPass = this->device->logical.createRenderPass(renderPassInfo);
}

void ao::vulkan::AOEngine::setUpFrameBuffers() {
	std::array<vk::ImageView, 2> attachments;

	// Depth/Stencil attachment is the same for all frame buffers
	attachments[1] = std::get<2>(this->stencilBuffer);

	// Create info
	vk::FramebufferCreateInfo frameBufferCreateInfo(
		vk::FramebufferCreateFlags(), renderPass, static_cast<uint32_t>(attachments.size()), attachments.data(),
		static_cast<uint32_t>(this->_settings.window.width), static_cast<uint32_t>(this->_settings.window.height), 1
	);

	// Create frame buffers
	this->frameBuffers.resize(this->swapchain->buffers.size());
	for (uint32_t i = 0; i < frameBuffers.size(); i++) {
		attachments[0] = this->swapchain->buffers[i].second;

		this->frameBuffers[i] = this->device->logical.createFramebuffer(frameBufferCreateInfo);
	}
}

void ao::vulkan::AOEngine::recreateSwapChain() {
	// Ensure all operations on the device have been finished
	this->device->logical.waitIdle();

	// Init swap chain
	this->swapchain->init(this->_settings.window.width, this->_settings.window.height, this->_settings.window.vsync);

	// Destroy stencil buffer
	this->device->logical.destroyImage(std::get<0>(this->stencilBuffer));
	this->device->logical.destroyImageView(std::get<2>(this->stencilBuffer));
	this->device->logical.freeMemory(std::get<1>(this->stencilBuffer));

	// Create stencil buffer
	this->createStencilBuffer();

	// Destroy all frame buffers
	for (uint32_t i = 0; i < this->frameBuffers.size(); i++) {
		this->device->logical.destroyFramebuffer(this->frameBuffers[i]);
	}

	// Set-up frame buffers
	this->setUpFrameBuffers();

	// Free command buffers
	this->swapchain->freeCommandBuffers();

	// Re-create command buffers
	this->swapchain->createCommandBuffers();
	this->swapchain->initCommandBuffers(this->frameBuffers, this->renderPass, this->_settings.window);

	// Wait device idle
	this->device->logical.waitIdle();

	// TODO: Add a method onSwapChainResize
}

void ao::vulkan::AOEngine::prepareVulkan() {
	/*if (this->device.debugMarkers) {
	   // TODO
	}*/

	// Init surface
	this->initSurface(this->swapchain->surface);
	this->swapchain->initSurface();

	// Init command pool
	this->swapchain->initCommandPool();

	// Init swap chain
	this->swapchain->init(this->_settings.window.width, this->_settings.window.height, this->_settings.window.vsync);

	// Create command buffers
	this->swapchain->createCommandBuffers();

	// Create waiting fences
	this->createWaitingFences();

	// Create stencil buffer
	this->createStencilBuffer();

	// Set-up render pass
	this->setUpRenderPass();

	// Create pipeline cache
	this->pipelineCache = this->device->logical.createPipelineCache(vk::PipelineCacheCreateInfo());

	// Set-up frame buffer
	this->setUpFrameBuffers();

	// Init command buffers
	this->swapchain->initCommandBuffers(this->frameBuffers, this->renderPass, this->_settings.window);
}

void ao::vulkan::AOEngine::setWindowTitle(std::string title) {
	this->_settings.window.name = title;
}

ao::vulkan::EngineSettings ao::vulkan::AOEngine::settings() {
	return this->_settings;
}

void ao::vulkan::AOEngine::loop() {
	while (this->loopingCondition()) {
		if (!this->isIconified()) {
			// Execute plugins...
			this->onLoopIteration();

			// Render frame
			this->render();
		}
		else {
			this->waitMaximized();
		}
	}
}

void ao::vulkan::AOEngine::onLoopIteration() {
	// Execute plugins' onUpdate()
	this->pluginsMutex.lock();
	{
		for (size_t i = 0; i < this->plugins.size(); i++) {
			this->plugins[i]->onUpdate();
		}
	}
	this->pluginsMutex.unlock();
}

void ao::vulkan::AOEngine::render() {
	this->device->logical.waitIdle();

	// Prepare frame
	this->prepareFrame();

	// Edit submit info
	this->submitInfo.setCommandBufferCount(1);
	this->submitInfo.setPCommandBuffers(&this->swapchain->commandBuffers[this->frameBufferIndex]);

	// Submit to queue
	this->queue.submit(this->submitInfo, nullptr);

	// Submit frame
	this->submitFrame();

	this->device->logical.waitIdle();
}

void ao::vulkan::AOEngine::prepareFrame() {
	vk::Result result = this->swapchain->nextImage(this->semaphores.first, this->frameBufferIndex);

	// Check result
	if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR) {
		LOGGER << LogLevel::DEBUG << "Swap chain is no longer compatible, re-create it";

		this->recreateSwapChain();
		return;
	}
	ao::vulkan::utilities::vkAssert(result, "Fail to get next image from swap chain");
}

void ao::vulkan::AOEngine::submitFrame() {
	vk::Result result = this->swapchain->enqueueImage(this->queue, this->frameBufferIndex, this->semaphores.second);

	// Check result
	if (result == vk::Result::eErrorOutOfDateKHR) {
		LOGGER << LogLevel::DEBUG << "Swap chain is no longer compatible, re-create it";
		
		this->recreateSwapChain();
		return;
	}
	else if (result == vk::Result::eSuccess || result == vk::Result::eSuboptimalKHR) {
		this->queue.waitIdle();
	}
	ao::vulkan::utilities::vkAssert(result, "Fail to enqueue image");
}

std::vector<char const*> ao::vulkan::AOEngine::deviceExtensions() {
	return std::vector<char const*>();
}

vk::QueueFlags ao::vulkan::AOEngine::queueFlags() {
	return vk::QueueFlagBits::eGraphics | vk::QueueFlagBits::eCompute;
}

vk::CommandPoolCreateFlags ao::vulkan::AOEngine::commandPoolFlags() {
	return vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
}

size_t ao::vulkan::AOEngine::selectVkPhysicalDevice(std::vector<vk::PhysicalDevice>& devices) {
	return 0;    // First device
}

void ao::vulkan::AOEngine::freePlugins() {
	this->pluginsMutex.lock();
	{
		for (size_t i = 0; i < this->plugins.size(); i++) {
			delete this->plugins[i];
		}
		this->plugins.clear();
	}
	this->pluginsMutex.unlock();
}
