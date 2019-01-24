// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#include "engine.h"

ao::vulkan::Engine::Engine(EngineSettings const& settings) : settings_(settings), thread_pool(settings.core.threadPoolSize) {
    LOGGER << ao::core::Logger::Level::info
           << fmt::format("Init a thread pool for command buffer processing with {0} thread{1}", this->thread_pool.size(),
                          this->thread_pool.size() > 1 ? "s" : "");
}

ao::vulkan::Engine::~Engine() {
    // Kill thread pool
    this->thread_pool.kill();

    this->freeVulkan();
}

void ao::vulkan::Engine::run() {
    // Init window
    this->initWindow();
    LOGGER << ao::core::Logger::Level::info << fmt::format("Init {0}x{1} window", this->settings_.window.width, this->settings_.window.height);

    // Init vulkan
    this->initVulkan();
    this->prepareVulkan();

    // Execute main loop
    this->loop();
}

void ao::vulkan::Engine::initVulkan() {
    // Create instance
    this->instance = std::make_shared<vk::Instance>(utilities::createVkInstance(this->settings_, this->instanceExtensions()));

    // Set-up debugging
    if (this->settings_.core.validationLayers) {
        this->setUpDebugging();
    }

    // Get GPUs
    std::vector<vk::PhysicalDevice> devices = ao::vulkan::utilities::vkPhysicalDevices(*this->instance);

    // Check count
    if (devices.empty()) {
        throw ao::core::Exception("Unable to find GPUs");
    }

    // Select a vk::PhysicalDevice & wrap it
    this->device = std::make_shared<ao::vulkan::Device>(devices[this->selectVkPhysicalDevice(devices)]);

    LOGGER << ao::core::Logger::Level::info << fmt::format("Select physical device: {0}", this->device->physical.getProperties().deviceName);

    // Init logical device
    this->device->initLogicalDevice(this->deviceExtensions(), this->deviceFeatures(), this->queueFlags(), this->commandPoolFlags());

    // Find suitable depth format
    this->device->depth_format = ao::vulkan::utilities::getSupportedDepthFormat(this->device->physical);

    // Create swapChain
    this->swapchain = std::make_shared<ao::vulkan::SwapChain>(this->instance, this->device);
}

void ao::vulkan::Engine::freeVulkan() {
    this->swapchain.reset();

    this->pipeline.reset();

    for (auto& pool : this->descriptorPools) {
        this->device->logical.destroyDescriptorPool(pool);
    }
    this->descriptorPools.clear();
    for (auto& layout : this->descriptorSetLayouts) {
        this->device->logical.destroyDescriptorSetLayout(layout);
    }
    this->descriptorSetLayouts.clear();

    this->device->logical.destroyRenderPass(this->renderPass);

    for (auto& frameBuffer : this->frames) {
        this->device->logical.destroyFramebuffer(frameBuffer);
    }
    this->frames.clear();

    this->device->logical.destroyImageView(std::get<2>(this->stencil_buffer));
    this->device->logical.destroyImage(std::get<0>(this->stencil_buffer));
    this->device->logical.freeMemory(std::get<1>(this->stencil_buffer));

    this->semaphores.clear();

    for (auto& fence : this->waiting_fences) {
        this->device->logical.destroyFence(fence);
    }
    this->waiting_fences.clear();

    this->device.reset();

    if (this->settings_.core.validationLayers) {
        PFN_vkDestroyDebugReportCallbackEXT DestroyDebugReportCallback =
            reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(vkGetInstanceProcAddr(*this->instance, "vkDestroyDebugReportCallbackEXT"));

        // Check function
        if (DestroyDebugReportCallback == nullptr) {
            throw ao::core::Exception("vkDestroyDebugReportCallbackEXT is null, fail to destroy callback");
        }

        DestroyDebugReportCallback(*this->instance, this->debug_callBack, nullptr);
    }

    this->instance->destroy();
}

void ao::vulkan::Engine::setUpDebugging() {
    PFN_vkCreateDebugReportCallbackEXT CreateDebugReportCallback =
        reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(this->instance->getProcAddr("vkCreateDebugReportCallbackEXT"));

    // Check function
    if (CreateDebugReportCallback == nullptr) {
        this->LOGGER << ao::core::Logger::Level::warning << "Fail to retrieve function: vkCreateDebugReportCallbackEXT, cancel debug callback set-up";
        return;
    }

    VkDebugReportCallbackCreateInfoEXT createInfo =
        vk::DebugReportCallbackCreateInfoEXT(this->debugReportFlags(), ao::vulkan::Engine::DebugReportCallBack);
    VkDebugReportCallbackEXT callback;

    // Create callback
    CreateDebugReportCallback(*this->instance, &createInfo, nullptr, &callback);

    // Update real callback
    this->debug_callBack = vk::DebugReportCallbackEXT(callback);
}

