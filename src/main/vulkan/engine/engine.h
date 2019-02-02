// Copyright 2018 Astral-Ocean Project
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

#include "../utilities/vulkan.h"
#include "container/semaphore_container.h"
#include "settings.h"
#include "wrapper/device.h"
#include "wrapper/pipeline.h"
#include "wrapper/swapchain.h"

namespace ao::vulkan {
    struct ValidationLayers {};

    /// <summary>
    /// Engine class
    /// </summary>
    class Engine {
       public:
        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="settings">Settings</param>
        explicit Engine(std::shared_ptr<EngineSettings> settings);
        /// <summary>
        /// Destructor
        /// </summary>
        virtual ~Engine();

        /// <summary>
        /// Methdo to run engine
        /// </summary>
        virtual void run();

        /// <summary>
        /// Method to get settings
        /// </summary>
        /// <returns>Settings</returns>
        std::shared_ptr<EngineSettings> settings() const;

       protected:
        core::Logger LOGGER = core::Logger::GetInstance<Engine>();
        std::shared_ptr<EngineSettings> settings_;

        // TODO: Refactor
        std::vector<vk::DescriptorSetLayout> descriptorSetLayouts;
        std::vector<vk::DescriptorPool> descriptorPools;
        std::vector<vk::DescriptorSet> descriptorSets;
        std::shared_ptr<Pipeline> pipeline;

        vk::DebugUtilsMessengerEXT debug_callBack;
        std::shared_ptr<vk::Instance> instance;
        std::shared_ptr<Swapchain> swapchain;
        std::shared_ptr<Device> device;
        SemaphoreContainer semaphores;
        vk::RenderPass render_pass;

        /// <summary>
        /// Method to init vulkan
        /// </summary>
        virtual void initVulkan();

        /// <summary>
        /// Method to prepare vulkan before running
        /// </summary>
        virtual void prepareVulkan();

        /// <summary>
        /// Method to clean-up vulkan
        /// </summary>
        virtual void freeVulkan();

        /// <summary>
        /// Method to set-up debugging
        /// </summary>
        virtual void setUpDebugging();

        /// <summary>
        /// Method to create render pass
        /// </summary>
        /// <returns>Render pass</returns>
        virtual vk::RenderPass createRenderPass() = 0;

        /// <summary>
        /// Method to re-create swap chain
        /// </summary>
        virtual void recreateSwapChain();

        /// <summary>
        /// Method to create pipelines
        /// </summary>
        virtual void createPipelines();

        /// <summary>
        /// Method to create semaphores
        /// </summary>
        virtual void createSemaphores();

        /// <summary>
        /// Method to create descriptor set layouts
        /// </summary>
        virtual void createDescriptorSetLayouts() = 0;

        /// <summary>
        /// Method to create descriptor pools
        /// </summary>
        virtual void createDescriptorPools() = 0;

        /// <summary>
        /// Method to create descriptor sets
        /// </summary>
        virtual void createDescriptorSets() = 0;

        /// <summary>
        /// Method to create pipeline layouts
        /// </summary>
        virtual void createPipelineLayouts() = 0;

        /// <summary>
        /// Method to set-up pipelines
        /// </summary>
        /// <returns></returns>
        virtual void setUpPipelines() = 0;

        /// <summary>
        /// Method to set-up vulkan buffers
        /// </summary>
        /// <returns></returns>
        virtual void createVulkanBuffers() = 0;

        /// <summary>
        /// Method to create secondary command buffers
        /// </summary>
        virtual void createSecondaryCommandBuffers() = 0;

        /// <summary>
        /// Method to create surface
        /// </summary>
        /// <returns>Surface</returns>
        virtual vk::SurfaceKHR createSurface() = 0;

        /// <summary>
        /// Method to init window
        /// </summary>
        virtual void initWindow() = 0;

        /// <summary>
        /// Method to clean-up window
        /// </summary>
        virtual void freeWindow() = 0;

        /// <summary>
        /// Method to know if window is iconified
        /// </summary>
        /// <returns></returns>
        virtual bool isIconified() const = 0;

        /// <summary>
        /// Method to define main loop
        /// </summary>
        virtual void loop();

        /// <summary>
        /// Method to define a loop condition
        /// </summary>
        /// <returns>True = continue to loop & False = stop looping</returns>
        virtual bool loopingCondition() const = 0;

        /// <summary>
        /// Method to wait until window is maximized
        /// </summary>
        virtual void waitMaximized() = 0;

        /// <summary>
        /// Method to render a frame
        /// </summary>
        virtual void render();

        /// <summary>
        /// Method to prepare frame
        /// </summary>
        virtual void prepareFrame();

        /// <summary>
        /// Method to submit frame
        /// </summary>
        virtual void submitFrame();

        /// <summary>
        /// Method to update command buffers
        /// </summary>
        virtual void updateCommandBuffers();

        /// <summary>
        /// Method called before command buffers update
        /// </summary>
        virtual void beforeCommandBuffersUpdate() = 0;

        /// <summary>
        /// Method called after frame submission
        /// </summary>
        virtual void afterFrame() = 0;

        /// <summary>
        /// Method called when swapchain is recreated
        /// </summary>
        virtual void onSwapchainRecreation(){};

        /// <summary>
        /// Method to execute secondary command buffers
        /// </summary>
        /// <param name="inheritanceInfo">Inheritance info</param>
        /// <param name="frameIndex">Current frame index</param>
        /// <param name="primaryCmd">Primary command buffer</param>
        virtual void executeSecondaryCommandBuffers(vk::CommandBufferInheritanceInfo& inheritanceInfo, int frameIndex,
                                                    vk::CommandBuffer primaryCmd) = 0;

        /// <summary>
        /// Method to get vkInstance extensions
        /// </summary>
        /// <returns>Extensions</returns>
        virtual std::vector<char const*> instanceExtensions() const = 0;

        /// <summary>
        /// Method to get device extensions that must be enabled
        /// </summary>
        /// <returns>Extensions</returns>
        virtual std::vector<char const*> deviceExtensions() const;

        /// <summary>
        /// Method to get device features that must be enabled
        /// </summary>
        /// <returns>Features</returns>
        virtual std::vector<vk::PhysicalDeviceFeatures> deviceFeatures() const;

        /// <summary>
        /// Method to define necessary queues
        /// </summary>
        /// <returns>Flags</returns>
        virtual std::vector<QueueRequest> requestQueues() const;

        /// <summary>
        /// Method to define validation layers severity
        /// </summary>
        /// <returns>Flags</returns>
        virtual vk::DebugUtilsMessageSeverityFlagsEXT validationLayersSeverity() const;

        /// <summary>
        /// Method to define validation layer callback
        /// </summary>
        /// <param name="severity">Severity</param>
        /// <param name="type">Type</param>
        /// <param name="callback_data">Callback's data</param>
        /// <param name="user_data">User's data</param>
        /// <returns>Abort</returns>
        static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallBack(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type,
                                                            const VkDebugUtilsMessengerCallbackDataEXT* callback_data, void* user_data);

        /// <summary>
        /// Method to select a device
        /// </summary>
        /// <param name="devices">devices</param>
        /// <returns>vk::PhysicalDevice</returns>
        virtual vk::PhysicalDevice selectVkPhysicalDevice(std::vector<vk::PhysicalDevice> const& devices) const;
    };
}  // namespace ao::vulkan
