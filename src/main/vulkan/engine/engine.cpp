// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#include "engine.h"

ao::vulkan::Engine::Engine(std::shared_ptr<EngineSettings> settings) : settings_(settings) {}

ao::vulkan::Engine::~Engine() {
    this->freeVulkan();
}

void ao::vulkan::Engine::run() {
    // Init window
    this->initWindow();
    LOGGER << ao::core::Logger::Level::info
           << fmt::format("Init {0}x{1} window", this->settings_->get<u64>(ao::vulkan::settings::WindowWidth),
                          this->settings_->get<u64>(ao::vulkan::settings::WindowHeight));

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
    if (this->settings_->get(ao::vulkan::settings::ValidationLayers, std::make_optional<bool>(false))) {
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
    this->swapchain = std::make_shared<ao::vulkan::Swapchain>(this->instance, this->device);
}

void ao::vulkan::Engine::freeVulkan() {
    this->swapchain.reset();

    this->pipeline.reset();

    for (auto& pool : this->descriptorPools) {
        this->device->logical.destroyDescriptorPool(pool);
    }

    for (auto& layout : this->descriptorSetLayouts) {
        this->device->logical.destroyDescriptorSetLayout(layout);
    }

    this->device->logical.destroyRenderPass(this->renderPass);

    this->semaphores.clear();

    this->device.reset();

    if (this->settings_->get(ao::vulkan::settings::ValidationLayers, std::make_optional<bool>(false))) {
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

    VkDebugReportCallbackCreateInfoEXT create_info =
        vk::DebugReportCallbackCreateInfoEXT(this->debugReportFlags(), ao::vulkan::Engine::DebugReportCallBack);
    VkDebugReportCallbackEXT callback;

    // Create callback
    CreateDebugReportCallback(*this->instance, &create_info, nullptr, &callback);

    // Update real callback
    this->debug_callBack = vk::DebugReportCallbackEXT(callback);
}

void ao::vulkan::Engine::createRenderPass() {
    this->setUpRenderPass();

    // Check render pass
    if (!this->renderPass) {
        throw ao::core::Exception("Fail to create render pass");
    }
}

void ao::vulkan::Engine::recreateSwapChain() {
    // Ensure all operations on the device have been finished
    this->device->logical.waitIdle();

    // Destroy pipeline
    this->pipeline.reset();

    // Destroy render pass
    this->device->logical.destroyRenderPass(this->renderPass);

    // Destroy framebuffers
    this->swapchain->destroyFramebuffers();

    // Free command buffers
    this->swapchain->freeCommandBuffers();

    /* RE-CREATION PART */

    // Init swap chain
    this->swapchain->init(this->settings_->get<u64>(ao::vulkan::settings::WindowWidth), this->settings_->get<u64>(ao::vulkan::settings::WindowHeight),
                          this->settings_->get(ao::vulkan::settings::WindowVsync, std::make_optional<bool>(false)),
                          this->settings_->get(ao::vulkan::settings::StencilBuffer, std::make_optional<bool>(false)));

    // Create command buffers
    this->swapchain->createCommandBuffers();
    this->createSecondaryCommandBuffers();

    // Create render pass
    this->createRenderPass();

    // Create pipelines
    this->createPipelines();

    // Create framebuffers
    this->swapchain->createFramebuffers(this->renderPass);

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
    vk::Semaphore acquire = this->device->logical.createSemaphore(vk::SemaphoreCreateInfo());
    vk::Semaphore render = this->device->logical.createSemaphore(vk::SemaphoreCreateInfo());

    // Fill container
    this->semaphores["acquireNextImage"].signals.push_back(acquire);

    this->semaphores["graphicQueue"].waits.push_back(acquire);
    this->semaphores["graphicQueue"].signals.push_back(render);

    this->semaphores["presentQueue"].waits.push_back(render);
}

void ao::vulkan::Engine::prepareVulkan() {
    // Init surface
    this->initSurface(this->swapchain->surface);
    this->swapchain->initSurface();

    // Create semaphores
    this->createSemaphores();

    // Init swap chain
    this->swapchain->init(this->settings_->get<u64>(ao::vulkan::settings::WindowWidth), this->settings_->get<u64>(ao::vulkan::settings::WindowHeight),
                          this->settings_->get(ao::vulkan::settings::WindowVsync, std::make_optional<bool>(false)),
                          this->settings_->get(ao::vulkan::settings::StencilBuffer, std::make_optional<bool>(false)));
    this->swapchain->prepare();

    // Create command buffers
    this->swapchain->createCommandBuffers();
    this->createSecondaryCommandBuffers();

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

    // Create framebuffers
    this->swapchain->createFramebuffers(this->renderPass);
}

std::shared_ptr<ao::vulkan::EngineSettings> ao::vulkan::Engine::settings() const {
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
    vk::Fence& fence = this->swapchain->currentFence();

    // Wait fence
    this->device->logical.waitForFences(fence, VK_TRUE, (std::numeric_limits<u64>::max)());

    // Prepare frame
    this->prepareFrame();

    // Update uniform buffers
    this->updateUniformBuffers();

    // Update command buffers
    this->updateCommandBuffers();

    // Create submit info
    vk::SubmitInfo submitInfo(static_cast<u32>(this->semaphores["graphicQueue"].waits.size()),
                              this->semaphores["graphicQueue"].waits.empty() ? nullptr : this->semaphores["graphicQueue"].waits.data(),
                              &this->pipeline->submit_pipeline_stages, 1, &this->swapchain->commands["primary"].buffers[this->swapchain->frame_index],
                              static_cast<u32>(this->semaphores["graphicQueue"].signals.size()),
                              this->semaphores["graphicQueue"].signals.empty() ? nullptr : this->semaphores["graphicQueue"].signals.data());

    // Reset fence
    this->device->logical.resetFences(fence);

    // Submit command buffer
    this->device->queues[vk::QueueFlagBits::eGraphics].queue.submit(submitInfo, fence);

    // Submit frame
    this->submitFrame();
}

void ao::vulkan::Engine::prepareFrame() {
    vk::Result result = this->swapchain->nextImage(this->semaphores["acquireNextImage"].signals.front());

    // Check result
    if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR) {
        LOGGER << ao::core::Logger::Level::warning << "Swap chain is no longer compatible, re-create it";

        this->recreateSwapChain();
        return;
    }
    ao::vulkan::utilities::vkAssert(result, "Fail to get next image from swap chain");
}

void ao::vulkan::Engine::submitFrame() {
    vk::Result result = this->swapchain->enqueueImage(this->semaphores["presentQueue"].waits);

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
    vk::CommandBuffer& command = this->swapchain->commands["primary"].buffers[this->swapchain->frame_index];
    vk::Framebuffer& frame = this->swapchain->currentFrame();

    // Create info
    vk::CommandBufferBeginInfo begin_info(vk::CommandBufferUsageFlagBits::eRenderPassContinue);

    std::vector<vk::ClearValue> clearValues(1);
    std::array<float, 4> color = {0.0f, 0.0f, 0.0f, 1.0f};
    clearValues[0].setColor(vk::ClearColorValue(color));
    if (this->settings_->get(ao::vulkan::settings::StencilBuffer, std::make_optional(false))) {
        clearValues.push_back(vk::ClearValue().setDepthStencil(vk::ClearDepthStencilValue(1)));
    }

    vk::RenderPassBeginInfo render_pass_info(this->renderPass, frame, vk::Rect2D().setExtent(this->swapchain->current_extent),
                                             static_cast<u32>(clearValues.size()), clearValues.data());

    command.begin(&begin_info);
    command.beginRenderPass(render_pass_info, vk::SubpassContents::eSecondaryCommandBuffers);
    {
        // Create inheritance info for the secondary command buffers
        vk::CommandBufferInheritanceInfo inheritance(this->renderPass, 0, frame);

        // Execute secondary command buffers
        this->executeSecondaryCommandBuffers(inheritance, this->swapchain->frame_index, command);
    }
    command.endRenderPass();
    command.end();
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
    std::array<vk::DebugReportFlagBitsEXT, 5> all_flags = {
		vk::DebugReportFlagBitsEXT::eError, vk::DebugReportFlagBitsEXT::eWarning,
        vk::DebugReportFlagBitsEXT::eDebug, vk::DebugReportFlagBitsEXT::eInformation,
        vk::DebugReportFlagBitsEXT::ePerformanceWarning
	};
    // clang-format on

    // Find best flag
    vk::DebugReportFlagsEXT _flags(flags);
    std::optional<vk::DebugReportFlagBitsEXT> flag;
    for (auto& _flag : all_flags) {
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
