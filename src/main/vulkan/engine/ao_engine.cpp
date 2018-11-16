#include "ao_engine.h"

ao::vulkan::AOEngine::AOEngine(EngineSettings settings) {
	this->_settings = settings;

	// Resize pool
	this->commandBufferPool.resize(this->_settings.threadPoolSize);
	LOGGER << LogLevel::INFO << "Init a thread pool for command buffer processing with " << this->commandBufferPool.size() << " thread(s)";
}

ao::vulkan::AOEngine::~AOEngine() {
	// Execute plugins' beforeDestroy()
	this->pluginsMutex.lock();
	{
		for (auto& plugin : this->plugins) {
			plugin->beforeDestroy();
		}
	}
	this->pluginsMutex.unlock();

	// Kill thread pool
	this->commandBufferPool.stop();

	this->freeVulkan();
	this->freePlugins();
}

void ao::vulkan::AOEngine::run() {
	// Init window
	this->initWindow();
	LOGGER << LogLevel::INFO << "Init " << this->_settings.window.width << "x" << this->_settings.window.height << " window";

	// Init vulkan
	this->initVulkan();
	this->prepareVulkan();

	// Execute plugins' onInit()
	this->pluginsMutex.lock();
	{
		for (auto& plugin : this->plugins) {
			plugin->onInit();
		}
	}
	this->pluginsMutex.unlock();

	// Execute main loop
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

	// Get GPUs
	std::vector<vk::PhysicalDevice> devices = ao::vulkan::utilities::vkPhysicalDevices(this->instance);

	// Check count
	if (devices.empty()) {
		throw ao::core::Exception("Unable to find GPUs");
	}

	// Select a vk::PhysicalDevice & wrap it
	this->device = new Device(devices[this->selectVkPhysicalDevice(devices)]);

	LOGGER << LogLevel::INFO << "Select physical device: " << this->device->physical.getProperties().deviceName;

	// Init logical device
	this->device->initLogicalDevice(this->deviceExtensions(), this->deviceFeatures(), this->queueFlags(), this->commandPoolFlags());

	// Find suitable depth format
	this->device->depthFormat = ao::vulkan::utilities::getSupportedDepthFormat(this->device->physical);

	// Create swapChain
	this->swapchain = new SwapChain(&this->instance, this->device);
}

void ao::vulkan::AOEngine::freeVulkan() {
	delete this->swapchain;

	delete this->pipeline;

	for (auto& pool : this->descriptorPools) {
		this->device->logical.destroyDescriptorPool(pool);
	}
	this->descriptorPools.clear();
	for (auto& layout : this->descriptorSetLayouts) {
		this->device->logical.destroyDescriptorSetLayout(layout);
	}
	this->descriptorSetLayouts.clear();
	
	this->device->logical.destroyRenderPass(this->renderPass);

	for (auto& frameBuffer : this->frameBuffers) {
		this->device->logical.destroyFramebuffer(frameBuffer);
	}
	this->frameBuffers.clear();
	
	this->device->logical.destroyImageView(std::get<2>(this->stencilBuffer));
	this->device->logical.destroyImage(std::get<0>(this->stencilBuffer));
	this->device->logical.freeMemory(std::get<1>(this->stencilBuffer));

	this->semaphores.clear();

	for (auto& fence : this->waitingFences) {
		this->device->logical.destroyFence(fence);
	}
	this->waitingFences.clear();

	delete this->device;
	this->instance.destroy();
}