void ao::vulkan::Engine::createWaitingFences() {
    // Resize vector
    this->waiting_fences.resize(this->swapchain->buffers.size());

    // Create fences
    for (auto& fence : this->waiting_fences) {
        fence = this->device->logical.createFence(vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled));
    }
}

void ao::vulkan::Engine::createStencilBuffer() {
    // Create info
    vk::ImageCreateInfo imageInfo(vk::ImageCreateFlags(), vk::ImageType::e2D, this->device->depth_format,
                                  vk::Extent3D(static_cast<u32>(this->settings_.window.width), static_cast<u32>(this->settings_.window.height), 1), 1,
                                  1, vk::SampleCountFlagBits::e1, vk::ImageTiling::eOptimal,
                                  vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eTransferSrc);

    vk::MemoryAllocateInfo allocInfo;

    vk::ImageViewCreateInfo depthStencilView(
        vk::ImageViewCreateFlags(), vk::Image(), vk::ImageViewType::e2D, this->device->depth_format, vk::ComponentMapping(),
        vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil, 0, 1, 0, 1));

    // Create image
    std::get<0>(this->stencil_buffer) = this->device->logical.createImage(imageInfo);

    // Get memory requirements
    vk::MemoryRequirements memReqs = this->device->logical.getImageMemoryRequirements(std::get<0>(this->stencil_buffer));
    allocInfo.setAllocationSize(memReqs.size);
    allocInfo.setMemoryTypeIndex(this->device->memoryType(memReqs.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal));

    // Allocate memory
    std::get<1>(this->stencil_buffer) = this->device->logical.allocateMemory(allocInfo);
    this->device->logical.bindImageMemory(std::get<0>(this->stencil_buffer), std::get<1>(this->stencil_buffer), 0);

    // Create image view
    depthStencilView.setImage(std::get<0>(this->stencil_buffer));
    std::get<2>(this->stencil_buffer) = this->device->logical.createImageView(depthStencilView);
}

void ao::vulkan::Engine::createRenderPass() {
    this->setUpRenderPass();

    // Check render pass
    if (!this->renderPass) {
        throw ao::core::Exception("Fail to create render pass");
    }
}

void ao::vulkan::Engine::setUpFrameBuffers() {
    std::array<vk::ImageView, 2> attachments;

    // Depth/Stencil attachment is the same for all frame buffers
    attachments[1] = std::get<2>(this->stencil_buffer);

    // Create info
    vk::FramebufferCreateInfo frameBufferCreateInfo(vk::FramebufferCreateFlags(), renderPass, static_cast<u32>(attachments.size()),
                                                    attachments.data(), static_cast<u32>(this->settings_.window.width),
                                                    static_cast<u32>(this->settings_.window.height), 1);

    // Create frame buffers
    this->frames.resize(this->swapchain->buffers.size());
    for (u32 i = 0; i < frames.size(); i++) {
        attachments[0] = this->swapchain->buffers[i].second;

        this->frames[i] = this->device->logical.createFramebuffer(frameBufferCreateInfo);
    }
}

void ao::vulkan::Engine::recreateSwapChain() {
    // Ensure all operations on the device have been finished
    this->device->logical.waitIdle();

    // Destroy pipeline
    this->pipeline.reset();

    // Destroy render pass
    this->device->logical.destroyRenderPass(this->renderPass);

    // Destroy frame buffers
    for (auto& frameBuffer : this->frames) {
        this->device->logical.destroyFramebuffer(frameBuffer);
    }
    this->frames.clear();

    // Destroy stencil buffer
    this->device->logical.destroyImageView(std::get<2>(this->stencil_buffer));
    this->device->logical.destroyImage(std::get<0>(this->stencil_buffer));
    this->device->logical.freeMemory(std::get<1>(this->stencil_buffer));

    // Free command buffers
    this->swapchain->freeCommandBuffers();

    /* RE-CREATION PART */

    // Init swap chain
    this->swapchain->init(this->settings_.window.width, this->settings_.window.height, this->settings_.window.vsync);

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

void ao::vulkan::Engine::createPipelines() {
    // Create pipeline
    this->pipeline = std::make_shared<ao::vulkan::Pipeline>(this->device);

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
        LOGGER << ao::core::Logger::Level::warning << "Pipeline vector is empty";
    }
}

void ao::vulkan::Engine::createSemaphores() {
    this->semaphores = SemaphoreContainer(this->device);

    // Create semaphores
    vk::Semaphore acquireSem = this->device->logical.createSemaphore(vk::SemaphoreCreateInfo());
    vk::Semaphore renderSem = this->device->logical.createSemaphore(vk::SemaphoreCreateInfo());

    // Fill container
    this->semaphores["acquireNextImage"].signals.push_back(acquireSem);

    this->semaphores["graphicQueue"].waits.push_back(acquireSem);
    this->semaphores["graphicQueue"].signals.push_back(renderSem);

    this->semaphores["presentQueue"].waits.push_back(renderSem);
}

