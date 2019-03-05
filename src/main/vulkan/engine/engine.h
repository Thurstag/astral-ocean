// Copyright 2018-2019 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <functional>
#include <mutex>
#include <tuple>
#include <vector>

#include <ao/core/exception/exception.h>
#include <ao/core/logger/logger.h>
#include <vulkan/vulkan.hpp>

#include "../container/pipeline_container.h"
#include "../container/semaphore_container.h"
#include "../utilities/vulkan.h"
#include "../wrapper/device.h"
#include "../wrapper/swapchain.h"
#include "settings.h"

namespace ao::vulkan {
    struct ValidationLayers {};

    /**
     * @brief Engine class
     *
     */
    class Engine {
       public:
        /**
         * @brief Construct a new Engine object
         *
         * @param settings Settings
         */
        explicit Engine(std::shared_ptr<EngineSettings> settings);

        /**
         * @brief Destroy the Engine object
         *
         */
        virtual ~Engine() = default;

        /**
         * @brief Run engine
         *
         */
        virtual void run();

        /**
         * @brief Clean-up vulkan stuff
         *
         */
        virtual void freeVulkan();

        /**
         * @brief Get settings
         *
         * @return std::shared_ptr<EngineSettings> Settings
         */
        std::shared_ptr<EngineSettings> settings() const {
            return this->settings_;
        }

       protected:
        core::Logger LOGGER = core::Logger::GetInstance<Engine>();
        std::shared_ptr<EngineSettings> settings_;
        std::atomic_bool enforce_resize;

        vk::DebugUtilsMessengerEXT debug_callBack;
        std::shared_ptr<vk::Instance> instance;
        std::shared_ptr<Swapchain> swapchain;
        std::shared_ptr<Device> device;
        SemaphoreContainer semaphores;
        PipelineContainer pipelines;
        vk::RenderPass render_pass;

        /**
         * @brief Init vulkan objects
         *
         */
        virtual void initVulkan();

        /**
         * @brief Prepare vulkan before running
         *
         */
        virtual void prepareVulkan();

        /**
         * @brief Set-up debugging (validation layers...)
         *
         */
        virtual void setUpDebugging();

        /**
         * @brief Create a Render Pass object
         *
         * @return vk::RenderPass Render pass
         */
        virtual vk::RenderPass createRenderPass() = 0;

        /**
         * @brief Re-create swapchain object
         *
         */
        virtual void recreateSwapChain();

        /**
         * @brief Create semaphores
         *
         */
        virtual void createSemaphores();

        /**
         * @brief Create pipelines
         *
         */
        virtual void createPipelines() = 0;

        /**
         * @brief Create vulkan buffers (UBO, vertex buffer...)
         *
         */
        virtual void createVulkanBuffers() = 0;

        /**
         * @brief Create a Surface object
         *
         * @return vk::SurfaceKHR Surface
         */
        virtual vk::SurfaceKHR createSurface() = 0;

        /**
         * @brief Init window
         *
         */
        virtual void initWindow() = 0;

        /**
         * @brief Clean-up window
         *
         */
        virtual void freeWindow() = 0;

        /**
         * @brief Window is iconified
         *
         * @return true Window is minimized
         * @return false Windows isn't minimized
         */
        virtual bool isIconified() const = 0;

        /**
         * @brief Run engine loop
         *
         */
        virtual void loop();

        /**
         * @brief Define looping condition
         *
         * @return true Continue to loop
         * @return false Stop loping
         */
        virtual bool loopingCondition() const = 0;

        /**
         * @brief Wait until window is maximized
         *
         */
        virtual void waitMaximized() = 0;

        /**
         * @brief Render a frame (prepare, process, submit)
         *
         */
        virtual void render();

        /**
         * @brief Prepare a frame
         *
         */
        virtual void prepareFrame();

        /**
         * @brief Submit a frame
         *
         */
        virtual void submitFrame();

        /**
         * @brief Update command buffers
         *
         */
        virtual void updateCommandBuffers() = 0;

        /**
         * @brief Called before command buffers update
         *
         */
        virtual void beforeCommandBuffersUpdate() = 0;

        /**
         * @brief Called after frame submission
         *
         */
        virtual void afterFrame() = 0;

        /**
         * @brief Called when swapchain is re-created
         *
         */
        virtual void onSwapchainRecreation(){};

        /**
         * @brief Define instance's extensions to enable
         *
         * @return std::vector<char const*> Extensions
         */
        virtual std::vector<char const*> instanceExtensions() const = 0;

        /**
         * @brief Define device's extensions to enable
         *
         * @return std::vector<char const*> Extensions
         */
        virtual std::vector<char const*> deviceExtensions() const {
            return {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
        }

        /**
         * @brief Define device features to enable
         *
         * @return std::vector<vk::PhysicalDeviceFeatures> Features
         */
        virtual std::vector<vk::PhysicalDeviceFeatures> deviceFeatures() const {
            return {};
        }

        /**
         * @brief Define queue requests
         *
         * @return std::vector<QueueRequest> Requests
         */
        virtual std::vector<QueueRequest> requestQueues() const {
            return {ao::vulkan::QueueRequest(vk::QueueFlagBits::eGraphics)};
        }

        /**
         * @brief Define validation layers severity
         *
         * @return vk::DebugUtilsMessageSeverityFlagsEXT
         */
        virtual vk::DebugUtilsMessageSeverityFlagsEXT validationLayersSeverity() const;

        /**
         * @brief Validation layers callback
         *
         * @param severity Severity
         * @param type Message type
         * @param callback_data Data
         * @param user_data User's data
         * @return Exit
         *
         */
        static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallBack(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type,
                                                            const VkDebugUtilsMessengerCallbackDataEXT* callback_data, void* user_data);

        /**
         * @brief Select a physical device (GPU)
         *
         * @param devices Devices
         * @return vk::PhysicalDevice Selected device
         */
        virtual vk::PhysicalDevice selectVkPhysicalDevice(std::vector<vk::PhysicalDevice> const& devices) const {
            return devices.front();  // First device
        }
    };
}  // namespace ao::vulkan