void ao::vulkan::AOEngine::createWaitingFences() {
	// Resize vector
	this->waitingFences.resize(this->swapchain->buffers.size());
	
	// Create fences
	for (auto& fence : this->waitingFences) {
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

void ao::vulkan::AOEngine::createRenderPass() {
	this->setUpRenderPass();

	// Check render pass
	if (!this->renderPass) {
		throw ao::core::Exception("Fail to create render pass");
	}
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

	// Destroy pipeline
	delete this->pipeline;

	// Destroy render pass
	this->device->logical.destroyRenderPass(this->renderPass);

	// Destroy frame buffers
	for (auto& frameBuffer : this->frameBuffers) {
		this->device->logical.destroyFramebuffer(frameBuffer);
	}
	this->frameBuffers.clear();

	// Destroy stencil buffer
	this->device->logical.destroyImageView(std::get<2>(this->stencilBuffer));
	this->device->logical.destroyImage(std::get<0>(this->stencilBuffer));
	this->device->logical.freeMemory(std::get<1>(this->stencilBuffer));

	// Free command buffers
	this->swapchain->freeCommandBuffers();

	/* RE-CREATION PART */

	// Init swap chain
	this->swapchain->init(this->_settings.window.width, this->_settings.window.height, this->_settings.window.vsync);

	// Create command buffers
	this->swapchain->createCommandBuffers();
	this->createSecondaryCommandBuffers();

	// Create stencil buffer
	this->createStencilBuffer();

	// Create render pass
	this->createRenderPass();

	// Create pipelines
	this->createPipelines();

	// Set-up frame buffers
	this->setUpFrameBuffers();

	// Wait device idle
	this->device->logical.waitIdle();
}

void ao::vulkan::AOEngine::createPipelines() {
	// Create pipeline
	this->pipeline = new ao::vulkan::Pipeline(this->device);

	// Create pipeline cache
	this->pipeline->cache = this->device->logical.createPipelineCache(vk::PipelineCacheCreateInfo());

	// Create layouts
	this->createPipelineLayouts();

	// Set-up pipelines
	this->setUpPipelines();

	// Check pipelines
	for (auto& pipeline : this->pipeline->pipelines) {
		if (!pipeline) {
			throw ao::core::Exception("Fail to create pipeline");
		}
	}
	if (this->pipeline->pipelines.empty()) {
		LOGGER << LogLevel::WARN << "Pipeline vector is empty";
	}
}

void ao::vulkan::AOEngine::createSemaphores() {
	this->semaphores = SemaphoreContainer(this->device);

	// Create semaphores
	vk::Semaphore renderSem = this->device->logical.createSemaphore(vk::SemaphoreCreateInfo());
	vk::Semaphore presentSem = this->device->logical.createSemaphore(vk::SemaphoreCreateInfo());

	// Create container
	this->semaphores[std::string("graphics")].waits.push_back(presentSem);
	this->semaphores[std::string("graphics")].signals.push_back(renderSem);

	this->semaphores[std::string("present")].waits.push_back(renderSem);
	this->semaphores[std::string("present")].signals.push_back(presentSem);
}

void ao::vulkan::AOEngine::prepareVulkan() {
	// Init surface
	this->initSurface(this->swapchain->surface);
	this->swapchain->initSurface();

	// Create semaphores
	this->createSemaphores();

	// Init command pool
	this->swapchain->initCommandPool();

	// Init swap chain
	this->swapchain->init(this->_settings.window.width, this->_settings.window.height, this->_settings.window.vsync);

	// Create command buffers
	this->swapchain->createCommandBuffers();
	this->createSecondaryCommandBuffers();

	// Create waiting fences
	this->createWaitingFences();

	// Create stencil buffer
	this->createStencilBuffer();

	// Create render pass
	this->createRenderPass();

	// Create descriptor set layouts
	this->createDescriptorSetLayouts();

    // Create pipelines
	this->createPipelines();

	// Set-up vulkan buffers
	this->setUpVulkanBuffers();

	// Create descriptor pools & sets
	this->createDescriptorPools();
	this->createDescriptorSets();

	// Set-up frame buffer
	this->setUpFrameBuffers();
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
			// Render frame
			this->render();
		}
		else {
			this->waitMaximized();
		}
	}
}

void ao::vulkan::AOEngine::afterFrameSubmitted() {
	// Execute plugins' onUpdate()
	this->pluginsMutex.lock();
	{
		for (auto& plugin : this->plugins) {
			plugin->onUpdate();
		}
	}
	this->pluginsMutex.unlock();
}

