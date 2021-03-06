// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#include "engine.h"

ao::vulkan::Engine::Engine(std::shared_ptr<EngineSettings> settings) : settings_(settings), enforce_resize(false), current_frame(0) {}

void ao::vulkan::Engine::run() {
    // Init window
    this->initWindow();
    LOG_MSG(info) << fmt::format("Init {0}x{1} window", this->settings_->get<u32>(ao::vulkan::settings::SurfaceWidth),
                                 this->settings_->get<u32>(ao::vulkan::settings::SurfaceHeight));

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
    this->instance = std::make_shared<vk::Instance>(utilities::createInstance(this->settings_, this->instanceExtensions()));
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

    LOG_MSG(info) << fmt::format("Select physical device: {0}", this->device->physical().getProperties().deviceName);

    // Init logical device
    this->device->initLogicalDevice(this->deviceExtensions(), this->deviceFeatures(), this->requestQueues());

    // Create swapChain
    this->swapchain = std::make_shared<ao::vulkan::Swapchain>(this->instance, this->device);
}

void ao::vulkan::Engine::freeVulkan() {
    this->swapchain.reset();

    this->pipelines.clear();

    this->device->logical()->destroyRenderPass(this->render_pass);

    for (auto& fence : this->fences) {
        this->device->logical()->destroyFence(fence);
    }

    this->semaphores->clear();

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
    this->device->logical()->waitIdle();

    // Destroy framebuffers
    this->swapchain->destroyFramebuffers();

    // Recreate swap chain
    this->swapchain->init(this->settings_->get<u32>(ao::vulkan::settings::SurfaceWidth),
                          this->settings_->get<u32>(ao::vulkan::settings::SurfaceHeight),
                          this->settings_->get(ao::vulkan::settings::WindowVsync, std::make_optional(false)),
                          this->settings_->get(ao::vulkan::settings::StencilBuffer, std::make_optional(false)));

    // Create framebuffers
    this->swapchain->createFramebuffers(this->render_pass);

    // Call onSwapchainRecreation()
    this->onSwapchainRecreation();

    // Wait device idle
    this->device->logical()->waitIdle();
}

void ao::vulkan::Engine::createSemaphores() {
    this->semaphores = std::make_unique<ao::vulkan::SemaphoreContainer>(this->device->logical());

    // Create semaphores
    this->semaphores->resize(3 * this->swapchain->size());
    for (size_t i = 0; i < this->swapchain->size(); i++) {
        vk::Semaphore acquire = this->device->logical()->createSemaphore(vk::SemaphoreCreateInfo());
        vk::Semaphore render = this->device->logical()->createSemaphore(vk::SemaphoreCreateInfo());

        // Fill container
        this->semaphores->at(ao::vulkan::semaphore::AcquireImage * this->swapchain->size() + i).signals.push_back(acquire);

        this->semaphores->at(ao::vulkan::semaphore::GraphicProcess * this->swapchain->size() + i).waits.push_back(acquire);
        this->semaphores->at(ao::vulkan::semaphore::GraphicProcess * this->swapchain->size() + i).signals.push_back(render);

        this->semaphores->at(ao::vulkan::semaphore::PresentImage * this->swapchain->size() + i).waits.push_back(render);
    }
}

void ao::vulkan::Engine::createFences() {
    this->fences.resize(this->swapchain->size());

    for (auto& fence : this->fences) {
        fence = this->device->logical()->createFence(vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled));
    }
}

void ao::vulkan::Engine::prepareVulkan() {
    // Init surface
    this->swapchain->setSurface(this->createSurface())->initSurface();

    // Init swap chain
    this->swapchain->init(this->settings_->get<u32>(ao::vulkan::settings::SurfaceWidth),
                          this->settings_->get<u32>(ao::vulkan::settings::SurfaceHeight),
                          this->settings_->get(ao::vulkan::settings::WindowVsync, std::make_optional(false)),
                          this->settings_->get(ao::vulkan::settings::StencilBuffer, std::make_optional(false)));

    // Create semaphores
    this->createSemaphores();

    // Create fences
    this->createFences();

    // Create render pass
    if (!(this->render_pass = this->createRenderPass())) {
        throw ao::core::Exception("Render pass isn't initialized");
    }

    // Create vulkan objects
    this->createVulkanObjects();

    // Create framebuffers
    this->swapchain->createFramebuffers(this->render_pass);
}

