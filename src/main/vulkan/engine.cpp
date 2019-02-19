// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#include "engine.h"

ao::vulkan::Engine::Engine(std::shared_ptr<EngineSettings> settings) : settings_(settings), enforce_resize(false) {}

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

    // Free vulkan
    this->freeVulkan();
}

void ao::vulkan::Engine::initVulkan() {
    // Init volk
    ao::vulkan::utilities::vkAssert(volkInitialize(), "Fail to initialize vulkan loader");

    // Create instance
    this->instance = std::make_shared<vk::Instance>(utilities::createVkInstance(this->settings_, this->instanceExtensions()));
    volkLoadInstance(*this->instance);

    // Set-up debugging
    if (this->settings_->get(ao::vulkan::settings::ValidationLayers, std::make_optional(false))) {
        this->setUpDebugging();
    }

    // Get GPUs
    std::vector<vk::PhysicalDevice> devices = this->instance->enumeratePhysicalDevices();

    // Check count
    if (devices.empty()) {
        throw ao::core::Exception("Unable to find GPUs");
    }

    // Select a vk::PhysicalDevice & wrap it
    this->device = std::make_shared<ao::vulkan::Device>(this->selectVkPhysicalDevice(devices));

    LOGGER << ao::core::Logger::Level::info << fmt::format("Select physical device: {0}", this->device->physical.getProperties().deviceName);

    // Init logical device
    this->device->initLogicalDevice(this->deviceExtensions(), this->deviceFeatures(), this->requestQueues());

    // Create swapChain
    this->swapchain = std::make_shared<ao::vulkan::Swapchain>(this->instance, this->device);
}

void ao::vulkan::Engine::freeVulkan() {
    this->swapchain.reset();

    this->pipelines.clear();

    this->device->logical.destroyRenderPass(this->render_pass);

    this->semaphores.clear();

    this->device.reset();

    if (this->settings_->get(ao::vulkan::settings::ValidationLayers, std::make_optional(false))) {
        this->instance->destroyDebugUtilsMessengerEXT(this->debug_callBack);
    }

    this->instance->destroy();
}

void ao::vulkan::Engine::setUpDebugging() {
    this->debug_callBack = this->instance->createDebugUtilsMessengerEXT(
        vk::DebugUtilsMessengerCreateInfoEXT(vk::DebugUtilsMessengerCreateFlagsEXT(), this->validationLayersSeverity(),
                                             vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
                                                 vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation,
                                             ao::vulkan::Engine::DebugCallBack));
}

void ao::vulkan::Engine::recreateSwapChain() {
    // Ensure all operations on the device have been finished
    this->device->logical.waitIdle();

    // Destroy framebuffers
    this->swapchain->destroyFramebuffers();

    // Recreate swap chain
    this->swapchain->init(this->settings_->get<u64>(ao::vulkan::settings::WindowWidth), this->settings_->get<u64>(ao::vulkan::settings::WindowHeight),
                          this->settings_->get(ao::vulkan::settings::WindowVsync, std::make_optional(false)),
                          this->settings_->get(ao::vulkan::settings::StencilBuffer, std::make_optional(false)));

    // Create framebuffers
    this->swapchain->createFramebuffers(this->render_pass);

    // Call onSwapchainRecreation()
    this->onSwapchainRecreation();

    // Wait device idle
    this->device->logical.waitIdle();
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
    this->swapchain->setSurface(this->createSurface())->initSurface();

    // Create semaphores
    this->createSemaphores();

    // Init swap chain
    this->swapchain->init(this->settings_->get<u64>(ao::vulkan::settings::WindowWidth), this->settings_->get<u64>(ao::vulkan::settings::WindowHeight),
                          this->settings_->get(ao::vulkan::settings::WindowVsync, std::make_optional(false)),
                          this->settings_->get(ao::vulkan::settings::StencilBuffer, std::make_optional(false)));

    // Create secondary commands
    this->createSecondaryCommandBuffers();

    // Create render pass
    if (!(this->render_pass = this->createRenderPass())) {
        throw ao::core::Exception("Render pass isn't initialized");
    }

    // Create pipelines
    this->createPipelines();

    // Set-up vulkan buffers
    this->createVulkanBuffers();

    // Create framebuffers
    this->swapchain->createFramebuffers(this->render_pass);
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
    vk::Fence fence = this->swapchain->currentFence();
    vk::PipelineStageFlags pipeline_stage = vk::PipelineStageFlagBits::eColorAttachmentOutput;

    // Wait fence
    this->device->logical.waitForFences(fence, VK_TRUE, (std::numeric_limits<u64>::max)());

    // Prepare frame
    this->prepareFrame();

    // Call	beforeCommandBuffersUpdate()
    this->beforeCommandBuffersUpdate();

    // Update command buffers
    this->updateCommandBuffers();

    // Create submit info
    vk::SubmitInfo submitInfo(static_cast<u32>(this->semaphores["graphicQueue"].waits.size()),
                              this->semaphores["graphicQueue"].waits.empty() ? nullptr : this->semaphores["graphicQueue"].waits.data(),
                              &pipeline_stage, 1, &this->swapchain->currentCommand(),
                              static_cast<u32>(this->semaphores["graphicQueue"].signals.size()),
                              this->semaphores["graphicQueue"].signals.empty() ? nullptr : this->semaphores["graphicQueue"].signals.data());

    // Reset fence
    this->device->logical.resetFences(fence);

    // Submit command buffer
    this->device->queues->at(vk::to_string(vk::QueueFlagBits::eGraphics)).value.submit(submitInfo, fence);

    // Submit frame
    this->submitFrame();

    this->afterFrame();
}