void ao::vulkan::AOEngine::render() {
	// Wait fence
	auto MAX_64 = std::numeric_limits<uint64_t>::max;
	this->device->logical.waitForFences(this->waitingFences[this->frameBufferIndex], VK_TRUE, MAX_64());

	// Prepare frame
	this->prepareFrame();

	// TODO: Compute stuff

	// Update command buffers
	this->updateCommandBuffers();

	// Update uniform buffers
	this->updateUniformBuffers();

	// Create submit info
	vk::SubmitInfo submitInfo(
		static_cast<uint32_t>(this->semaphores[std::string("graphics")].waits.size()),
		this->semaphores[std::string("graphics")].waits.empty() ? nullptr : this->semaphores[std::string("graphics")].waits.data(),
		&this->pipeline->submitPipelineStages,
		1, &this->swapchain->primaryCommandBuffers[this->frameBufferIndex],
		static_cast<uint32_t>(this->semaphores[std::string("graphics")].signals.size()),
		this->semaphores[std::string("graphics")].signals.empty() ? nullptr : this->semaphores[std::string("graphics")].signals.data()
	);

	// Reset fence
	this->device->logical.resetFences(this->waitingFences[this->frameBufferIndex]);

	// Submit command buffer
	this->device->queues[vk::QueueFlagBits::eGraphics].queue.submit(submitInfo, this->waitingFences[this->frameBufferIndex]);

	// Submit frame
	this->submitFrame();

	// Execute plugins...
	this->afterFrameSubmitted();
}

void ao::vulkan::AOEngine::prepareFrame() {
	vk::Result result = this->swapchain->nextImage(this->semaphores[std::string("present")].signals.front(), this->frameBufferIndex);

	// Check result
	if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR) {
		LOGGER << LogLevel::WARN << "Swap chain is no longer compatible, re-create it";

		this->recreateSwapChain();
		return;
	}
	ao::vulkan::utilities::vkAssert(result, "Fail to get next image from swap chain");
}

void ao::vulkan::AOEngine::submitFrame() {
	vk::Result result = this->swapchain->enqueueImage(this->frameBufferIndex, this->semaphores[std::string("present")].waits);

	// Check result
	if (result == vk::Result::eErrorOutOfDateKHR) {
		LOGGER << LogLevel::WARN << "Swap chain is no longer compatible, re-create it";
		
		this->recreateSwapChain();
		return;
	}
	ao::vulkan::utilities::vkAssert(result, "Fail to enqueue image");
}

void ao::vulkan::AOEngine::updateCommandBuffers() {
	// Get current command buffer/frame
	vk::CommandBuffer& currentCommand = this->swapchain->primaryCommandBuffers[this->frameBufferIndex];
	vk::Framebuffer& currentFrame = this->frameBuffers[this->frameBufferIndex];

	// Create info
	vk::CommandBufferBeginInfo beginInfo(vk::CommandBufferUsageFlagBits::eRenderPassContinue);

	vk::RenderPassBeginInfo renderPassInfo(
		this->renderPass, currentFrame, this->swapchain->commandBufferHelpers.second,
		static_cast<uint32_t>(this->swapchain->commandBufferHelpers.first.size()),
		this->swapchain->commandBufferHelpers.first.data()
	);

	currentCommand.begin(&beginInfo);
	{
		currentCommand.beginRenderPass(renderPassInfo, vk::SubpassContents::eSecondaryCommandBuffers);

	    // Create inheritance info for the secondary command buffers
		vk::CommandBufferInheritanceInfo inheritanceInfo(this->renderPass, 0, currentFrame);

		std::vector<vk::CommandBuffer> secondaryCommands;
		auto& helpers = this->swapchain->commandBufferHelpers;
		std::vector<std::future<vk::CommandBuffer>> futures;

		// Get drawing functions (TODO: Plugins can draw ???)
		std::vector<ao::vulkan::DrawInCommandBuffer> functions = this->updateSecondaryCommandBuffers();

		// Execute drawing functions
		int index = this->frameBufferIndex;
		for (auto& function : functions) {
			futures.push_back(this->commandBufferPool.push([&](int id) {
				return function(index, inheritanceInfo, helpers);
			}));
		}

		// Wait execution & add command buffer
		for (auto& future : futures) {
			secondaryCommands.push_back(future.get());
		}

		// Pass commands to current command buffer
		currentCommand.executeCommands(secondaryCommands);
		currentCommand.endRenderPass();
	}
	currentCommand.end();
}

std::vector<char const*> ao::vulkan::AOEngine::deviceExtensions() {
	return std::vector<char const*>();
}

std::vector<vk::PhysicalDeviceFeatures> ao::vulkan::AOEngine::deviceFeatures() {
	return std::vector<vk::PhysicalDeviceFeatures>();
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