void ao::vulkan::Engine::loop() {
    while (this->loopingCondition()) {
        if (!this->isIconified()) {
            // Render frame
            this->render();

            // Call afterFrame()
            this->afterFrame();
        } else {
            this->waitMaximized();
        }
    }
}

void ao::vulkan::Engine::render() {
    vk::PipelineStageFlags pipeline_stage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    vk::Fence fence = this->fences[this->current_frame];

    // Wait fence
    this->device->logical()->waitForFences(fence, VK_TRUE, (std::numeric_limits<u64>::max)());

    // Prepare frame
    this->prepareFrame();

    // Call	beforeCommandBuffersUpdate()
    this->beforeCommandBuffersUpdate();

    // Update command buffers
    this->updateCommandBuffers();

    // Create submit info
    auto sem_index = (ao::vulkan::semaphore::GraphicProcess * this->swapchain->size()) + this->current_frame;
    vk::SubmitInfo submit_info(static_cast<u32>(this->semaphores->at(sem_index).waits.size()),
                               this->semaphores->at(sem_index).waits.empty() ? nullptr : this->semaphores->at(sem_index).waits.data(),
                               &pipeline_stage, 1, &this->swapchain->currentCommand(),
                               static_cast<u32>(this->semaphores->at(sem_index).signals.size()),
                               this->semaphores->at(sem_index).signals.empty() ? nullptr : this->semaphores->at(sem_index).signals.data());

    // Reset fence
    this->device->logical()->resetFences(fence);

    // Submit command buffer
    this->device->queues()->at(vk::to_string(vk::QueueFlagBits::eGraphics)).value.submit(submit_info, fence);

    // Submit frame
    this->submitFrame();

    // Increment frame index
    this->current_frame = (this->current_frame + 1) % this->swapchain->size();
}

void ao::vulkan::Engine::prepareFrame() {
    vk::Result result = this->swapchain->acquireNextImage(
        this->semaphores->at(ao::vulkan::semaphore::AcquireImage * this->swapchain->size() + this->current_frame).signals.front());

    // Check result
    if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR || this->enforce_resize) {
        LOG_MSG(warning) << "Swap chain is no longer compatible, re-create it";

        this->enforce_resize = false;
        return this->recreateSwapChain();
    }
    ao::vulkan::utilities::vkAssert(result, "Fail to get next image from swap chain");
}

void ao::vulkan::Engine::submitFrame() {
    vk::Result result = this->swapchain->enqueueImage(
        this->semaphores->at(ao::vulkan::semaphore::PresentImage * this->swapchain->size() + this->current_frame).waits);

    // Check result
    if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR || this->enforce_resize) {
        LOG_MSG(warning) << "Swap chain is no longer compatible, re-create it";

        this->enforce_resize = false;
        return this->recreateSwapChain();
    }
    ao::vulkan::utilities::vkAssert(result, "Fail to enqueue image");
}

vk::DebugUtilsMessageSeverityFlagsEXT ao::vulkan::Engine::validationLayersSeverity() const {
    return vk::DebugUtilsMessageSeverityFlagBitsEXT::eError | vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
           vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo;
}

VKAPI_ATTR VkBool32 VKAPI_CALL ao::vulkan::Engine::DebugCallBack(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
                                                                 VkDebugUtilsMessageTypeFlagsEXT type,
                                                                 const VkDebugUtilsMessengerCallbackDataEXT* callback_data, void* user_data) {
    // Format message
    std::string message = fmt::format("[{}] {}", to_string(vk::DebugUtilsMessageTypeFlagsEXT(type)), callback_data->pMessage);

    switch (vk::DebugUtilsMessageSeverityFlagBitsEXT(severity)) {
        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eError:
            LOG_MSG(error) << message;
            break;

        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo:
            LOG_MSG(info) << message;
            break;

        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose:
            LOG_MSG(trace) << message;
            break;

        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning:
            LOG_MSG(warning) << message;
            break;

        default:
            throw ao::core::Exception(
                fmt::format("Unknown vk::DebugUtilsMessageSeverityFlagBitsEXT: {}", to_string(vk::DebugUtilsMessageSeverityFlagBitsEXT(severity))));
    }
    return VK_FALSE;
}