void ao::vulkan::Engine::prepareVulkan() {
    // Init surface
    this->initSurface(this->swapchain->surface);
    this->swapchain->initSurface();

    // Create semaphores
    this->createSemaphores();

    // Init command pool
    this->swapchain->initCommandPool();

    // Init swap chain
    this->swapchain->init(this->settings_.window.width, this->settings_.window.height, this->settings_.window.vsync);

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

ao::vulkan::EngineSettings const& ao::vulkan::Engine::settings() const {
    return this->settings_;
}

void ao::vulkan::Engine::loop() {
    while (this->loopingCondition()) {
        if (!this->isIconified()) {
            // Render frame
            this->render();
        } else {
            this->waitMaximized();
        }
    }
}

void ao::vulkan::Engine::render() {
    // Wait fence
    this->device->logical.waitForFences(this->waiting_fences[this->frameBufferIndex], VK_TRUE, (std::numeric_limits<u64>::max)());

    // Prepare frame
    this->prepareFrame();

    // Update uniform buffers
    this->updateUniformBuffers();

    // Update command buffers
    this->updateCommandBuffers();

    // Create submit info
    vk::SubmitInfo submitInfo(static_cast<u32>(this->semaphores["graphicQueue"].waits.size()),
                              this->semaphores["graphicQueue"].waits.empty() ? nullptr : this->semaphores["graphicQueue"].waits.data(),
                              &this->pipeline->submit_pipeline_stages, 1, &this->swapchain->commands["primary"].buffers[this->frameBufferIndex],
                              static_cast<u32>(this->semaphores["graphicQueue"].signals.size()),
                              this->semaphores["graphicQueue"].signals.empty() ? nullptr : this->semaphores["graphicQueue"].signals.data());

    // Reset fence
    this->device->logical.resetFences(this->waiting_fences[this->frameBufferIndex]);

    // Submit command buffer
    this->device->queues[vk::QueueFlagBits::eGraphics].queue.submit(submitInfo, this->waiting_fences[this->frameBufferIndex]);

    // Submit frame
    this->submitFrame();
}

void ao::vulkan::Engine::prepareFrame() {
    vk::Result result = this->swapchain->nextImage(this->semaphores["acquireNextImage"].signals.front(), this->frameBufferIndex);

    // Check result
    if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR) {
        LOGGER << ao::core::Logger::Level::warning << "Swap chain is no longer compatible, re-create it";

        this->recreateSwapChain();
        return;
    }
    ao::vulkan::utilities::vkAssert(result, "Fail to get next image from swap chain");
}

void ao::vulkan::Engine::submitFrame() {
    vk::Result result = this->swapchain->enqueueImage(this->frameBufferIndex, this->semaphores["presentQueue"].waits);

    // Check result
    if (result == vk::Result::eErrorOutOfDateKHR) {
        LOGGER << ao::core::Logger::Level::warning << "Swap chain is no longer compatible, re-create it";

        this->recreateSwapChain();
        return;
    }
    ao::vulkan::utilities::vkAssert(result, "Fail to enqueue image");
}

void ao::vulkan::Engine::updateCommandBuffers() {
    // Get current command buffer/frame
    vk::CommandBuffer& currentCommand = this->swapchain->commands["primary"].buffers[this->frameBufferIndex];
    vk::Framebuffer& currentFrame = this->frames[this->frameBufferIndex];
    auto& helpers = this->swapchain->command_helpers;
    int index = this->frameBufferIndex;

    // Prepare sync objects
    std::atomic_size_t completed = 0;
    std::condition_variable task_cv;
    std::mutex cmdMutex;

    // Create info
    vk::CommandBufferBeginInfo beginInfo(vk::CommandBufferUsageFlagBits::eRenderPassContinue);

    vk::RenderPassBeginInfo renderPassInfo(this->renderPass, currentFrame, this->swapchain->command_helpers.second,
                                           static_cast<u32>(this->swapchain->command_helpers.first.size()),
                                           this->swapchain->command_helpers.first.data());

    currentCommand.begin(&beginInfo);
    currentCommand.beginRenderPass(renderPassInfo, vk::SubpassContents::eSecondaryCommandBuffers);
    {
        // Create inheritance info for the secondary command buffers
        vk::CommandBufferInheritanceInfo inheritanceInfo(this->renderPass, 0, currentFrame);

        // Get functions
        std::vector<ao::vulkan::DrawInCommandBuffer> functions = this->updateSecondaryCommandBuffers();

        // Execute drawing functions
        for (auto& function : functions) {
            // TODO: futures.push_back(this->thread_pool.push([&](int id) { return function(index, inheritanceInfo, helpers); }));
            this->thread_pool.enqueue<bool>([function, index, &inheritanceInfo, &helpers, &cmdMutex, &currentCommand, &completed, &task_cv]() {
                auto& cmd = function(index, inheritanceInfo, helpers);

                // Execute in primary command
                cmdMutex.lock();
                currentCommand.executeCommands(cmd);
                cmdMutex.unlock();

                // Notify task end
                completed++;
                task_cv.notify_one();

                return std::make_shared<bool>(false);
            });
        }

        // Wait execution
        std::mutex cv_m;
        std::unique_lock<std::mutex> lock(cv_m);
        task_cv.wait(lock, [&]() { return completed == functions.size(); });
    }
    currentCommand.endRenderPass();
    currentCommand.end();
}