void ao::vulkan::Engine::prepareFrame() {
    vk::Result result = this->swapchain->acquireNextImage(this->semaphores["acquireNextImage"].signals.front());

    // Check result
    if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR || this->enforce_resize) {
        LOGGER << ao::core::Logger::Level::warning << "Swap chain is no longer compatible, re-create it";

        this->enforce_resize = false;
        return this->recreateSwapChain();
    }
    ao::vulkan::utilities::vkAssert(result, "Fail to get next image from swap chain");
}

void ao::vulkan::Engine::submitFrame() {
    vk::Result result = this->swapchain->enqueueImage(this->semaphores["presentQueue"].waits);

    // Check result
    if (result == vk::Result::eErrorOutOfDateKHR || this->enforce_resize) {
        LOGGER << ao::core::Logger::Level::warning << "Swap chain is no longer compatible, re-create it";

        this->enforce_resize = false;
        return this->recreateSwapChain();
    }
    ao::vulkan::utilities::vkAssert(result, "Fail to enqueue image");
}

void ao::vulkan::Engine::updateCommandBuffers() {
    // Get current command buffer/frame
    vk::CommandBuffer command = this->swapchain->currentCommand();
    vk::Framebuffer frame = this->swapchain->currentFrame();

    // Create info
    vk::CommandBufferBeginInfo begin_info(vk::CommandBufferUsageFlagBits::eRenderPassContinue);

    std::vector<vk::ClearValue> clearValues(1);
    std::array<float, 4> color = {0.0f, 0.0f, 0.0f, 1.0f};
    clearValues[0].setColor(vk::ClearColorValue(color));
    if (this->settings_->get(ao::vulkan::settings::StencilBuffer, std::make_optional(false))) {
        clearValues.push_back(vk::ClearValue().setDepthStencil(vk::ClearDepthStencilValue(1)));
    }

    vk::RenderPassBeginInfo render_pass_info(this->render_pass, frame, vk::Rect2D().setExtent(this->swapchain->extent()),
                                             static_cast<u32>(clearValues.size()), clearValues.data());

    command.begin(&begin_info);
    command.beginRenderPass(render_pass_info, vk::SubpassContents::eSecondaryCommandBuffers);
    {
        // Create inheritance info for the secondary command buffers
        vk::CommandBufferInheritanceInfo inheritance(this->render_pass, 0, frame);

        // Execute secondary command buffers
        this->executeSecondaryCommandBuffers(inheritance, this->swapchain->currentFrameIndex(), command);
    }
    command.endRenderPass();
    command.end();
}

std::vector<char const*> ao::vulkan::Engine::deviceExtensions() const {
    return {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
}

std::vector<vk::PhysicalDeviceFeatures> ao::vulkan::Engine::deviceFeatures() const {
    return {};
}

std::vector<ao::vulkan::QueueRequest> ao::vulkan::Engine::requestQueues() const {
    return {ao::vulkan::QueueRequest(vk::QueueFlagBits::eGraphics)};
}

vk::DebugUtilsMessageSeverityFlagsEXT ao::vulkan::Engine::validationLayersSeverity() const {
    return vk::DebugUtilsMessageSeverityFlagBitsEXT::eError | vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
           vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo;
}

VKAPI_ATTR VkBool32 VKAPI_CALL ao::vulkan::Engine::DebugCallBack(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
                                                                 VkDebugUtilsMessageTypeFlagsEXT type,
                                                                 const VkDebugUtilsMessengerCallbackDataEXT* callback_data, void* user_data) {
    core::Logger LOGGER = core::Logger::GetInstance<ValidationLayers>();

    // Format message
    std::string message = fmt::format("[{}] {}", to_string(vk::DebugUtilsMessageTypeFlagsEXT(type)), callback_data->pMessage);

    switch (vk::DebugUtilsMessageSeverityFlagBitsEXT(severity)) {
        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eError:
            LOGGER << ao::core::Logger::Level::error << message;
            break;

        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo:
            LOGGER << ao::core::Logger::Level::info << message;
            break;

        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose:
            LOGGER << ao::core::Logger::Level::trace << message;
            break;

        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning:
            LOGGER << ao::core::Logger::Level::warning << message;
            break;

        default:
            throw ao::core::Exception(
                fmt::format("Unknown vk::DebugUtilsMessageSeverityFlagBitsEXT: {}", to_string(vk::DebugUtilsMessageSeverityFlagBitsEXT(severity))));
    }
    return VK_FALSE;
}

vk::PhysicalDevice ao::vulkan::Engine::selectVkPhysicalDevice(std::vector<vk::PhysicalDevice> const& devices) const {
    return devices.front();  // First device
}
