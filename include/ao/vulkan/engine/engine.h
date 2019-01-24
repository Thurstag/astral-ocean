// Copyright 2018 Astral-Ocean Project
// Licensed under GPLv3 or any later version
// Refer to the LICENSE.md file included.

#pragma once

#include <execution>
#include <functional>
#include <mutex>
#include <tuple>
#include <vector>

#include <ao/core/exception/exception.h>
#include <ao/core/logger/logger.h>
#include <vulkan/vulkan.hpp>

#include "../utilities/vulkan.h"
#include "containers/semaphore_container.h"
#include "settings.h"
#include "wrappers/device.h"
#include "wrappers/pipeline.h"
#include "wrappers/swapchain.h"

namespace ao::vulkan {
    using DrawInCommandBuffer = std::function<vk::CommandBuffer(int, vk::CommandBufferInheritanceInfo const&,
                                                                std::pair<std::array<vk::ClearValue, 2> const&, vk::Rect2D> const&)>;

    /// <summary>
    /// Engine class
    /// </summary>
    class Engine {
       public:
        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="settings">Settings</param>
        explicit Engine(EngineSettings const& settings);
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
        EngineSettings const& settings() const;

       protected:
        core::Logger LOGGER = core::Logger::GetInstance<Engine>();

        std::vector<vk::Fence> waiting_fences;
        SemaphoreContainer semaphores;

        std::vector<vk::DescriptorSetLayout> descriptorSetLayouts;
        std::vector<vk::DescriptorPool> descriptorPools;
        std::vector<vk::DescriptorSet> descriptorSets;

        std::tuple<vk::Image, vk::DeviceMemory, vk::ImageView> stencil_buffer;
        std::vector<vk::Framebuffer> frames;  // TODO: MOVE TO SWAPCHAIN
        u32 frameBufferIndex = 0;             // TODO: MOVE TO SWAPCHAIN

        vk::DebugReportCallbackEXT debug_callBack;
        EngineSettings settings_;

        std::shared_ptr<vk::Instance> instance;
        std::shared_ptr<SwapChain> swapchain;
        std::shared_ptr<Pipeline> pipeline;
        std::shared_ptr<Device> device;
        vk::RenderPass renderPass;

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
        /// Method to create waiting fences
        /// </summary>
        virtual void createWaitingFences();
        /// <summary>
        /// Method to create stencil buffer
        /// </summary>
        virtual void createStencilBuffer();
        /// <summary>
        /// Method to create render pass
        /// </summary>
        virtual void createRenderPass();
        /// <summary>
        /// Method to set-up frame buffers
        /// </summary>
        virtual void setUpFrameBuffers();
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
        /// Method to set-up render pass
        /// </summary>
        virtual void setUpRenderPass() = 0;
        /// <summary>
        /// Method to set-up vulkan buffers
        /// </summary>
        /// <returns></returns>
        virtual void setUpVulkanBuffers() = 0;
        /// <summary>
        /// Method to create secondary command buffers
        /// </summary>
        virtual void createSecondaryCommandBuffers() = 0;

        /// <summary>
        /// Method to init surface
        /// </summary>
        /// <param name="surface">Surface reference</param>
        virtual void initSurface(vk::SurfaceKHR& surface) = 0;
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
        /// Method to update uniform buffers
        /// </summary>
        virtual void updateUniformBuffers() = 0;
        /// <summary>
        /// Method to get functions that will update secondary command buffers
        /// </summary>
        /// <returns>Function vector</returns>
        virtual std::vector<DrawInCommandBuffer> updateSecondaryCommandBuffers() = 0;

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
        /// Method to define queue flags
        /// </summary>
        /// <returns>Flags</returns>
        virtual vk::QueueFlags queueFlags() const;
        /// <summary>
        /// Method to define command pool flags
        /// </summary>
        /// <returns>Flags</returns>
        virtual vk::CommandPoolCreateFlags commandPoolFlags() const;
        /// <summary>
        /// Method to define debug report flags
        /// </summary>
        /// <returns>Flags</returns>
        virtual vk::DebugReportFlagsEXT debugReportFlags() const;

        /// <summary>
        /// Method to define a report callback
        /// </summary>
        /// <param name="flags">Flags</param>
        /// <param name="type">Type</param>
        /// <param name="srcObject">Source object</param>
        /// <param name="location">Location</param>
        /// <param name="msgCode">Message code</param>
        /// <param name="pLayerPrefix">Layer prefix</param>
        /// <param name="pMsg">Message</param>
        /// <param name="pUserData">Use data</param>
        /// <returns>True or False</returns>
        static VKAPI_ATTR VkBool32 VKAPI_CALL DebugReportCallBack(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT type, u64 srcObject,
                                                                  size_t location, s32 msgCode, const char* pLayerPrefix, const char* message,
                                                                  void* pUserData);

        /// <summary>
        /// Method to select a vk::PhysicalDevice
        /// </summary>
        /// <param name="devices">vk::PhysicalDevice</param>
        /// <returns>Index</returns>
        virtual size_t selectVkPhysicalDevice(std::vector<vk::PhysicalDevice> const& devices) const;  // TODO: Optimize this, return reference
    };
}  // namespace ao::vulkan