std::vector<char const*> ao::vulkan::Engine::deviceExtensions() const {
    return std::vector<char const*>();
}

std::vector<vk::PhysicalDeviceFeatures> ao::vulkan::Engine::deviceFeatures() const {
    return std::vector<vk::PhysicalDeviceFeatures>();
}

vk::QueueFlags ao::vulkan::Engine::queueFlags() const {
    return vk::QueueFlagBits::eGraphics | vk::QueueFlagBits::eCompute;
}

vk::CommandPoolCreateFlags ao::vulkan::Engine::commandPoolFlags() const {
    return vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
}

vk::DebugReportFlagsEXT ao::vulkan::Engine::debugReportFlags() const {
    return vk::DebugReportFlagBitsEXT::eError | vk::DebugReportFlagBitsEXT::eWarning | vk::DebugReportFlagBitsEXT::eInformation |
           vk::DebugReportFlagBitsEXT::eDebug | vk::DebugReportFlagBitsEXT::ePerformanceWarning;
}

VKAPI_ATTR VkBool32 VKAPI_CALL ao::vulkan::Engine::DebugReportCallBack(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT type, u64 srcObject,
                                                                       size_t location, s32 msgCode, const char* pLayerPrefix, const char* message,
                                                                       void* pUserData) {
    ao::core::Logger LOGGER = core::Logger::GetInstance<ao::vulkan::Engine>();
    // clang-format off
    std::array<vk::DebugReportFlagBitsEXT, 5> Allflags = {
		vk::DebugReportFlagBitsEXT::eError, vk::DebugReportFlagBitsEXT::eWarning,
        vk::DebugReportFlagBitsEXT::eDebug, vk::DebugReportFlagBitsEXT::eInformation,
        vk::DebugReportFlagBitsEXT::ePerformanceWarning
	};
    // clang-format on

    // Find best flag
    vk::DebugReportFlagsEXT _flags(flags);
    std::optional<vk::DebugReportFlagBitsEXT> flag;
    for (auto& _flag : Allflags) {
        if ((_flags & _flag) && (!flag || flag.value() < _flag)) {
            flag = _flag;
        }
    }

    // Check flag
    if (!flag) {
        throw ao::core::Exception("Fail to find best vk::DebugReportFlagBitsEXT");
    }

    // Log
    vk::DebugReportObjectTypeEXT _type = vk::DebugReportObjectTypeEXT(type);
    switch (flag.value()) {
        case vk::DebugReportFlagBitsEXT::eInformation:
            LOGGER << ao::core::Logger::Level::trace << fmt::format("[{0}] [{1}] {2}", to_string(flag.value()), to_string(_type), message);
            break;

        case vk::DebugReportFlagBitsEXT::eWarning:
        case vk::DebugReportFlagBitsEXT::ePerformanceWarning:
            LOGGER << ao::core::Logger::Level::warning << fmt::format("[{0}] [{1}] {2}", to_string(flag.value()), to_string(_type), message);
            break;

        case vk::DebugReportFlagBitsEXT::eError:
            LOGGER << ao::core::Logger::Level::fatal << fmt::format("[{0}] [{1}] {2}", to_string(flag.value()), to_string(_type), message);
            return VK_TRUE;

        case vk::DebugReportFlagBitsEXT::eDebug:
            LOGGER << ao::core::Logger::Level::debug << fmt::format("[{0}] [{1}] {2}", to_string(flag.value()), to_string(_type), message);
            break;

        default:
            throw ao::core::Exception("Unknown vk::DebugReportFlagBitsEXT");
    }

    return VK_FALSE;  // Avoid to abort
}

size_t ao::vulkan::Engine::selectVkPhysicalDevice(std::vector<vk::PhysicalDevice> const& devices) const {
    return 0;  // First device
}
